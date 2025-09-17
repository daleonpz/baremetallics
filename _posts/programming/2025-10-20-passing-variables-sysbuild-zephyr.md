---
layout: post
title: Passing variables to Sysbuild Zephyr
category: programming
description: How to pass variables to CMake when using Sysbuild in Zephyr projects.
tags: [zephyr, sysbuild, cmake, embedded]
---

Sysbuild is a higher-level build system that can be used to combine different modules that are not in the necessary under the same project like MCUmgr and MCUBoot.  It's a great tool and it's part of the Zephyr ecosystem, but sometimes can be a pain in the ass, like when you want to pass a configuration variable to a specific module using the famous `-D` flag in CMake.  

* 
{:toc}

# Passing Variables to CMake
Let’s say you have the following `Cmakelists.txt` and a simple zepyhr application that prints the current version of your software. I know you could set the software version with a `VERSION` file, but for this post I'll ignore it to show my point.

```rust
cmake_minimum_required(VERSION 3.20.0)
set(BOARD "nucleo_wb55rg")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(otau-example)

if(NOT DEFINED SW_VERSION)
    set(SW_VERSION 0)
else()
    set(SW_VERSION ${SW_VERSION})
endif()

set(VERSION_STRING "${SW_VERSION}." CACHE STRING "Version string" FORCE)
add_compile_definitions(-DVERSION_STRING="${VERSION_STRING}")

target_sources(app PRIVATE src/main.c)
```

Somewhere in your code `main.c` you have

```c
LOG_DBG("SW Version: %s", VERSION_STRING);
```

Now, you can pass a parameter with `-D` as follows

```sh
 west build   -- -DBOARD=nucleo_wb55rg -DSW_VERSION=1
 [00:00:00.003,000] <dbg> otau_smp_sample: main: SW Version: 1
```

That works fine. But then you move to **Sysbuild** because you want a modular build system (or to add MCUboot), and things break.

# The Problem with Sysbuild
Let’s say your project looks like this:

```sh
$ tree
.
|-- CMakeLists.txt
|-- prj.conf
|-- src
|   `-- main.c
|-- sysbuild
|   `-- mcuboot.conf
|-- sysbuild.conf
`-- west.yaml
```

According to the [Sysbuild docs](https://docs.zephyrproject.org/latest/build/sysbuild/index.html), you should be able to pass variables like this:
```sh
west build --sysbuild ... -- -Dmy_sample_FOO=BAR
```

But here’s the catch: **It doesn’t work**.

```
west build --pristine --sysbuild  -- -DBOARD=nucleo_wb55rg -Dotau_example_SW_VERSION=9

...
-- Including signing script: /home/dnl/Documents/git/nix_zephyr/zephyr/cmake/mcuboot.cmake
-- Board: nucleo_wb55rg
-- SW Version: 0
-- Configuring done (2.2s)
-- Generating done (0.1s)
-- Build files have been written to: /home/dnl/Documents/git/nix_zephyr/otau_example/build/otau_example
-- Configuring done (5.6s)
-- Generating done (0.0s)

[00:00:00.003,000] <dbg> otau_smp_sample: main: SW Version: 0
```

The software version is still 0 , even when i set it to 9.. WTF?!

# The Fix: zephyr_get()
There is a non-well-documented function you need to add to your CMakeLists.txt. The infamous `zephyr_get()`

```rust
cmake_minimum_required(VERSION 3.20.0)
set(BOARD "nucleo_wb55rg")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(otau-example)

# Grab variable from Sysbuild
zephyr_get(SW_VERSION SYSBUILD GLOBAL)

if(NOT DEFINED SW_VERSION)
    set(SW_VERSION 0)
else()
    set(SW_VERSION ${SW_VERSION})
endif()

message(STATUS "Version: ${SW_VERSION}")
set(VERSION_STRING "${SW_VERSION}" CACHE STRING "Version string" FORCE)
add_compile_definitions(-DVERSION_STRING="${VERSION_STRING}")

target_sources(app PRIVATE src/main.c)
```

Now rebuild

```sh
west build --pristine --sysbuild  -- -DBOARD=nucleo_wb55rg -Dotau_example_SW_VERSION=9 

-- Including signing script: /home/dnl/Documents/git/nix_zephyr/zephyr/cmake/mcuboot.cmake
-- Board: nucleo_wb55rg
-- SW Version: 9
-- Configuring done (2.2s)
-- Generating done (0.1s)
-- Build files have been written to: /home/dnl/Documents/git/nix_zephyr/otau_example/build/otau_example


[00:00:00.003,000] <dbg> otau_smp_sample: main: SW Version: 9
```

It works! Now we're talking!!!
# Conclusions
Zephyr's ecosystem is great; there are many tools that simplify your development like `sysbuild` but some of them are not well-documented. Nevertheless, it's great to share tips and tricks, so everybody can benefit from it.

