---
layout: post
title: Understanding C declarations 
category: programming
---

In this journey to learn the most obscure secrets of C, I realized that somehow I could read C declarations but I didn't have a deep understanding of it. 
I didn't have problems with declaration such as `char *p[13]` or `void (*p)(**void)`, but for more complicated declarations such as `char (*(*x[3])())[5]`, I did have a hard time.
And even after I understood what the declaration meant I wasn't very sure. 


In the book [The C programming Language](https://www.amazon.com/Programming-Language-Brian-W-Kernighan/dp/0131103628) section 5.12, the authors explain the grammar of C declaration and show the a simple code to _"Translate C to English"_. 
But the explanation is not straightforward. 
On the other hand, in the book [Expert C programming](https://www.amazon.com/Expert-Programming-Peter-van-Linden/dp/0131774298/ref=sr_1_1?ie=UTF8&qid=1492784910&sr=8-1&keywords=Expert-Programming-Peter-van-Linden), the author shows in chapter 3 an _"algorithm"_ that it is really easy to apply.

I will show you how to read declarations with an example. Let's use `char (*(*x[3])())[5]`.

* *Find the identifier.* 
    In this case is `x`. So **x is a**.
* *Check whether there is a `[` or `(` on the right side.*
    `x[` **x is an array**
* In this case we reach a `[` and there is a `3` before the next `]` on the right. `x[3]`
    **x is an array [3] of**
* There is no `[` or `(` next to `x[3]`. So let's check the left side.  
    `*x[3]`    **x is an array [3] of pointers to**
* `*x[3]` is enclosed in `()`.  
* *Check whether there is a `[` or `(` on the right side of `(*x[3])`.*
    There is a `(` followed by `)`. **x is an array [3] of pointers to function returning**
* Check the left side of `(*x[3])()`. `*(*x[3])()`  **x is an array [3] of pointers to function returning a pointer to**
* `*(*x[3])()`  is enclosed in `()`.
* Check right side of `(*(*x[3])())`. There is a `[5]`.  **x is an array [3] of pointers to function returning a pointer to array [5] of**
* Check left side of  `(*(*x[3])())[5]`. There is a data type `char`.**x is an array [3] of pointers to function returning a pointer to array [5] of type char** 

Here are more examples in which I applied the same algorithm.
   
```c
/* x is a pointer to read-only int */
int const * x;

/* x is a const or read-only pointer to int */
int * const x;

/* p a array [3] of pointers to int*/
int *p[3]

/* p is a pointer to an array [3] of int */
int (*p)[3]

/* p is a pointer to a function returning an int*/
int (*p)()

/* p in a function returning a pointer to function 
returning an int */
int (*p())()

/* x is a pointer to function returning a pointer 
to read-only pointer to char  */
char * const *(*x)();

/* c is an array of pointers to function
that receive a pointer to pointer to int
and returns a pointer to char */
char *(*c[10])(int **p);
```

By now you should be able to understand any C declaration.

But there is one more step: **implement the algorithm**, a basic version of course, no error checking, no input parameters in functions.


# Implementation!!!! wah wah wah waaaaaaaaahhh

First things first, I will need to: 
* Detect whether the input is a data **TYPE** (`long, char, float, int, double, short, unsigned, signed, struct, enum, union`), **QUALIFIER** (`const, volatile`) or **IDENTIFIER**.
* Get tokens. The input is divided in tokens, for example `char *p` has 3 tokens: `char, *, p`.
* Translate the token to English for example `*` to **pointer to**.
* This last part can be divided into multiple parts: get the identifier (there is only one), detect arrays, detect functions and pointers. 

With the help of the books I mentioned at the beginning of this post, let's start. 

## Global variables
I will defined the following variables:

```c
#define MAXTOKENS 100
#define MAXTOKENLEN 64

enum token_type { IDENTIFIER, QUALIFIER, TYPE};

/* the token has type and a value */
struct token { 
    char type;
    char string[MAXTOKENLEN]; 
};

/* I will use the concept of a LIFO to save the tokens 
into memory as I reach the identifier  */
int top=-1;

#define pop stack[top--]
#define push(s) stack[++top]=s

struct token stack[MAXTOKENS];

/* holds the token just read */
struct token this;
```

## Detect input type
To achieve this I used `strcmp`. `strcmp` returns `0` if the strings are equal.

```c
enum token_tag classify_string(void){
    char *s = this.string;
    
    if (!strcmp(s,"const")) return QUALIFIER;
    if (!strcmp(s,"volatile")) return QUALIFIER;
    if (!strcmp(s,"void")) return TYPE;
    if (!strcmp(s,"char")) return TYPE;
    if (!strcmp(s,"signed")) return TYPE;
    if (!strcmp(s,"unsigned")) return TYPE;
    if (!strcmp(s,"short")) return TYPE;
    if (!strcmp(s,"int")) return TYPE;
    if (!strcmp(s,"long")) return TYPE;
    if (!strcmp(s,"float")) return TYPE;
    if (!strcmp(s,"double")) return TYPE;
    if (!strcmp(s,"struct")) return TYPE;
    if (!strcmp(s,"union")) return TYPE;
    if (!strcmp(s,"enum")) return TYPE;

    return IDENTIFIER;
}
```

## Get tokens

```c
void gettoken(void){
    char *p = this.string;

    /* omit simple white spaces */
    while ((*p = getchar()) == ' ')  ;

    /* get numbers or words */ 
    if ( isalnum(*p)){
        while ( isalnum(*++p=getchar()) );
        /* ungetc since the last read is 
           not alphanumeric and null 
           character should be at the end  */
        ungetc(*p,stdin);
        *p = '\0';
        this.type=classify_string();
        return;
    }

    /* it could be also a pointer */
    if (*p == '*'){
        this.type = '*';
        strcpy(this.string, "pointer to");
        return;
    }
   
    /* empty string */ 
    this.type = *p;
    this.string[0] = '\0';
    return;
}
```

## Translate to English
First let's start with the identifier.

```c
void read_identifier(void){
   
    while( this.type != IDENTIFIER )  {
        push(this)
        gettoken();
        };
    gettoken(); // update this

    printf("%s is a ", this.string);
     
}
```

Now let's focus on detect function. `deal_with_function_args` will read past `)` and print out **function returning**.

```c
void deal_with_function_args(void){

    while ( this.type != ')') 
        gettoken();

    gettoken(); // update this
    printf("function returning ");
}
``` 

It's time to detect arrays.

```c
void deal_with_arrays(void){

    while( this.type != ']'){
        printf("array of [");
        /* we just need to detect the first digit 
            atoi will do the rest */
        if ( isdigit (this.string[0]) ){
            printf("%s", atoi(&this.string[0])  );
            gettoken();
        }     
        gettoken()
    }
    
    gettoken(); //update this
    printf("] "); 

```

Finally pointers.

```c
void deal_with_pointers(void){
    /* '*' is a  left side token , and all of them 
    are in the stack */
    while( stack[top].type == '*'){

    /* this could be an option
        printf("pointer to");
        pop;

       but don't forget that when type = '*'
       then string is "pointer to" and also
       pop in stack[top--]
    */
        printf("%s ",pop.string)
    }  
}
```

## Putting all together
Now that we finished to code the tools needed, it is time to put all together.  We still need to code the declaration manager.

```c
/* for example if the input is 
     int (*p)() 
        call read_identifier()
            in stack [ int , (, * ]

        call declarator
            call deal_with_pointers
                     int (*p) ()
                this.string ^
            pop stack -->  stack [int, ( ]
            
            call gettoken
                    int (*p) ()
                this.string  ^

            call declarator
                call deal_with_function_arg
                            int (*p)()
                        this.string  ^
                pop stack --> stack [ int ]
                call gettoken()
                            int (*p)() 
                    this.string        ^ 

                call declarator
                    print stack.string = int

*/ 
void declarator(void){

    if (this.type == '[')
        deal_with_arrays();

    if (this.type == '(')
        deal_with_function_args();

    deal_with_pointers();
    
    /* let's empty the stack*/
    while( top != -1){
        if (stack[top].type == '(' ){
            top--;
            gettoken();
            declarator();
        }
        else {
            print("%s ", pop.string);
        }
    }
}
```

So the main function is as follows:

```c
int main(){
    read_identifier();
    declarator();
    printf("\n");
    return 0;
}
```

# Last words
I hope you master the C declarations. The code that I implemented is the challenge at the end of chapter 3 of [Expert C Programming](https://www.amazon.com/Expert-Programming-Peter-van-Linden/dp/0131774298/ref=sr_1_1?ie=UTF8&qid=1492784910&sr=8-1&keywords=Expert-Programming-Peter-van-Linden).
