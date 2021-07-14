---
layout: post
title: Minimalist Assert Function for Embedded C 
category: programming
description: minimalist assert function for embedded c
---

Asserts is a great tool to find errors, bugs, or unintended behaviors on runtime. If implemented correctly, asserts give developer context information about when and where in the code an issue showed up.

In this post, I will discuss a minimalist assert implementation in C suitable for bare-metal implementations. 

* 
{:toc}

# Disclaimer
- You should apply this information only to non-safety-critical embedded systems.
- This post is based on a [post by Memfault](https://interrupt.memfault.com/blog/asserts-in-embedded-systems).
- I run the examples on my laptop because when I wrote this post I was on vacation, but the ideas are still valid for any architecture. 

# Requirements 
You will need a data structure to save all the **Program Counter (PC)** and **Linker Register (LR)**.
- The PC is  the  current  program  address.  This  register  can  be  written  to  control  the program flow.
- The LR register that stores the return address, such as when making a function call. 

For example, in the following code,

```c
    int y = 0;
    y = 2; 
    y = funcA();
    printf("y = %i\r\n", y);
```

in the function call `y = funcA()`, one the code is inside `funcA()`.  The value of LR will be the address of `y = funcA()`. You can verify it looking at dissambly with, for example, `objdump -D`.  If you want to know how? Read this [post]({% post_url programming/2017-03-30-hack-main %}).

In order to save LR and PC, I created a struct called `sAssertInfo` and defined a function `my_assert()`. This function will save the values of LR and PC in variable `g_assert_info`. 

```c
typedef struct sAssertInfo {
    uint64_t pc;
    uint64_t lr;
} sAssertInfo;

sAssertInfo g_assert_info; 

void my_assert(const uint64_t *pc, const uint64_t *lr) {
    g_assert_info.pc = pc;
    g_assert_info.lr = lr;
    while(1){};
}
```

**Note** that there is a `while(1){}` at the end of my function. For debugging purposes is useful, because the code will stop there and that allows you to solve one problem at a time.
That means, for production, you may use a recovery and logging mechanism. 

# Base Implementation 

Let's start with the C file. **Note** that `my_assert()` needs `uint64_t` pointers. You should change them based on your architecture. For example for ARM 32-bit chips, you should use `uint32_t` for the PC and LR pointers.

## *.c File
```c
// asserts.c
#include "my_asserts.h"
#include <strings.h>

static sAssertInfo g_assert_info;

void my_assert(const uint64_t *pc, const uint64_t *lr) {
    g_assert_info.pc = pc;
    g_assert_info.lr = lr; 
    while(1){};
}
```

Now let's write the header file, here is where the magic happens.

## *.h File
I used macros for readability purposes, because the code for reading the PC and LR involves assembly.

```c
#pragma once

#ifndef _MY_ASSERTS_H_ 
#define _MY_ASSERTS_H_ 

#include <stdint.h>

typedef struct sAssertInfo {
    uint64_t pc;    
    uint64_t lr;   
} sAssertInfo;

void my_assert(const uint64_t *pc, const uint64_t *lr);

#define GET_LR() __builtin_return_address(0)
// #define GET_PC(_a) __asm volatile ("mov %0, pc" : "=r" (_a)) // Comment out this for ARM
#define GET_PC(_a) __asm volatile("1: lea 1b(%%rip), %0;": "=a"(pc));  // Comment this for x86

#define MY_ASSERT_RECORD()       \
    do {                         \
      void *pc = NULL;           \
      GET_PC(pc);            \
      const void *lr = GET_LR(); \
      my_assert(pc, lr);         \
    } while (0)

#define ASSERT_EXP(exp)                          \
    do {                                        \
        if (!(exp)) { MY_ASSERT_RECORD(); }     \
    } while (0)

#endif /* _MY_ASSERTS_H_ */
```

Things to notice:
- `#pragma once`. From [Wikipedia](https://en.wikipedia.org/wiki/Pragma_once): "is a non-standard but widely supported preprocessor directive designed to cause the current source file to be included only once in a single compilation. Thus, `#pragma once` serves the same purpose as `#include` guards, but with several advantages, including: less code, avoiding name clashes, and improved compile speed."

- `void *pc`. It's to take advantage that a void pointer can hold address of any type and it can be really useful if the you are not sure about the data type.

- `while(0)` at the end of each `#define`. The reason is good explained in [stackoverflow](https://stackoverflow.com/questions/257418/do-while-0-what-is-it-good-for). Basically, it helps to group multiple statements into a single one so that a function-like macro can actually be used as a function.

# Simple Example

I packed the assert library with the example code in one file for education purposes. I was lazy, that's the truth :).

There are two functions `funcA()` and `funcB()`.  `funcB()` calls `funcA()`, and this one contains a failing assert. `MY_ASSERT(0)` means that the code will crash there, because 0 is interpreted as false .

```c
#include <stdio.h>
#include <inttypes.h>

#define GET_LR() __builtin_return_address(0)
// #define GET_PC(_a) __asm volatile ("mov %0, pc" : "=r" (_a))
#define GET_PC(_a) __asm volatile("1: lea 1b(%%rip), %0;": "=a"(pc)); 

// void pointer -> generic pointer to capture the register value
#define MY_ASSERT_RECORD()     \
    do {                         \
        void *pc = NULL;               \
        GET_PC(pc);                 \
        const void *lr = GET_LR(); \
        my_assert(pc, lr);         \
    } while (0)

#define MY_ASSERT(exp)         \
    do {                         \
        if (!(exp)) {              \
            MY_ASSERT_RECORD();      \
        }                          \
    } while (0)


typedef struct sAssertInfo {
    uint64_t pc;
    uint64_t lr;
} sAssertInfo;

sAssertInfo g_assert_info;

void my_assert(const uint64_t *pc, const uint64_t *lr) {
    g_assert_info.pc = pc;
    g_assert_info.lr = lr;
    while(1){};
}


uint64_t x = 0;
uint64_t funcA(void)
{
    x = 8;
    MY_ASSERT(0);
    return x;
}

uint64_t funcB(void)
{
    uint64_t y = 0;
    y = funcA();
    return y;
}


uint64_t main ()
{
    funcB();
}
```

Let's compile the code:

```sh
$ gcc -g -m64 main.c
```

I use **gdb** to get the values of PC/LR held by `g_assert_info`.

```sh
$ gdb a.out
# Inside GDB run  and then ctrl-D

GNU gdb (Ubuntu 8.1.1-0ubuntu1) 8.1.1
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from a.out...done.
(gdb) run
Starting program: /home/me/Documents/git/t/a.out 
^C


Program received signal SIGINT, Interrupt.
my_assert (pc=0x555555554639 <funcA+27>, lr=0x55555555467d <funcB+21>) at main.c:35
35          while(1){};
(gdb) quit

```

As you can see, the function `my_assert()` is called and gdb also prints the PC/LR. 

```sh
my_assert (pc=0x555555554639 <funcA+27>, lr=0x55555555467d <funcB+21>) at main.c:35
```

That means that: PC was 0x639 and LR was 0x67d, when the program crashed due to `MY_ASSERT(0)`.

## Retrieving Code Info

With the tool `addr2line` you can convert addresses into file names and line numbers.

Let's start with the PC:

```sh
addr2line -e a.out 0x639
/home/me/main.c:43

$  sed -n '43,1p' main.c
    MY_ASSERT(0);
```

And now the LR:

```sh
$  addr2line -e a.out 0x67d
/home/me/main.c:50

sed -n '50,1p' main.c
    y = funcA();

```

As expected, PC is the address where the code stopped, and LR is where `funcA()` was called.

# Conclusion
Asserts is a great tool to find errors, bugs or unintended behaviors on runtime. If implemented correctly, asserts give developer context information about when and where in the code an issue showed up.

I discussed a minimalist assert implementation in C suitable for baremetal implementations. It's not as straightforward as a simple `printf`, but it way more lightway and most likely useful.


