---
layout: post
title: Spaghetti Code in C 
category: programming
description: reduce complexity in C spaghetti code 
---

> It's OK to build very complex software, but you don't have to build it in a complicated way. 

![Spaguetti code](/images/posts/spaghetti.jpg)


I already talked about [How to write clean code]({% post_url programming/2020-05-02-write-clean-code %}), but I didn't talk about code complexity. In this post you'll read:

- What's Spaghetti Code
- What's Code Complexity
- Tools to Estimate Your Code Complexity
- Rules of Thumb
- Conclusions

# What's a Spaghetti Code
It's code that's hard to maintain. Some reasons why a code can be denoted as "Spaghetti Code" are:
- Complex control structures. Nested for loops, if/ switch cases. 

```c
for( int i=0; i<LEN; i++)
{
    switch some_value:
        case 5:
            if ( i>some_value) 
            {
                ...
            }
            else
            {
                ...
            }
        break;
}
```

- Hardcoded code.

```c
char x[50];

for (int i=0; i<50; i++){...}
```
instead of

```c
#define LEN (50)
char x[LEN];
for( int i=0; i<LEN; i++) {...}
```

- Code duplication. Have you ever copied-pasted lines of code instead of using a function or macros?

```c
if ( x = 0)
{
    y = x*y;
    x +=5;
    printf("%i \r\n", x);
}
else if (x<0)
{
    y = x*y;
    x -=5;
    printf("%i \r\n", x);
}
else
{
    y = x*y;
    x *=5;
    printf("%i \r\n", x);
}
```

instead of

```c
y = x*y;

if ( x = 0) { x +=5; }
else if (x<0) { x -=5; }
else { x *=5; }

printf("%i \r\n", x);
```

Those were some examples of spaghetti code.

# What's Code Complexity 
A very common measurement is the **Cyclomatic Complexity** or **McCabe's Complexity**. [Wikipedia](https://en.wikipedia.org/wiki/Cyclomatic_complexity) defines it as  a quantitative measure of the number of linearly independent paths through a program's source code. 

A very simplified idea is that McCabe's complexity tells you how many decisions your code has to make.  For example,

```c
int main()
{
    int x=0;
    printf("%i", x);
}
```

The McCabe's Complexity of that code is 1. But for example

```c
int main()
{
    int x=0;
    if( x<5 && x>0) printf("%i", x);
    else printf("%i", x+1);
}

```

It's McCabe's complexity is 3. Because 1  + 1 (if) + 1 (and condition `&&`).


# How to Estimate Your Code Complexity

A very handful tool is [Lizard](http://www.lizard.ws/). It's a Complexity Analyzer for many programming languages including C. 
It gives you not only your code complexity but also the number of line code without comments.

In [GitHub](https://github.com/terryyin/lizard), there is a clear explanation on how to install it.

I will run Lizard in one of my projects. This is the source code.

```c
#include <stdlib.h>
#include <stdio.h>

static int checkResources(int *max, int* all, int* av, int N){
    int n = 1;
    int i;
    for ( i = 0; i<N ;i++){
        n = n && ( max[i] - all[i] <= av[i] ? 1:0);
    }

    return n;
}


/*
 M is the number of proccesses
 N is the number of resources
 */
static void bankers( int *max, int *all, int *av, int *safe, int M, int N){
    int i,k;
    int *done;
    int n = 0;

    done = (int*)calloc(M , sizeof(int) );

    while(n < M){
        for (i = 0; i<M; i++){
            if (!done[i]){
                if ( checkResources(max + i*N, all+ i*N, av, N) ){
                    safe[n] = i;
                    n++;
                    for( k = 0; k < N; k++) av[k] += all[i*N+k]; 
                    done[i] = 1;
                }
            } 
        }
    }

    free(done);
}

int main(){
    int i;
    int N, M;
    int *max, *all, *av, *safe;

    puts("-----------------------------");
    puts("  Banker's Algortihm Tester  ");
    puts("-----------------------------");

    printf("\nEnter the number of processes: ");
    scanf("%d", &M);

    printf("\nEnter the number of resources: ");
    scanf("%d", &N);


    max = (int *)calloc( N*M , sizeof(int));
    all= (int *)calloc( N*M , sizeof(int));
    av = (int *)calloc( N , sizeof(int));
    safe = (int *)calloc( M , sizeof(int)); 
   
    printf("\nEnter maximum resource table: start with the first row\n");
    for (i = 0; i < N*M; i++) {
            scanf("%d", max+i);
    }

    printf("\nEnter allocated resource table: start with the first row\n");
    for(i = 0; i < N*M; i++) {
            scanf("%d", all+i);
    }

    printf("\nEnter available resource table:\n");
    for(i = 0; i < N; i++) {
            scanf("%d", av+i);
    }

    bankers(max, all, av, safe, M,N); 
    for(int k=0; k<M; k++)  printf("%i ", safe[k] );
    printf("\n");
   
    free(safe);
    free(av);
    free(all);
    free(max); 
}

```

As you can see there are many for-loops, a couple of if-conditions, and some logical operations. 
To run lizard for C code is as follows: 

```sh
lizzard -l c <ProjectDir>
```

In my case, 

```sh
$  lizard -l c  BankersAlgorithm/
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
       8      4     70      4       9 checkResources@4-12@BankersAlgorithm/banker.c
      19      6    161      6      22 bankers@19-40@BankersAlgorithm/banker.c
      35      5    290      0      45 main@43-87@BankersAlgorithm/banker.c
1 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     64      20.7     5.0      173.7         3     BankersAlgorithm/banker.c

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
        64      20.7     5.0      173.7        3            0      0.00    0.00
```

You should only need to pay attention to the following parameters:
- **NLOC**: Number of lines of code
- **CCN**: Code Complexity Number
- **location**: Lizard gives you a list of all the functions under the **location** column.

For intance  `checkResources@4-12@BankersAlgorithm/banker.c`: 
- `checkResources` is the name of the function
- `4-12@BankersAlgorithm/banker.c` means that the `checkResources` is between lines **4** and **12** of the file `BankersAlgorithm/banker.c`.

If any of the functions would have a CCN greater than 15, Lizard will let you know. For example, I run Lizard for an [open source project](https://sourceforge.net/projects/bacnet/files/bacnet-stack/bacnet-stack-0.8.3/) and I got the following output. 

```sh
===========================================================================================================                             
!!!! Warnings (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100) !!!!                             
================================================                                                                                        
  NLOC    CCN   token  PARAM  length  location                                                                                          
------------------------------------------------                                                                                        
      96     16    583      2     143 rr_address_list_encode@669-811@src/address.c
      45     17    256      4      53 decode_tag_number_and_value_safe@418-470@src/bacdcode.c
     125     20    704      3     146 cov_notify_decode_service_request@173-318@src/cov.c
      77     31    362      2     108 bacapp_encode_application_data@56-163@src/bacapp.c
      86     33    409      4     116 bacapp_decode_data@167-282@src/bacapp.c
      78     29    367      3     106 bacapp_encode_context_data_value@449-554@src/bacapp.c
     125     40    345      2     132 bacapp_context_tag_type@557-688@src/bacapp.c
...
```

# Rules of Thumb 

Usually, you should try to keep your CCN lower than 15, but in some cases is ok to go beyond that. 
But here are some rules of thumb from [Better Embedded System SW](https://betterembsw.blogspot.com/2017/08/the-spaghetti-factor-software.html).

Scoring:
- **5-10**: This is the sweet spot for most code except simple helper functions
- **15**: Don't go above this for most modules
- **20**: Look closely; review to see if refactoring makes sense
- **30**: Refactor the design
- **50**:  Untestable; throw the module away and fix the design
- **75**: Unmaintainable; throw the module away; throw the design away; start over
- **100**: Nightmare; probably you need to throw the whole subsystem away and re-architect it

# Conclusions
Writing code is easy, but writing good code requires discipline. It's not hard. Cyclomatic complexity is a measurement that tells us if we are overcomplicated our code, a good rule of thumb is to keep complexity under 15. 

In  a future post, I'll show how to use Docker to make your Code Quality Checker using Lizard, so keep in touch.
