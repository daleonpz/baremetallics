---
layout: post
title: Linking basics
category: programming
tags: [ basics, binary, compiler ]
---

In my journey to master the most obscure and secrets arts of computer systems, I started by mastering the basics. 
One thing I have never worried to learn properly was *linking*, since most of the projects I worked on had everything set, `Makefile` was already written.
But the time to grow up becomes.
Let's start then.a
I took the examples from 
[Computer Systems: A Programmer's Perspective ](https://www.amazon.com/Computer-Systems-Programmers-Perspective-3rd/dp/013409266X/ref=sr_1_1?ie=UTF8&qid=1490718500&sr=8-1&keywords=computer+systems)


# ELF object file

The ELF file contains a _header_ that give us information that will help the linker to parse and interpret the object file. 
Here are some information of an ELF header, I used the `readelf` command.

```
ELF Header:
  Class:                    ELF64
  Data:                     2's complement, little endian
  Version                   1 (current)
  OS/ABI:                   UNIX - System V
  Type:                     EXEC (Executable file)
  Machine                   Advanced Micro Devices X86-64
  Entry point address:      0x4003b0
  Start of program headers: 64 (bytes into file)
  Start of section headers: 6568 (bytes into file)
  Size of this header:      64 (bytes)
  Size of program headers:  56 (bytes)
  Number of program headers:9
  Size of section headers:  64 (bytes)
  Number of section headers:28
```

The basic structure of an ELF file is as follow:

![ELF object file](/images/posts/linking-basics-1.png)
*Image obtained from this [site](https://people.cs.pitt.edu/~xianeizhang/notes/Linking.html)*

* `.init` is the initialization code before the `main` in C such as set to zero global variables or defined the interrupt vector table. 
* `.text` is the machine code of the compiled program
* `.rodata` read-only data, such as `const char`
* `.data`  global variables that have been initialized
* `.bss`  uninitialized global variables
* `.symtab` table symbol with information about functions and global variables that are defined and referenced in the program
* `.debug` debugging symbol table, only generated when compiling with `-g`
* `.line` mapping between actual C code and compiled code, only generated when compiling with `-g`
* `.strtab` a string table for symbol in `.symtab` and `.debug`

## Example
Let's analyze the following example
```c
// elf.c
#include<stdio.h>

int A = 5;
const char a[] = "hallo danke"; 

int main()
{
	printf("hello world\n"); 
	printf("%i\n",A);  
        printf("%s\n",a);  
	return 0;
}

```
I compiled it with `gcc -c` since I just wanted to focus on the code itself.
Let's check the `.rodata` and see what it contains, I used `objdump -D ` to get the assembly code of that section.

```nasm
Disassembly of section .rodata:

0000000000000000 <a>:
   0:	68 61 6c 6c 6f  h a l l o
   5:	20 64 61 6e       d a n
   9:	6b 65 00 68     k e 0 h
   d:	65 6c           e l
   f:	6c              l
  10:	6f              o
  11:	20 77 6f          w o
  14:	72 6c           r l
  16:	64              d
  17:	00              0 
  18:	25              %
  19:	69              i
  1a:	0a 00           ⤶ 0

```

In case it is no obvious, I added the letters using the ASCII code. As I mentioned before `.rodata` only contains read only data. 

Now let's focus on the `.symtab`. 
To do so, let's compile the code without the `-c` option.
I used `objdump -x` to visualize the `.symtab`. Some  entries of the `.symtab` are shown below.

```  
400546 g     F .text	0000036      main
400608 g     O .rodata	000000c      a
000000       F *UND*	0000000      printf@@GLIBC_2.2.5
```

Description of the columns from left to right.
* virtual memory offset 
* unit size, in the case of `main` and `a` is *g*, it stands for _giant words_ (8 bytes)
* type, `F` and `O` stand for _function_ and _object_ respectively
* section where it belongs, for example `main` belongs to `.text`
* size in hex
* name, in case of `printf`, it is also mentioned the shared library where it is defined.

So these are the basics of the ELF files. If want to read more just google it or read the man pages of the commands I used, I am not kidding. You can also read the chapter 7 of book it mentioned above.





# Symbol Resolution
The compiler associates each symbol reference with one symbol definition in the ELF file. The compiler allows one only ones definition of each local symbol per module. It also ensures that static local variables have unique names.
In case of global symbols such as variables or functions the compiler has to decide which reference to use, and thus symbol resolution maybe be tricky.

## Example 1
In `symbol1.c`, the symbol `x` has been defined as `int` and it has a reference inside the `printf`.

```c
// symbol1.c
#include <stdio.h>

int x = 15213;
int y = 15212;

int main(){
	printf("x = %i \t y = %i \n", 
			x,y);
	return 0;
}
```

```c
x = 15213 	 y = 15212
```

But what would happen if we defined the  symbol `x` somewhere else:

```c
// symbol2.c
double x;

void f(){
	x = -0.0;
}
```

```c
// symbol3.c
#include <stdio.h>

void f(void);

int x = 15213;
int y = 15212;

int main(){
	f();
	printf("x = 0x%x\t y = 0x%x \n", 
			x,y);
	return 0;
}
```
I compiled it as follow `gcc -o test symbol3.c symbol2.c`, and this is the result.

```c
x = 0x0 	 y = 0x80000000 
```

What had happened? 

In `symbol3` both `x` and `y` have been declared as `int`, but in `symbol2.c` `x` has been declared as `double`. So in memory is something like this.

```c
// symbol3.c
0000    X X X X   // 4 bytes
0004    Y Y Y Y   // 4 bytes
0008    ...

// symbol2.c
0000    X X X X X X X X   // 8 bytes
        0 0 0 0 0 0 0 8   // -0.0
0008    ...
```

Thus, the assignment of `x` as `double` overwrites the memory location of `int x` and `int y`. And that's a nasty bug.  


## Example 2

Let's defined a function `p()` and a `char main` in `symbol4.c`.
```c
//symbol4.c
#include <stdio.h>

char main;

void p(){
	printf("0x%x\n",*(&main+1));
	printf("0x%x\n",main);
}
```
And call `p()` in another file `symbol5.c`

```c
//symbol5.c
void p(void);

int main(){
	p();
	return 0;
}
```

The result of compiling as follow `gcc -o test symbol5.c symbol4.c` is:

```c
// output
0x48
0x55
```

Why did `p()` print something even though `main` has not been initialized?

With the help of `objdump -x test` let's see if `main` is defined in first place:

```
4004f6 g     F .text	000010       main
```

It is defined as _function_, this is because `char main` is a weak symbol (it has been declared but not defined) whereas `int main() {...}` is a strong symbol. Thus, _function_ `main` overrides `char main`.

Now it's time to explain the output. I use `objdump -D` to see the opcode of _main_. 

```nasm
00000000004004f6 <main>:
  4004f6:	55            	push   %rbp
  4004f7:	48 89 e5      	mov    %rsp,%rbp
  4004fa:	e8 07 00 00 00	callq  400506 <p2>
  4004ff:	b8 00 00 00 00	mov    $0x0,%eax
  400504:	5d            	pop    %rbp
  400505:	c3
```

It is easy to observed that `printf("0x%x\n",*(&main+1));` prints what is in address `4004f7` (0x48), and `printf("0x%x\n",main);` prints what is in address `4004f6` (0x55).

# Final thoughts
* Always compile your code with `-W` and `-Wall` flags to avoids these types of "unexpected" behaviors.
* `objdump` is a very useful tool to understand what is going on under the hood, but it can be a pain in the ass if the code is very large. 
* Even though I didn't use `gdb` in this post, I highly recommend to learn it.
