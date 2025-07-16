---
layout: post
title: The C Coding Standard 
category: programming
tags: [c, coding standard, security, quality ]
---

Whenever I read a programming book I do notes, test the examples and try to solve the problems.
I was reading the book [Hacking: The Art of Exploitation](https://www.amazon.de/Hacking-Art-Exploitation-Jon-Erickson/dp/1593271441), and just in the second chapter I wasn't able to run a example. I checked the code  and I didn't made any mistake. The code was quiet simple:

```c
#include <stdio.h>
#include <stdint.h>

int main() {
   int i;
   char char_array[5] = {'a', 'b', 'c', 'd', 'e'};
   unsigned int hacky_nonpointer;
   hacky_nonpointer = (unsigned int) char_array;

   for(i=0; i < 5; i++) {      
        printf("[hacky_nonpointer] points to %p, which contains the char '%c'\n",
            hacky_nonpointer, *((char *) hacky_nonpointer));
            hacky_nonpointer = hacky_nonpointer + sizeof(char);
   }
}

```

Basically the program iterate over the `char` array using a `unsigned int` pointer just to explain how _typecasting_ works. Nonetheless, this code taken from the examples produced a segmentation fault. As you may observed, the code has no mistakes. However, this code is **Non-compliant**. A **compliant code** is a code that follows a standard.


The **SEI CERT C Coding Standard** documented in this [wiki](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard), has a section related to converting a pointer to integer or integer to pointer. 

Even though, it makes sense to use integers and pointers interchangeably in C both are 32 bit length, according to the C standard, using integers as a pointer or viceversa may have undesired consequences. Read more [here](https://wiki.sei.cmu.edu/confluence/display/c/INT36-C.+Converting+a+pointer+to+integer+or+integer+to+pointer).


Any valid pointer to `void` can be converted to or from `intptr_t` or `uintptr_t` with no value change. Also, a pointer to `void` may be converted to or from a pointer to any type and back again,  and the result must remain the same. Thus, direct conversion of a `char` pointer to a `uintptr_t` is allowed as long as the  implementation supports the `uintptr_t` type.

In the example taken from the book, we were in the same case. There was a cast of a pointer to `char` to `unsigned int`, that must be changed to  `uintptr_t` in to be consequent with the standard. 

Code compliant version of the above code is:
```c
#include <stdio.h>
#include <stdint.h>

int main() {
   int i;
   char char_array[5] = {'a', 'b', 'c', 'd', 'e'};
   uintptr_t hacky_nonpointer;
   hacky_nonpointer = (uintptr_t) char_array;

   for(i=0; i < 5; i++) { // Iterate through the int array with the int_pointer.
      printf("[hacky_nonpointer] points to %p, which contains the char '%c'\n",
        hacky_nonpointer, *((char *) hacky_nonpointer));
        hacky_nonpointer = hacky_nonpointer + sizeof(char);
   }
}
```

Now the output makes sense:

```sh
[hacky_nonpointer] points to 0x7ffcfaf9e433, which contains the char 'a'
[hacky_nonpointer] points to 0x7ffcfaf9e434, which contains the char 'b'
[hacky_nonpointer] points to 0x7ffcfaf9e435, which contains the char 'c'
[hacky_nonpointer] points to 0x7ffcfaf9e436, which contains the char 'd'
[hacky_nonpointer] points to 0x7ffcfaf9e437, which contains the char 'e'
``` 
