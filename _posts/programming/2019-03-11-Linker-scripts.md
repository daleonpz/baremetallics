---
layout: post
title: Linker scripts 
category: programming
tags: [basics, linker]
---

I while ago I wrote a post about [linking basics]({% post_url programming/2017-02-11-linking-basics %}), where I talked about ELF files and their structure. 
This post is based on chapter 7 from the book [Computer Systems: A Programmer's Perspective ](https://www.amazon.com/Computer-Systems-Programmers-Perspective-3rd/dp/013409266X/ref=sr_1_1?ie=UTF8&qid=1490718500&sr=8-1&keywords=computer+systems) and this [document](http://www.scoberlin.de/content/media/http/informatik/gcc_docs/ld_toc.html)
 

# Object files
Linkers combines inputs files (relocatable files) into a single output file (executable file), this files are **object files**

Object files come in three forms:
- Relocatable object file. Contains binary code and data in a form that can be combined with other relocatable object files at compile time to create an executable object file.
- Executable object file. Contains binary code and data in a form that can be copied directly into memory and executed.
- Shared object file. A special type of relocatable object file that can be loaded into memory and linked dynamically, at either load time or run time.


Each object file has a list of sections:

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

## Note
 ELF executables are designed to be easy to load into memory, with contiguous chunks of the executable file mapped to contiguous memory segments. This mapping is described by the _segment header table_. That means that relocatable files **does not** have a segments header table.  

# Linker Scripting 
I will explain the `STM32F746NGHx_FLASH.ld` linker file found in the `STM32CubeF7`, because I think is the best way to understand this scripting language.

The basic skeleton is as follows:


```text
/* Entry Point */
ENTRY(Reset_Handler)

/* Specify the memory areas */
MEMORY
{
}

/* Define output sections */
SECTIONS
{
}
``` 

You can notice three linker commands:  
- `ENTRY` command defines the location where the program execution starts. It is at the beginning of `.text`. In this case the program will start at the symbol `Reset_Handler`.
- `MEMORY` command describes the location and size of blocks of memory in the target. You can use it to describe which memory regions may be used by the linker, and which memory regions it must avoid.
- `SECTIONS` command tells the linker how to map input sections into output sections, and how to place the output sections in memory.  

## MEMORY command
The syntax of this command is:

```text
MEMORY
  {
    name (attr) : ORIGIN = origin, LENGTH = len
    ...
  }
```


The memory mapping described in the [datasheet](https://www.st.com/resource/en/datasheet/DM00166116.pdf) shows the addresses of the memories. `RAM` starts at 0x20000000 and has a length of `320KB` which means that the end of the stack is at 0x20050000,  while the 1MB `FLASH` (with AXIM interface)  starts at 0x8000000.  
Thus the memory regions are defined as:

```text
/* Highest address of the user mode stack */
_estack = 0x20050000;    /* end of RAM */


/* Specify the memory areas */
MEMORY
{
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 320K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
}

``` 

Note that `RAM` has three attributes:  contain executable code (x), read (r) and write (w). On the other hand, `FLASH` is a write (w) section and contain executable code (x).

 

## SECTIONS command
 The format of this command is:

```text
SECTIONS
{
  sections-command
  sections-command
  ...
}
```

and the full description of an output section looks like this: 

```text
section [address] [(type)] : [AT(lma)]
  {
    output-section-command
    output-section-command
    ...
  } [>region] [:phdr :phdr ...] [=fillexp]
```

Before we start, I have to mention that every output section has two addresses:
- A **virtual memory address**, which is the address the section will have when the code runs
- A **load memory address**, where the code lives


There are two memory regions defined: RAM and FLASH. RAM is volatile memory, and hence it is not possible to directly make the data available in RAM, on power up.
Thus, all code and data should be stored in Flash before power-up


The main output sections in the linker file are:

 ![output sections](/images/posts/linker-scripts-1.png)

If we translate this table to linker scripting language: 

```text
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

    /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

}
``` 


Things to note:
- `.`: is the **location counter**, and  always refers to a location in an output section
- `symbol = expression`: assign _expression_ to this _symbol_
- `ALIGN(exp)`:  Return the location counter (.) aligned to the next `exp` boundary. `exp` must be an expression whose value is a power of two. 
- `>RAM AT> FLASH`: The linker will normally set the LMA equal to the VMA. You can change that by using the `AT` keyword. The expression lma that follows the AT keyword specifies the load address of the section 
- `*(pattern)`: `*` is a [wildcard pattern](http://www.scoberlin.de/content/media/http/informatik/gcc_docs/ld_3.html#SEC23). For example: `*(.text)` means _put here all the .text sections_. 
- `KEEP(symbol)`: is a command keeps the symbol, when link-time garbage collection `--gc-sections` is in use 

In conclusion, the job of the linker can be summarized as follows:

![Executable file](/images/posts/linker_script.png)


# Final words
If there are files such as `STM32F746NGHx_FLASH.ld`, why should I bother to learn linker scripting language? 

- Understanding linkers will help you to manage different input object files from different sources (C, ADA, assembler) into a single executable file. 
- Understanding linking will help you understand some computers works, how the code is run.

I hope this guide is helpful ;)


