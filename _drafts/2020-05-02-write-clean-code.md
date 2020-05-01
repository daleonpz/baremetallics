

Programmers think that they write code only when it compiles and runs.
But they forget that they also write for humans.

> It doesn't matter if you write the most beautiful poetry. If nobody can't read it.


Think for a moment about the next questions:


- Who do you think will read your code in tomorrow, in 1 month, in year? 
- How many times you didn't understand the code your wrote last week,  last month or even last year?
- How many times you had to read the source code of a function to understand what the function did?
- How many times you forget the beginning of the function because it was too long? 
- How many times you had to use `printfs` to understand big chunks of code?
- How many times you opened a file and your eyes bled from seeing the code?

you got the point, right?

# Clean Code Benefits 

We understand that delivering a product on time is important. But we shouldn't give up the quality.

Here are three main benefits of writing clean code.

- **Fewer Bugs**.  It would be easier to spot bugs during your code review with your team. Because it would be easier to notice inconsistencies. 
- **Easy to Mantain**. Good names. Short functions. Good Formating. Good Comments. No duplicated code. That makes your code easier to read and understand. 
- **Adaptation**. Because it's easier and faster to modify something with a clear structure and names than a spaguetti code.

# How To Write Clean Code

We give you some tips I learned from the book [Clean Code](https://amzn.to/2J7mMTE).
You can apply this tips to any code. But we focused on C.

> We want our code to be a quick skim, not an intense study. _Robert Martin_

## Choose Good Names
Characters are free. Use them to make your code easier to read. We're humans. We're good at words. 


**Avoid abbreviations**. Sometimes abbreviations are clear. For example SPI, UART. But most of the time are confusing. Or even worst. You could forget about their meaning.  

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

**Use Pronunceable Names**. Did ever feel weird saying "pee ess zet is an int"? 

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

**Small Function**.
A [study](https://pdos.csail.mit.edu/archive/6.097/readings/osbugs.pdf) showed that error rates for big functions were two to six times higher than for smaller routines.

The question is now... How small is small? Of course 8000 lines abominations is far from the answer.

Some people say. As long as it fits on your screen is okay. Other says. As long as you can keep it in your head is okay.

You will find the answer with experience. And by following good practices. 

**Code indentation**. 
This is an understimated tip. Sometimes a `for` or `while` loop, or even a simple `if` statement doesn't work well because we forgot the brackets. 
Sometimes a function is hard to read only because the indentation is bad.

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

If you want to more examples. [Check this summary](https://www2.cs.arizona.edu/~mccann/indent_c.html#ThreeOne) from the University of Arizona.

**They Do as They Say**. Functions shouldn't have hidden behaviors. 

>  you know you are working on clean code when each routine turns out to be pretty much what you expected._Ward Cunningham_

For example. If a function name is `TurnOnModem`, you expect that if you call this function your modem will turn on. Not to turn on and connect to a network. 

**Function Arguments**. Functions with many arguments are a pain in the ass from the testing point of view. Each argument increases the number of test scenarios. 

In this [post]({% post_url programming/2020-04-11-tdd-cmock %}). We show a practical example using a I2C library from Dialog microcontrollers. 

If there are no arguments. Testing is trivial. If there are one or two arguments. Testing is still easy. 

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



