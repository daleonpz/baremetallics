---
layout: post
title: No More Eclipse Workflow
category: programming
description: remove your workflow from eclipse
---

Working with makefiles has a huge advantage over working on Eclipse.

> Automatization is the biggest advantage

Tools like [Jenkins](https://www.jenkins.io/) take your development workflow to the next level.

You can automate your builds, tests, and deploy many instances of your software. 

Also, the workflow between teams is smoother and easier. The new developer can stop worrying about installing new toolchains, checking dependencies, or trying to make the IDE works on your OS. 


> You can have a ready-to-go solution.

This is a tutorial on how to have an indenpendent workflow to Eclipse.

**Note**: 
- This process can be used with CMake and Makefiles. I will use rake because it's easier.
- In this tutorial, I use the family of microcontrollers MSP430 and Linux. 

If you never heard about Rake. There is a [post]({% post_url programming/2020-10-17-rakefile-short-tutorial %}) with all you need to know. It's a 2-3 minutes introduction. 


# Writing your Rakefile

Main process:  
  
1) Download proprietary IDE.  
2) Build a "Hello World" example.  
3) Copy the output from the building process.  
4) "Reverse engineering" the output.  

## STEP 1: Download proprietary IDE
You can download the proprietary Eclipse-based IDE from the [texas instruments website](https://www.ti.com/tool/CCSTUDIO-MSP).

```sh
$  ls
CCS10.1.0.00010_linux-x64.tar.gz
```

Untar the file 

```sh
$  tar -xvzf CCS10.1.0.00010_linux-x64.tar.gz
$  ls
CCS10.1.0.00010_linux-x64/  CCS10.1.0.00010_linux-x64.tar.gz
```

Execute the installer

```sh
$  pwd
CCS10.1.0.00010_linux-x64

$  ls
binary/      features/      ccs_setup_10.1.0.00010.run*  README_FIRST_linux.txt
components/  artifacts.jar  content.jar                  timestamp.txt

$  ./ccs_setup_10.1.0.00010.run
```

I chose `MPS430 ultra-low power MCUs` as the only component to install. Because I want to write a rakefile for that family.

By default the CCS is installed under `/home/<USER>/ti/ccs1010`. in my case `<USER>` is `me`

Run the CCS

```sh
$  cd /home/me/ti/ccs1000/ccs/eclipse
$  ./ccstudio
```

I use `/home/me/ti/workspace` as workspace directory.

If you never used eclipse-base IDE, probably you have no idea what the _Eclipse workspace_ is.

> The workspace is a directory on the disk where the Eclipse platform and all the installed plug-ins store preferences, configurations, and temporary information. Subsequent Eclipse invocations will use this storage to restore the previous state. As the name suggests, it is your "space of work".


## STEP 2: Build The "hello world" Example
Create a "New Project". 

In my case:
- Target: MPS430FRxxx Family
- Microcontroller: MPS430FR2433

Because that's the micro I have. But as you will see later, it doesn't matter which microcontroller you choose.

Choose from the project template "Blink the LED".  As shown in the figure below:

![New project](/images/posts/ccs_new_project.png)

Click on "Finish".

Then, in the "Project Explorer" click on the project name "blinky".

![Project Explorer](/images/posts/ccs_project_explorer.png)


Right-click and then "Build Project".
Check the "Console" output.

![Console Output](/images/posts/ccs_console.png)

## STEP 3: Copy The Console Output 
You will copy all the output to some file. In my case is `rakefile.rb`

```sh
$  cat rakefile.rb
**** Build of configuration Debug for project blinky ****

/home/me/Downloads/temp/ti/ccs1010/ccs/utils/bin/gmake -k -j 8 all -O 
 
Building file: "../blink.c"
Invoking: MSP430 Compiler
"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include" --include_path="/home/me/Downloads/temp/ti/workspace/blinky" --include_path="/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="blink.d_raw"  "../blink.c"
Finished building: "../blink.c"
 
Building target: "blinky.out"
Invoking: MSP430 Linker
"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 -z -m"blinky.map" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on -i"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/lib" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="blinky_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "blinky.out" "./blink.obj" "../lnk_msp430fr2433.cmd"  -llibmath.a -llibc.a 
<Linking>
remark #10371-D: (ULP 1.1) Detected no uses of low power mode state changing instructions
remark #10372-D: (ULP 4.1) Detected uninitialized Port A in this project. Recommend initializing all unused ports to eliminate wasted current consumption on unused pins.
remark #10372-D: (ULP 4.1) Detected uninitialized Port B in this project. Recommend initializing all unused ports to eliminate wasted current consumption on unused pins.
Finished building target: "blinky.out"
 

**** Build Finished ****
 
```

## STEP 4: "Reverse Engineering" 
The idea in this last step is to create a Rakefile to run the exact commands that Eclipse-based IDEs run.

Let's first delete the unnecessary lines.

You need to keep the lines that have a relation with the building process. 
In this case would be:

```sh
"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include" --include_path="/home/me/Downloads/temp/ti/workspace/blinky" --include_path="/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="blink.d_raw"  "../blink.c"

"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 -z -m"blinky.map" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on -i"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/lib" -i"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="blinky_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "blinky.out" "./blink.obj" "../lnk_msp430fr2433.cmd"  -llibmath.a -llibc.a 
```

Now I use rake syntax to clean up the code above.

> Remember! Usually, you need two basic steps: Compiling and linking. 

Let's define then one task for linking and one rule for compiling.

Why I do in this way... because linking needs a group of object files, but compiling is a one to one operation. For example, the compiler uses `file.c` to create `file.o`, and the linker uses all `*.o` to create an executable. 

**Note**: object files can be `*.o` or `*.obj`

The basic structure of a rakefile would be like this:

```ruby
task :build => OBJECTS do
    puts "[Linking]\n"
end

# #{task.name} is a reference to the '.obj` you are creating
# For example, #{task.name} can be `file.o`

rule '.obj' =>  '.c' do |task|
    puts "[Building Object] #{task.name}\n"
end
```

Let's arrange a little bit:

```ruby
task :build => OBJECTS do
    puts "[Linking]\n"
    sh "\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430\" -vmspx --use_hw_mpy=F5 --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 -z -m\"blinky.map\" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/lib\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include\" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info=\"blinky_linkInfo.xml\" --use_hw_mpy=F5 --rom_model -o \"blinky.out\" \"./blink.obj\" \"../lnk_msp430fr2433.cmd\"  -llibmath.a -llibc.a "
end

rule '.obj' =>  '.c' do |task|
    puts "[Building Object] #{task.name}\n"
    sh " \"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/bin/cl430\" -vmspx --use_hw_mpy=F5 --include_path=\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include\" --include_path=\"/home/me/Downloads/temp/ti/workspace/blinky\" --include_path=\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include\" --advice:power=all --advice:hw_config=all --define=__MSP430FR2433__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency=\"blink.d_raw\"  \"../blink.c\" "
end

```

Things to note:
- `puts`: it's to print something on the screen
- `sh`: it's a method to run system commands

Let's start using variables to make the task/rule more generic:

```ruby
TOOLCHAIN_PATH  = "/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler"
COMPILER_PATH   =  "#{TOOLCHAIN_PATH}/ti-cgt-msp430_20.2.2.LTS"

GCC         = "#{COMPILER_PATH}/bin/cl430" 
DEVICE      = "__MSP430FR2433__"

COMPILER_OPTS       = "-vmspx --use_hw_mpy=F5 -O2 -g --c99 --define=#{DEVICE}"
MISC_OPTS           = "--advice:power=\"all\" --advice:hw_config=\"all\"" 
RUNTIME_MODEL_OPTS  = "--printf_support=minimal \
                        --silicon_errata=CPU21 \
                        --silicon_errata=CPU22 \
                        --silicon_errata=CPU40"

CFLAGS = "#{COMPILER_OPTS} #{MISC_OPTS} #{RUNTIME_MODEL_OPTS}"

task :build => OBJECTS do
    puts "[Linking]\n"
    sh "#{GCC} #{CFLAGS} -z -m\"blinky.map\" --heap_size=160 --stack_size=160 --cinit_hold_wdt=on -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/lib\" -i\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include\" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info=\"blinky_linkInfo.xml\" --use_hw_mpy=F5 --rom_model -o \"blinky.out\" \"./blink.obj\" \"../lnk_msp430fr2433.cmd\"  -llibmath.a -llibc.a "
end

rule '.obj' =>  '.c' do |task|
    puts "[Building Object] #{task.name}\n"
    sh "#{GCC} #{CFLAGS} --include_path=\"/home/me/Downloads/temp/ti/ccs1010/ccs/ccs_base/msp430/include\" --include_path=\"/home/me/Downloads/temp/ti/workspace/blinky\" --include_path=\"/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler/ti-cgt-msp430_20.2.1.LTS/include\" --diag_warning=225 --diag_wrap=off --display_error_number  --preproc_with_compile --preproc_dependency=\"blink.d_raw\"  \"../blink.c\" "
end

```

The rakefile starts looking good.

With some more modifications I ended up with the following rakefile:

```ruby
require 'rake/clean'

##############################################
## Define Compiler/linker Variables 
##############################################
TOOLCHAIN_PATH = "/home/me/Downloads/temp/ti/ccs1010/ccs/tools/compiler"
COMPILER_PATH =  "#{TOOLCHAIN_PATH}/ti-cgt-msp430_20.2.2.LTS"

DEVICE      = "__MSP430FR2433__"
LINKER_CMD_FILE = "#{TOOLCHAIN_PATH}/ccs_base/msp430/include/lnk_#{DEV_NAME.downcase}.cmd"

puts "Linker file: #{LINKER_CMD_FILE}"

SOURCE_DIR  = "src"
INCLUDE_DIR = "include/"
OBJ_DIR     = "build/obj"
BUILD_DIR   = "build"
GCC         = "#{COMPILER_PATH}/bin/cl430" 

FileUtils.mkdir_p "#{OBJ_DIR}/"
FileUtils.mkdir_p "#{BUILD_DIR}/"

SOURCES     = Rake::FileList["#{SOURCE_DIR}/*.c"]
OBJECTS     = SOURCES.ext(".obj").pathmap("#{OBJ_DIR}/%f")
INCLUDES    = INCLUDE_DIR

TARGET      = "#{BUILD_DIR}/my_app"

STATIC_LIBS     = "-llibmath.a -llibc.a"
LIBS            = "#{LINKER_CMD_FILE} #{STATIC_LIBS}"

INCLUDE_PATHS = " \
                --include_path=\"#{TOOLCHAIN_PATH}/ccs_base/msp430/include\"  \
                --include_path=#{INCLUDES}  \
                --include_path=\"#{COMPILER_PATH}/include\" \
                " 

INCLUDE_LIBS_PATH = " \
                    -i\"#{TOOLCHAIN_PATH}/ccs_base/msp430/include\" \
                    -i\"#{TOOLCHAIN_PATH}/ccs_base/msp430/lib/5xx_6xx_FRxx\"    \
                    -i\"#{COMPILER_PATH}/lib\"  \
                    -i\"#{COMPILER_PATH}/include\" \
                    "

COMPILER_OPTS   = "-vmspx --use_hw_mpy=F5 -O2 -g --c99 --define=#{DEVICE}"

MEMORY_MAP  = "#{BUILD_DIR}/memory.map"
LINKER_OPTS = "--run_linker --heap_size=160 --stack_size=160 -m#{MEMORY_MAP}"
LINKER_OUTPUT_OPTS  = "" #"--xml_link_info=\"#{TARGET}.xml\""


MISC_OPTS           = "--advice:power=\"all\" --advice:hw_config=\"all\"" 
DIAGNOSTIC_OPTS     = "--diag_warning=255 \
                        --diag_wrap=off \
                        --display_error_number \
                        --warn_sections"

RUNTIME_ENV_OPTS    = "--cinit_hold_wdt=on --rom_model"
RUNTIME_MODEL_OPTS  = "--printf_support=nofloat \
                        --silicon_errata=CPU21 \
                        --silicon_errata=CPU22 \
                        --silicon_errata=CPU40"

FILE_SEARCH_PATH_OPTS   = "--reread_libs"


CFLAGS = "#{COMPILER_OPTS} #{MISC_OPTS} #{RUNTIME_MODEL_OPTS}"
LFLAGS = "#{LINKER_OPTS} \
            #{RUNTIME_ENV_OPTS} \
            #{DIAGNOSTIC_OPTS}  \
            #{FILE_SEARCH_PATH_OPTS}    \
            #{LINKER_OUTPUT_OPTS}"


##############################################
## Ruby Tools on Docker
##############################################
require_relative "/usr/toolchain/tools/RakeSupportFunctions"
include RakeSupportFunctions

##############################################
## Building Process
##############################################
task :default => :build

task :build => OBJECTS do
    puts "#{BBlue}[Linking]#{Color_Off}\n"
    quiet_sh "#{GCC} #{CFLAGS} #{LFLAGS} #{INCLUDE_LIBS_PATH} -o #{TARGET}.out #{OBJECTS} #{LIBS}"

end


# %f The base filename of the path, with its file extension,
# pathmap :Apply the pathmap spec to each of the included file names, returning a new file list with the modified paths. (See String#pathmap for details.)
#  lambda:     say_something = -> { puts "This is a lambda" }

# For each '.o' file add path "SOURCE_DIR/ " and change extension to C
rule '.obj' => ->(t){t.pathmap("#{SOURCE_DIR}/%f").ext(".c")} do |task|
    puts "#{BBlue}[Building Object] #{task.name} #{Color_Off} \n"
    quiet_sh "#{GCC} #{CFLAGS}  #{INCLUDE_PATHS} --preproc_with_compile --preproc_dependency=#{task.name.ext(".d_raw")} --obj_directory=#{OBJ_DIR}/ #{task.source} #{COLOR_OUTPUT} " 

end

CLEAN.include("#{OBJ_DIR}", "#{BUILD_DIR}" )

```

As you can see. Writing is a rakefile is straighforward.

# Conclusion
Eclipse-based IDEs are powerful tools for writing code, but impractical for an automated software development system.

Working with makefiles has a huge advantage over working on Eclipse. Because makefiles can run on servers. But Makefiles are hard to write and understand. 

That's why in this tutorial I used Rakefile. It's easy to use and uses Ruby's engine. 

As you could see. Writing rakefiles are easy to write and read. 

In a future post. You'll learn how to integrate your rakefile with docker and gitlab. So you can build your own solid and automated building and testing system.

