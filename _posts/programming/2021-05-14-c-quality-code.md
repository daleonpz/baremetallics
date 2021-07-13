---
layout: post
title: Tools for C quality code 
category: programming
description: tools for c  quality code 
---

> “Quality is not an act; it is a habit” - Aristotle 

![tools quality C code](/images/posts/tools_quality_code.jpg)


I already talked about [Spaghetti code]({% post_url programming/2021-05-06-spaghetti-code %}) and [How to format your C code]({% post_url programming/2021-04-29-automatic-c-formatting %}). In this post I'll put everything together using Docker, so you can run your C code quality checker on every project you are working on. 

You could check:
- Code Complexity: Do you have an overcomplicated code?
- Code Format: Does your code follow your organization code guidelines?
- Code Static Analysis: Did you misuse C code? Does your code have vulnerabilities?

You can find the repo of this post [here](https://github.com/daleonpz/Dockerfiles/tree/master/code-quality-control).

* 
{:toc}

# Solution
Most of the tools to check the quality of your code can run from the CLI or Command Line Interface. For that reason, I created bash scripts to adapt the tools to my needs, because some of them display more information than I need. 

After having created the bash scripts, I wrote a Dockerfile to create a Docker image. If you want to know more about Docker, check this [page]({{site.baseurl}}/topics/docker-essentials/). The goal is to have a Docker container with my scripts and from there I can verify if my code follows my standards. 

If you follow this post until the end, you will end up with a repo like this:


```sh
$  tree code-quality-control/
code-quality-control/
├── checkers
│   ├── 00-style-analysis.sh
│   ├── 01-code-complexity.sh
│   └── 02-static-analysis.sh
├── Dockerfile
└── README.md

1 directory, 5 files
```

**You DON'T need to install anything but Docker**.

Let's start by creating all the required files and directories.

```sh
$ mkdir code-quality-control 
$ cd code-quality-control 
$ mkdir checkers
$ touch 00-style-analysis.sh 01-code-complexity.sh 02-static-analysis.sh
$ touch Dockerfile
```

# Bash Scripts for C code checkers
## Is My Code Following The Code Guidelines? 
I use [Astyle](http://astyle.sourceforge.net/astyle.html) to format my code. In the following bash script, I define my code guidelines using AStyle options.  Of course, you can use general guidelines from Linux or Google. To learn how to do it with AStyle, check this [post]({% post_url programming/2021-04-29-automatic-c-formatting %}) 

Open the file `00-style-analysis.sh` with your favorite editor. 

```sh
#!/bin/bash

# Filename: 00-style-analysis.sh

usage() { echo "Usage: [-h help] [-I includes_folder ] [-s source_folder ] " >&2; exit 1; }

INCLUDES="include"
SOURCES="src"

while getopts n:c:h option
do
    case "${option}"
        in
        I) INCLUDES=${OPTARG};;
        s) SOURCES=${OPTARG};;
        h) usage;;
        *) usage;;
    esac
done


echo "==========================================================="
echo "      Running Style checking ...                           "
echo "         Processing *.c and *.h files                      "
echo "==========================================================="

results=$(astyle --style=allman --xC60 -xL --mode=c \
    -s4 -S -xW  -Y -p -xg -D -xe -f -k3 -n \
    $INCLUDES/*.h $SOURCES/*.c | \
    sed  -r '/Unchanged/d' | sed 's/Formatted  //g')

if [[ $? != 0 ]]; then
    echo "AStyle Command failed."
    exit 1
fi


echo "             Results                                      "

if [[ $results ]]; then
    echo "  FAIL                                                    " >&2
    echo "  Format correctly the following files:                   " >&2
    echo "===========================================================" >&2
    echo "$results">&2
    exit 1
else
    echo "  SUCCESS         "
    echo "  Everything is ok."
    echo "==========================================================="
    exit 0
fi
```

## Is My Code a Mess? 
To check the complexity of my code I use [Lizard](https://github.com/terryyin/lizard). Open the file `01-code-complexity.sh` with your favorite editor, and copy the following bash script:

```sh
#!/bin/bash

# Filename: 01-code-complexity.sh

usage() { echo "Usage: [-h help] [-n num_lines_of_code ] [-c code_complexity] " >&2; exit 1; }

# Default thresholds
NLOC=255
CCN=15

while getopts n:c:h option
do
    case "${option}"
        in
        n) NLOC=${OPTARG};;
        c) CCN=${OPTARG};;
        h) usage;;
        *) usage;;
    esac
done

echo "==========================================================="
echo "      Running Complexity Analysis ...                      "
echo " Thresholds:                                               "
echo "     NLOC: $NLOC                                           "
echo "     Complexity: $CCN                                      "
echo "==========================================================="

results=$(lizard -Tnloc=$NLOC -Tcyclomatic_complexity=$CCN -w --languages c src/  | \
    sed 's/: warning:/: function/g')

if [[ $? != 0 ]]; then
    echo "Lizard Command failed."
elif [[ $results ]]; then
    echo "             Results                                       " >&2
    echo "  FAIL                                                     " >&2
    echo "  Refactor the following modules:                          " >&2
    echo "===========================================================" >&2
    echo "$results">&2
    exit 1
else
    echo "             Results                                      "
    echo "  SUCCESS                                                 "
    echo "  Your code doesn't exceed the thresholds.                "
    echo "===========================================================" 
    exit 0
fi
```

## Is My Code Vulnerable? 
There are many C code static analyzer, they are also called [Linters](https://letmegooglethat.com/?q=lint+c). I use [CppCheck](http://cppcheck.sourceforge.net/) because is open-source, and have a big community. 

Open the file `02-static-analysis.sh` with your favorite editor, and copy the following bash script:

```sh
#!/bin/bash

usage() { echo "Usage: [-h help] [-c checker] [-i ingore [true|false]] " >&2; exit 1; }

dummy_file=___dummyfile
c_checker=cppcheck
ignore_flag=false

while getopts c:i:h option
do
    case "${option}"
        in
        c) c_checker=${OPTARG};;
        i) ignore_flag=${OPTARG};;
        h) usage;;
        *) usage;;
    esac
done

echo "==========================================================="
echo "      Running Static Code Analysis ...                     "
echo "              with $c_checker                              "
echo "==========================================================="

$c_checker --enable=warning --enable=performance --enable=unusedFunction \
    --enable=information --language=c  --inconclusive \
    --output-file=$dummy_file -I include/ src/  

if [[ $? != 0 ]]; then
    echo "$c_checker Command failed."
    exit 1
fi

echo "==========================================================="
echo "             Results                                       "
echo "==========================================================="

# ignoring possible ISR 
results=$(cat $dummy_file | \
    sed "/^$/d" | \
    sed -r "/[A-Z]+_ISR.*\[unusedFunction\]/,+1d" | \
    sed "/^nofile:0:0.*[missingInclude]/d")

if [[ $? != 0 ]]; then # This line evaluates the previus command
    rm $dummy_file
    echo "Result processing failed." &>2
    exit 1
fi

rm $dummy_file
if [[ $results ]]; then
    echo "  FAIL                                                     " 
    echo "  Refactor the following files:                            " 
    echo "==========================================================="
    echo "$results"
    if [[ $ignore_flag == false ]]; then exit 1; fi
    echo ""
    echo ""
    echo "  Static Analysis EXIT ERROR was IGNORED !"
    echo ""
else
    echo "  SUCCESS                                                 "
    echo "  The static analyzer approves your code."
fi

echo "==========================================================="
exit 0
```

**Important NOTE:**
- The analysis currently ignores messages related to possible Interrupt Service Routines (ISR).  That means that function written in uppercase will be ignored. **Note**: Usually ISRs are written in uppercase.
For example, the following ISR will be ignored:

```c
void __attribute__ ( ( interrupt( USCI_A1_VECTOR ) ) )
    USCI_A1_ISR ( void )
```

- CppCheck won't detect missing semi-colons. For example:

```c
int main ( )
{
    int x = 10
    x = 5
    return x
}
```

This type of erros will be overlooked

# Dockerize Your QA
If you don't know what Docker is, read the following lines.

__Docker is a tool designed to create, deploy and run an application by using containers, and a container is a unit of software that includes all its dependencies so the application runs regardless of the installed OS, because all that application might need is already in the container.__


The Dockerfile for our code quality checkers is as follows.

```ruby
FROM frolvlad/alpine-glibc:alpine-3.12
MAINTAINER Daniel Paredes <daleonpz@gmail.com>

# Set up a tools dev directory
WORKDIR /home/dev

# Installing bash
RUN apk add --update --no-cache bash

# Cyclomatic Complexity Tool
RUN apk add --update --no-cache python3 py3-pip
RUN pip install lizard
RUN lizard --version

# Static code analyzer 
RUN apk add --update --no-cache cppcheck 
RUN cppcheck --version

# Style formatter
RUN apk add --update --no-cache astyle
RUN astyle --version 

COPY checkers /usr/checkers/
```

To build the docker image, run the following command:

```sh
$ sudo docker build --network=host -f Dockerfile local/code_checker .
```

# How To Use Your Code Checker
I use a bash script to run my checkers automatically, you can also do the same. In your project directory create the folder utils:

```
$ mkdir utils
$ cd utils
$ touch run_checker_local.sh
```

Open the file `run_checker_local.sh` with your editor and copy the following lines:

```sh
#!/bin/bash

IMAGE_NAME="git.infra.cospace.de:4567/guidelines/code-quality-control/build-docker:latest"

LOCAL_WORKING_DIR="$(pwd)"
DOCKER_WORKING_DIR="/usr/$(basename "${LOCAL_WORKING_DIR}")"

echo ${LOCAL_WORKING_DIR}

COMMAND_TO_RUN_ON_DOCKER=(sh -c "bash /usr/checkers/00-style-analysis.sh  && \
        bash /usr/checkers/01-code-complexity.sh  -c 10 && \
        bash /usr/checkers/02-static-analysis.sh ")

sudo docker run \
    --rm \
    -v "${LOCAL_WORKING_DIR}":"${DOCKER_WORKING_DIR}"\
    -w "${DOCKER_WORKING_DIR}"\
    --name my_container "${IMAGE_NAME}"  \
    "${COMMAND_TO_RUN_ON_DOCKER[@]}"
```

To run this script, just execute on your project base directory. In my case my project folder looks like this:

```sh
$ tree -L 2
.
├── data.md
├── include
│   ├── uart_bitbang.h
│   └── uart.h
├── rakefile.rb
├── README.md
├── src
│   ├── main.c
│   ├── uart_bitbang.c
│   └── uart.c
└── utils
    └── run_checks_local.sh
```

And I run my C code checkers with 

```sh
$ bash utils/run_checks_local.sh
```

You should an output similar to this one:

```sh
$  bash utils/run_checks_local.sh
/home/me/Documents/git/ti_projects/pfeiffer-vaccum-poc
===========================================================
      Running Style checking ...
         Processing *.c and *.h files
===========================================================
             Results
  SUCCESS
  Everything is ok.
===========================================================
===========================================================
      Running Complexity Analysis ...
 Thresholds:
     NLOC: 255
     Complexity: 10
===========================================================
             Results
  FAIL
  Refactor the following modules:
===========================================================
src/app_usart_to_extender.c:557: function usart_execute_cmd has 102 NLOC, 19 CCN, 422 token, 2 PARAM, 125 length
src/main.c:26: function main has 93 NLOC, 13 CCN, 345 token, 1 PARAM, 114 length
```

# Conclusion
Most of the tools to check the quality of your code can run from the CLI or Command Line Interface. For this reason, we created bash scripts to adapt the tools to your own needs.

After having created the bash scripts, we wrote a Dockerfile to create a Docker image. The goal is to have a Docker container with my scripts and from there you can verify if your code follows some standards. 

I hope you enjoy this post. If so, subscribe to my mailing list.

