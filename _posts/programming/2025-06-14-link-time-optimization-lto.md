---
layout: post
title: Link Time Optimization (LTO) - Shrinking Embedded Firmware
category: programming
description: Link Time Optimization (LTO) is a powerful technique that can significantly reduce the size of embedded firmware without code changes.
tags: [zephyr, arm, kconfig, linking, lto, optimization]
---

I'm currently building a firmware application in Zephyr for the STM32WB55, a microcontroller with limited flash space (~400KB when using MCUBoot). While exploring kernel configuration options, I came across the Link Time Optimization (LTO).

Enabling it was as simple as enabling a couple of config flags:

```sh
CONFIG_ISR_TABLES_LOCAL_DECLARATION=y
CONFIG_LTO=y
```

This alone reduced the firmware size by 4%, with zero code changes  🤯.

* 
{:toc}

# Before and After: Size Comparison

Without LTO:

```sh
FLASH:     136784 B / 401072 B   (34.10%)
RAM:        45924 B / 192 KB     (23.36%)
```

With LTO:

```sh
FLASH:     120956 B / 401072 B   (30.16%)
RAM:        45860 B / 192 KB     (23.33%)
```

# What is Link Time Optimization (LTO)?

LTO is a type of interprocedural optimization done at the linking stage, not during regular compilation. I know, I know. It sounds confusing, but stick with me.

Normally, compilers treat each source file as an independent unit. With LTO, however, the compiler considers **all translation units together**, enabling more aggressive and global optimizations, such as inlining and dead code elimination across module boundaries.

# How it works:

**Without LTO:**

- Each `.c` file is compiled into its own object (`.o`) file.
- The linker stitches them together into a final binary.

**With LTO:**

- Each file is compiled into [LLVM bitcode](https://llvm.org/docs/BitCodeFormat.html), a bitstream format, instead of regular object files.
- The linker passes these bitcode files to an LTO optimizer (e.g., libLTO).
- The optimizer performs cross-module analysis and emits a **highly optimized object file**.
- The final binary is linked as usual, but with all the global optimizations included.

# Potential Benefits of LTO

- **Smaller binary size**: As seen in my project, immediate 4% reduction in flash usage.
- **Better inlining**: Frequently called small functions are embedded directly at call sites.
- **Dead code removal**: Unused functions or variables across files are removed.
- **Improved runtime performance**: Especially in tight loops or performance-critical code.

# Drawbacks and Limitations

Nothing is perfect, and LTO has some trade-offs:

- **Static libraries must be archived using compatible tools** (e.g., armllvm-ar).
- **Partial linking is not supported** — all object files must be linked at once.
- **Version compatibility**: Bitcode files must be generated using the same compiler version.
- **Stack size impact**: Increased inlining may increase stack usage. You may need to increase stack size or [disable inlining for large functions](https://software-dl.ti.com/codegen/docs/tiarmclang/compiler_tools_user_guide/compiler_manual/c_cpp_language_implementation/attributes/function_attributes.html#cm-noinline-function-attribute).

# LTO in Action: A Minimal Example
This simple benchmark shows a real improvement in runtime thanks to LTO:

```c
/* main.c */
#include <stdio.h>
#include <time.h>

extern int square(int x);

int main() {
    int sum = 0;
    clock_t start = clock();
    for (int i = 0; i < 100000; i++) {
        sum += square(i);
    }
    clock_t end = clock();
    printf("Sum: %d\n, Time taken: %f seconds\n", sum, (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

/* math_utils.c */
int square(int x) {
        return x * x;
}
```

With `-flto`, `square()` is inlined and optimized out — the result: faster execution and smaller binary.

```sh
$ ./program_no_lto
Sum: 216474736 , Time taken: 0.000073 second

$ ./program_lto
Sum: 216474736 , Time taken: 0.000024 seconds
```

Binary sizes:

```sh
-rwxr-xr-x 1 user user 15760 May 24 20:52 program_no_lto
-rwxr-xr-x 1 user user 15664 May 24 20:52 program_lto
```

# Disassembly Comparison

LTO significantly simplifies the machine code by inlining and optimizing loops. Here's a snippet showing how `square()` is removed in the LTO version and replaced by an inline multiply.

**No LTO:**

```c
$ objdump -D example/program_no_lto
0000000000401050 <main>:
  401050:	41 54                	push   %r12
  401052:	55                   	push   %rbp
  401053:	31 ed                	xor    %ebp,%ebp
  401055:	53                   	push   %rbx
  401056:	31 db                	xor    %ebx,%ebx
  401058:	e8 d3 ff ff ff       	call   401030 <clock@plt>
  40105d:	49 89 c4             	mov    %rax,%r12

  /* Loop to calculate sum of squares */
  401060:	89 df             j   	mov    %ebx,%edi
  401062:	83 c3 01             	add    $0x1,%ebx
  401065:	e8 36 01 00 00       	call   4011a0 <square> /* call to square() */
  40106a:	01 c5                	add    %eax,%ebp
  40106c:	81 fb a0 86 01 00    	cmp    $0x186a0,%ebx ; /* 0x186a0 = 100000 */
  401072:	75 ec                	jne    401060 <main+0x10>
  /* End of loop */

  401074:	e8 b7 ff ff ff       	call   401030 <clock@plt>
  401079:	66 0f ef c0          	pxor   %xmm0,%xmm0
  40107d:	89 ea                	mov    %ebp,%edx
  40107f:	bf 02 00 00 00       	mov    $0x2,%edi
  401084:	4c 29 e0             	sub    %r12,%rax
  401087:	48 8d 35 7a 0f 00 00 	lea    0xf7a(%rip),%rsi        # 402008 <_IO_stdin_used+0x8>
  40108e:	f2 48 0f 2a c0       	cvtsi2sd %rax,%xmm0
  401093:	b8 01 00 00 00       	mov    $0x1,%eax
  401098:	f2 0f 5e 05 90 0f 00 	divsd  0xf90(%rip),%xmm0        # 402030 <_IO_stdin_used+0x30>
  40109f:	00 
  4010a0:	e8 9b ff ff ff       	call   401040 <__printf_chk@plt>
  4010a5:	5b                   	pop    %rbx
  4010a6:	31 c0                	xor    %eax,%eax
  4010a8:	5d                   	pop    %rbp
  4010a9:	41 5c                	pop    %r12
  4010ab:	c3                   	ret
  4010ac:	0f 1f 40 00          	nopl   0x0(%rax)

  /* Function square */
00000000004011a0 <square>:
  4011a0:	0f af ff             	imul   %edi,%edi
  4011a3:	89 f8                	mov    %edi,%eax
  4011a5:	31 ff                	xor    %edi,%edi
  4011a7:	c3                   	ret

```

**With LTO:**

```c
$ objdump -D example/program_lto
0000000000401060 <main>:
  401060:	55                   	push   %rbp
  401061:	53                   	push   %rbx
  401062:	31 db                	xor    %ebx,%ebx
  401064:	48 83 ec 08          	sub    $0x8,%rsp
  401068:	e8 c3 ff ff ff       	call   401030 <clock@plt>
  40106d:	48 89 c5             	mov    %rax,%rbp
  401070:	31 c0                	xor    %eax,%eax 
  401072:	66 66 2e 0f 1f 84 00 	data16 cs nopw 0x0(%rax,%rax,1) #    
  401079:	00 00 00 00 
  40107d:	0f 1f 00             	nopl   (%rax)

  /* Loop to calculate sum of squares */
  401080:	89 c2                	mov    %eax,%edx  /* edx = eax  (temp = i) */
  401082:	0f af d0             	imul   %eax,%edx  /* multiplication  edx = edx * eax (temp*i = i*i) */
  401085:	83 c0 01             	add    $0x1,%eax  /*  eax = eax + 1 (i++) */
  401088:	01 d3                	add    %edx,%ebx  /* ebx = ebx + edx (ebx + i*i ) , ebx = sum */
  40108a:	3d a0 86 01 00       	cmp    $0x186a0,%eax  /* 0x186a0 = 100000 */
  40108f:	75 ef                	jne    401080 <main+0x20>
  /* End of loop */

  401091:	e8 9a ff ff ff       	call   401030 <clock@plt>
  401096:	66 0f ef c0          	pxor   %xmm0,%xmm0
  40109a:	89 da                	mov    %ebx,%edx 
  40109c:	bf 02 00 00 00       	mov    $0x2,%edi
  4010a1:	48 29 e8             	sub    %rbp,%rax
  4010a4:	48 8d 35 5d 0f 00 00 	lea    0xf5d(%rip),%rsi        # 402008 <_IO_stdin_used+0x8>
  4010ab:	f2 48 0f 2a c0       	cvtsi2sd %rax,%xmm0
  4010b0:	b8 01 00 00 00       	mov    $0x1,%eax
  4010b5:	f2 0f 5e 05 73 0f 00 	divsd  0xf73(%rip),%xmm0        # 402030 <_IO_stdin_used+0x30>
  4010bc:	00 
  4010bd:	e8 7e ff ff ff       	call   401040 <__printf_chk@plt>
  4010c2:	48 83 c4 08          	add    $0x8,%rsp
  4010c6:	31 c0                	xor    %eax,%eax
  4010c8:	5b                   	pop    %rbx
  4010c9:	5d                   	pop    %rbp
  4010ca:	c3                   	ret
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
```

The inlining is clear in the assembly code. Without LTO, there is a function call at `0x401065` to `square()`, but with LTO, that function call is replaced at `0x401082` with

```c
/* From the LTO version */
call   4011a0 <square> 

/* To Inline multiplication */
mov %eax, %edx   -> edx = eax
imul %eax, %edx  -> edx = edx * eax = edx * edx
```

# Final Thoughts

LTO is a simple optimization that has in most cases only benefits in embedded development:

- Smaller firmware
- Faster execution

# References

- [Arm - What is Link Time Optimization (LTO)](https://developer.arm.com/documentation/101458/2404/Optimize/Link-Time-Optimization--LTO-/What-is-Link-Time-Optimization--LTO-)
- [TI - Link Time Optimization - LTO](https://software-dl.ti.com/codegen/docs/tiarmclang/compiler_tools_user_guide/compiler_manual/link_time_optimization/index.html)
- [LTO in Zephyr](https://www.zephyrproject.org/welcome-zephyr-3-6/)

# Extras

Here is the Makefile used to build the example:

```Makefile
# Compiler and flags
CC = gcc
CFLAGS = -O2
LTO_FLAGS = -flto

# Directories
EXAMPLE_DIR = example

EXAMPLE_SRC = $(EXAMPLE_DIR)/main.c $(EXAMPLE_DIR)/math_utils.c
EXAMPLE_OBJ = $(EXAMPLE_SRC:.c=.o)
EXAMPLE_OBJ_LTO = $(EXAMPLE_SRC:.c=_lto.o)

# Targets
all: example1_no_lto example1_lto

# Example 1: Without LTO
example1_no_lto: $(EXAMPLE_OBJ)
	$(CC) $(EXAMPLE_OBJ) -o $(EXAMPLE_DIR)/program_no_lto

# Example 1: With LTO
example1_lto: $(EXAMPLE_OBJ_LTO)
	$(CC) $(LTO_FLAGS) $(EXAMPLE_OBJ_LTO) -o $(EXAMPLE_DIR)/program_lto

# Compile source files to object files (without LTO)
$(EXAMPLE_DIR)/%.o: $(EXAMPLE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files to object files (with LTO)
$(EXAMPLE_DIR)/%_lto.o: $(EXAMPLE_DIR)/%.c
	$(CC) $(CFLAGS) $(LTO_FLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(EXAMPLE_DIR)/*.o $(EXAMPLE_DIR)/program_*
```
