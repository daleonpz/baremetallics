---
layout: post
title: Embedded Ada Journey 003 - Project structure
category: programming
tags: [ada, stm32]
---

It's more than year that I stopped learning Ada, but I'm back!!!. 
Anyway, it's really important for me to understand what tools can I use and how to use them, before even start coding in Ada.
There are some basic concepts/ tools that I had to learn the hard way.

# Basic file type
Ada language uses  `.ads` and `.adb` files to organize the packages. 

* `.ads` are the **specification** files, similar to `.h` in C
* `.adb` are the **implementation** files, similar to `.c` in C

For example, a folder structure can be as follows: 

```sh
.
├── build
├── common
│   ├── stm32f40x.ads
│   ├── stm32f40x-gpio.ads
│   └── stm32f40x-rcc.ads
└── src
    └── main.adb
```

# The compiler GNAT
GNAT is the official compiler for Ada, which is part of the GNU Compiler collection or GCC for friends. 
 Three steps are needed to create an executable file from an Ada source file:

1. The source file must first be compiled.
2. The source file then must be bound using the GNAT binder.
3. All appropriate object files must be linked to produce an executable
  
To learn more check the [User's Guide](https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gnat_ugn_unw/)

# GNAT Project Manager 
It's a tool that allows you to manage complex builds involving several source files or libraries, also can be customize using options defined by the user. The coolest thing is that can handle C and C++. 

Basically it's a replacement for  Makefile that is waaaaaaaaaaaaaaaay easier to understand. 
Just check this example out:

```ruby
project myProject is
   for Main use ("src/main.adb");
   for Source_Dirs use ("src","common");
   for Runtime ("ada") use "ravenscar-full-stm32f4";
   for Target use "arm-eabi";
   for Languages use ("Ada");
   for Object_Dir use "build";
   for Create_Missing_Dirs use "True";
end myProject;

```

Even if you have no idea about Ada you may able to understand what going on. But with Makefile I always need a [Cheatsheet](https://github.com/rstacruz/cheatsheets/blob/master/makefile.md).

More info about GNAT project manager:
- Let's get started [link](https://www.adacore.com/gems/gem-65)
- Intro to GPR [link](https://people.cs.kuleuven.be/~dirk.craeynest/ada-belgium/events/09/090207-fosdem/02a-gnat-project-facility.pdf)
- Official documentation [link](https://docs.adacore.com/gprbuild-docs/html/gprbuild_ug/gnat_project_manager.html)

# Full Project
Here is my project example:

```sh
├── ProjectFolder
│   ├── build
│   ├── common
│   │   ├── stm32f40x.ads
│   │   ├── stm32f40x-gpio.ads
│   │   └── stm32f40x-rcc.ads
│   ├── myProject.gpr
│   ├── README.md
│   └── src
│       └── main.adb

```

* `build` is where my binaries will be.
* `common` is where the files I _include_ in my main file.
* `myProject.gpr` is my gprbuilder for this project, and it's the same file as in the section above.
* `README.md` is just the readme file of this project
* `src` is where my source files are. In this case I only have one file `main.adb`.

In order to compile this code I need to use `gprbuild`.
Since there is just one `.gpr` it's enough to run `gprbuild` but if you have several `.gpr` files, then you should specify which one you will use.

Running only `gprbuild`

```sh
$ gprbuild
using project file myProject.gpr
Compile
   [Ada]          main.adb
   [Ada]          stm32f40x.ads
   [Ada]          stm32f40x-gpio.ads
   [Ada]          stm32f40x-rcc.ads
Bind
   [gprbind]      main.bexch
   [Ada]          main.ali
Link
   [link]         main.adb

```

Giving a project file:

```sh
$ gprbuild myProject.gpr
Compile
   [Ada]          main.adb
   [Ada]          stm32f40x.ads
   [Ada]          stm32f40x-gpio.ads
   [Ada]          stm32f40x-rcc.ads
Bind
   [gprbind]      main.bexch
   [Ada]          main.ali
Link
   [link]         main.adb
```

After compilation the `build` folder has the following files:

```sh
 build
 ├── b__main.adb
 ├── b__main.ads
 ├── b__main.ali
 ├── b__main.o
 ├── main
 ├── main.adb.stderr
 ├── main.adb.stdout
 ├── main.ali
 ├── main.bexch
 ├── main.o
 ├── stm32f40x.ads.stderr
 ├── stm32f40x.ads.stdout
 ├── stm32f40x.ali
 ├── stm32f40x-gpio.ads.stderr
 ├── stm32f40x-gpio.ads.stdout
 ├── stm32f40x-gpio.ali
 ├── stm32f40x-gpio.o
 ├── stm32f40x.o
 ├── stm32f40x-rcc.ads.stderr
 ├── stm32f40x-rcc.ads.stdout
 ├── stm32f40x-rcc.ali
 └── stm32f40x-rcc.o

```

I won't explain the file extensions today, because I think it's irrelevant right now, but the executable is `main`. 

```bash
$ readelf -h main
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2s complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           ARM
  Version:                           0x1
  Entry point address:               0x8004899
  Start of program headers:          52 (bytes into file)
  Start of section headers:          496068 (bytes into file)
  Flags:                             0x5000400, Version5 EABI, hard-float ABI
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         3
  Size of section headers:           40 (bytes)
  Number of section headers:         21
  Section header string table index: 18
```

# Final thoughts
So far it seems easier to understand and work than C projects, mainly because of the verbosity of the language and tools.
However, this is just the beginning let's see how far can I go with this.
 
