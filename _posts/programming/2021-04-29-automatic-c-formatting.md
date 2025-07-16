---
layout: post
title: Automatic C Formatting
category: programming
description: format C code fast, automatically 
tags: [style, quality, tools, tips]
---

When working alone or in a team, it's important to keep a consistent code style. And the easiest way is to automate it.


> Always code as if the guy who ends up maintaining your code will be a violent psychopath who knows where you live. -- Martin Golding


![code format](/images/posts/c-code-formatting.jpg)

* 
{:toc}

# Why you should format your code
Automatic code formating has the following benefits: 
- Improves the performance, because you focus on the task and not the style.
- Helps code review. Same structure, same understanding, no wasted time on discussions about style. 
- Improves maintainability. The code structure is consistent and is this predictable. 
- Shows your professionalism. Clean and neat! 


# Simple C  Formatter 

I personally use Artistic Style or [Astyle](http://astyle.sourceforge.net/). It is a source code indenter, formatter, and beautifier for the C, C++, C++/CLI, Objective-C, C# and Java programming languages.

If you're an Ubuntu user. You can install Astyle from the repo with `apt install astyle`. For Arch/Manjaro users, AStyle is also available from the repos. Otherwise, you can always download it from [Sourceforge](https://sourceforge.net/projects/astyle/files/astyle/).

You can use Astyle to format your `*.c` and `*.h`. It will take care of the indentations. There are many predefined styles that you can use. For example GNU, Linux, Google, Mozilla. 

```sh
astyle --style=linux -n include/*.h src/*.c
```

For my code I use the following options:

```sh
astyle --style=allman --xC60 -xL --mode=c -s4 -S -xW  -Y -p -xg -D -xe -f -k3 -n include/*.h src/*.c
```

A detailed explanation of Astyle options can be found [here](http://astyle.sourceforge.net/astyle.html)

*Quick reference:*  
- max code length: `-xC#`  
- break afterlogical: `-xL`  
- indent spaces: `-s#`  
- padding in equations and function calls: `-p` , `-xg`, `-D`  
- delete empty lines: `-xe`  
- break blocks: `-j` Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...).  
- `-n` Do not retain a backup of the original file.  
- indent switches: `-S`

```c
switch (foo)
{
case 1:
    a += 1;
    break;

case 2:
{
    a += 2;
    break;
}
}
```

becomes:

```c
switch (foo)
{
    case 1:
        a += 1;
        break;

    case 2:
    {
        a += 2;
        break;
    }
}
```

- indent-preproc-block: `-xW`

```c
#ifdef _WIN32
#include <windows.h>
#ifndef NO_EXPORT
#define EXPORT
#endif
#endif
```

becomes:

```c
#ifdef _WIN32
    #include <windows.h>
    #ifndef NO_EXPORT
        #define EXPORT
    #endif
#endif
```

-  indent-col1-comments: `-Y`

```c
void Foo()
{
// comment
    if (isFoo)
        bar();
}
```

becomes:

```c
void Foo()
{
    // comment
    if (isFoo)
        bar();
}
```

- align pointer=name: `-k3`

```c
char* foo1;
char & foo2;
string ^s1;
```

becomes 

```c
char *foo1;
char &foo2;
string ^s1;
```


# Other formatters:
There are more options such as [Uncrustify](http://uncrustify.sourceforge.net/), [Clang-format](https://clang.llvm.org/docs/ClangFormat.html) ,and [Jindent](https://marketplace.eclipse.org/content/jindent-source-code-formatter-javacc).

But you can also do a quick [Google search](https://letmegooglethat.com/?q=c+code+formatter+github) for repos in Github.

# Conclusion
It's always nice to automate tasks because it helps us focus on what's important. Development. And nowadays there are many great, free and easy options available. So download one today and start using it right away. 

In  a future post, I'll show how to use Docker to make your own Code Quality Checker, which will include not only style checking but also static analysis.
