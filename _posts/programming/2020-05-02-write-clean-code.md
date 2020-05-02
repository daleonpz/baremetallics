---
layout: post
title: How To Write Clean Code
category: programming
description: Learn how to write clean C Code
---


Developers think that they write code only when it compiles and runs.
But they forget that they also write for humans.

> It doesn't matter if you write the most beautiful poetry. If nobody can't read it.


Think for a moment about the next questions:

- How many times you didn't understand the code you wrote last week,  last month or even last year?
- How many times you had to read the source code of a function to understand what the function did?
- How many times you had to use `printf` to analyze a function because the code wasn't clear?
- How many times you forgot the beginning of the function because it was too long? 
- How many times you opened a file and your eyes bled from seeing the code?

you got the point, right?

# Clean Code Benefits 

We understand that delivering a product on time is important. But we shouldn't give up the quality.

Here are three big benefits of writing clean code.

- **Fewer Bugs**.  It would be easier and faster to spot bugs during code review with your team. Because it would be easier to notice inconsistencies. 
- **Easy to Maintain**. Good names. Short functions. Good Formating. Good Comments. No duplicated code. That's the meaning of happiness for a developer. 
- **Adaptation**. Because it's easier and faster to modify code with a clear structure than spaghetti code.


There is one extra benefit.
- **It's Visually Pleasing**. The clean code doesn't intimidate you. You have the feeling that it's easy to understand. 

# Don't live with broken windows

[The broken window theory](https://www.britannica.com/topic/broken-windows-theory) is an academic theory proposed by James Q. Wilson and George Kelling in 1982. 

One broken window, when left unrepaired for awhile, gives the inhabitants of the building a sense of abandonment. It gives a sense that nobody cares about the building. 

> So another window gets broken.

People start littering. Graffiti appears. Serious structural damage begins. In a short space of time, the building becomes damaged beyond the owner's desire to fix it. **And the sense of abandonment becomes reality.**


Bad designs. Poor code. Compilation warnings.

Don't leave "broken windows" unrepaired. Fix each one as soon as you see them. And if there is "no enough time" to fix it. Then you can comment out the broken parts. 

Be a good boy scout. **Leave the code better than you found it.** 

# How To Write Clean Code

We give you some tips I learned from the book [Clean Code](https://amzn.to/2J7mMTE).
You can apply these tips to any language. But we focus on C.

> We want our code to be a quick skim, not an intense study. _Robert Martin_

## Choose Good Names
Characters and blank space are free. Use them. Make your code easier to read. We're humans. We're good at words. 


**Avoid abbreviations**. Sometimes abbreviations are clear. For example SPI and UART. But most of the time they are confusing. Or even worst. You could forget about their meaning.  

Compare

```c
int s,m,h;

s = getSecs();
m = s*60;
h = m*60; 
```

to

```c
int Seconds, Minutes, Hours;

Seconds = getSeconds();
Minutes = Seconds*60;
Hours = Minutes*60; 
```

**Use Pronounceable Names**. Did ever feel weird saying "pee ess zet is an int"? 

Compare

```c
void RecDat( int t);
```

to

```c
void RecordData(int Timestamp);
```

**Use Searchable Names**. What would be easier to find, `5` or `WORK_DAYS_PER_WEEK`?

Compare

```c
for( int j=0; j<5; j++)
{
     ... 
}
```
to

```c
#define WORK_DAYS_PER_WEEK  5;

for( int j=0; j < WORK_DAYS_PER_WEEK; j++)
{
    ...
}
```

## Write Good Functions
We use functions every day. Keep them clean. Put some effort. Show that you're a professional.

**Small Function**.

> A [study](https://pdos.csail.mit.edu/archive/6.097/readings/osbugs.pdf) showed that error rates for big functions were two to six times higher than for smaller routines.

The question is now... How small is small? Of course, a 1000 lines abomination is far from the answer.

Some people say. As long as it fits on your screen is okay. Others say. As long as you can keep it in your head is okay.


**Code indentation**. 
This is underestimated. Sometimes a `for` or `while` loop, or even a simple `if` statement doesn't work well because we forgot the brackets. 
Sometimes a function is hard to read because the indentation is bad.

Compare

```c
int binary_search (struct collection list[], int first_index,
int last_index, key_type target);

...

if (victor(human)) {
human_wins++;
printf("I am your humble servant.\n");
} else {
computer_wins++;
printf("Your destiny is under my control!\n");
}

```

to 

```
int binary_search (struct collection 	list[], 
		   int 			first_index,
		   int 			last_index, 
		   key_type 		target);

...

if (victor(human)) 
{
	human_wins++;
	printf("I am your humble servant.\n");
} else 
{
	computer_wins++;
	printf("Your destiny is under my control!\n");
}

```

If you want more examples. [Check this summary](https://www2.cs.arizona.edu/~mccann/indent_c.html#ThreeOne) from the University of Arizona.

**They Do as They Say**. Functions shouldn't have hidden behaviors. 

>  You know you are working on clean code when each routine turns out to be pretty much what you expected. _Ward Cunningham_

For example. If a function name is `TurnOnModem`, you expect that if you call this function your modem will turn on. Not to turn on and connect to a network. 

**Function Arguments**. Functions with many arguments are a pain in the ass from the testing point of view. Each argument increases the number of test scenarios. 

In this [post]({% post_url programming/2020-04-11-tdd-cmock %}). We show a practical example using a I2C library from Dialog microcontrollers. 

If there are no arguments. Testing is trivial. If there are one or two arguments. Testing is still manageable. 

You could reduce the number of arguments by using structures.

Compare 


```c

int CalculateVolumeRectangularPrism( int length, int height, int width);
```

to

```c
typedef struct {
    int length;
    int height;
    int width;
} RectangularPrism;

...
int CalculateVolumeRectangularPrism(RectangularPrism Prism);

```


**Avoid Duplicated Code**. Have you ever heard about DYR? It stands for **Don't Repeat Yourself**.
Duplicated code is hard to maintain. Because if you want to modify a line or variable. You must change it on every single copy. 

People make mistakes. And we get tired. That's why you may forget about a copy. You may write the wrong value. 

> Duplicated code is the root of all evil. 

So it's always better to have only one entry point. If there is an error. The error only "infects" one.

# Summary
Developers think that they write code only when it compiles and runs. But they forget that they also write for humans.
And sometimes we forget that we're humans. We make mistakes. And we get tired. 
And to keep our code clean. We must be disciplined. 

You learned some tips that will help you 100% in your journey to mastery.

If you liked this post. **Subscribe right now**. You'll get our stuff direct in your inbox. 

