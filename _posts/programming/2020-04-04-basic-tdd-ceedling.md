---
layout: post
title: Test-Driven Development (TDD) with ceedling 
category: programming
description: Test-Driven Development example with ceedling
tags: [testing, tools, quality]
---

We gave already an introduction to Test-Driven Development based on the  book [Test driven development for embedded C](https://amzn.to/2wsWFnp) in a previous [post]({% post_url programming/2020-03-28-tdd-intro %}).
In this post, you will learn how to apply TDD using **Unity**. A well-know testing framework in the embedded world.

Keep reading to see that beauty in action. 

## Testing Framework for Test-Driven Development 
First, you should know what Ceedling and Unity are. 

[Ceedling](http://www.throwtheswitch.org/ceedling) is a build system for C projects. It works on top of [Unity](http://www.throwtheswitch.org/unity), a unit testing for C. That means two things:  

- We build and run tests with ceedling.
- We use Unity special syntax to code our tests. 

You will have a clearer idea when you see **Unity** and **Ceedling** in action in the following section.

## TDD example with Ceedling and Unity
The goal in this section is that you understand TDD workflow and ceedling.
TDD workflow is as follows. **Writing a test is the first step.**

![tdd workflow](/images/posts/tdd-workflow.png)

Why should we write a test first? Because before coding, we should know how our module behaves based on its inputs and outputs.

In this section, we'll develop a square function for our calculator module. You can find the code [here](https://github.com/daleonpz/rechner). `Rechner` is the project name. The basic project structure is:

```sh
├── ceedling
├── code
│   ├── include
│   ├── src
│   └── test
├── project.yml
└── tools
    └── vendor
```

- `ceedling` is a build system for C project.
- `code/include` is where the headers `*.h` are.
- `code/src` is where `*.c` files are.
- `code/test` is where our test files. It doesn't only include the tests files, but also any additional file that can help us to run the tests. 
- `project.yml` is ceedling configuration file.
- `tools/vendor` is ceedling source code. It includes `CMock` and `Unity`.

## TDD First Step - Write a Test
Let's start by writing the first test:

```c
/* code/test/test_rechner.c */
#include "unity.h"
#include "rechner.h"

void setUp(void)
{
}

void tearDown()
{
}

void test_SquareOfFive(void)
{
    int16_t result;
    int16_t x = 5;

    result = mySqrt(x);

    TEST_ASSERT_EQUAL_INT16(25, result);
}
```

Things to notice:
- Ceedling/Unity format for test files is  `test_<MODULE_UNDER_TEST>.c`. 
- The header `unity.h` is the testing framework, and `rechner.h` is the module under test. 
- Functions `setUp` and `tearDown` are the _constructor_ and _destructor_.  Everything inside `setUp` executes at the beginning of each test. And everything inside `tearDown` executes at the end of each test.
- Test function format is `void test_<TEST_NAME> (void)`.
- `TEST_ASSERT_EQUAL_INT16` is a unity assertion.  You can find unity assertions cheatsheet [here](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsCheatSheetSuitableforPrintingandPossiblyFraming.pdf).

**Tip**
> Your code should be self-documented, try to avoid abbreviations. Let me ask you something. Which is easier to understand? `t` or `temperature`. `test_SquareOfFive` or `test_sqrt5`. Letters are free. Computers should understand your code, and humans too. Read [Clean Code](https://amzn.to/2J7mMTE) for more tips. 

## TDD Step Two - Step Check Failures
To build and run our test we use ceedling.

```sh
./ceedling test:<MODULE_UNDER_TEST>
```

In our case:
```sh
./ceedling test:rechner
Test 'test_rechner.c'
---------------------
code/test/test_rechner.c:2:10: fatal error: rechner.h: No such file or directory
 #include "rechner.h"
          ^~~~~~~~~~~
```

## TDD Third Step - Write Code
We will write enough code just to pass the test. **Try to avoid the temptation to code the full module.** Remember. The goal is to develop software in small steps at a rapid and steady pace.

Let's start with the header file `rechner.h`.

```sh
/* code/include/project/rechner.h */
#ifndef _RECHNER_H_
#define _RECHNER_H_

#include <stdint.h>

int16_t  mySqrt(int16_t x); 

#endif /* _RECHNER_H_ */
```

Now the implementation `rechner.c`.

```sh
#include "rechner.h"

int16_t  mySqrt(int16_t x){
    return 25;
}
```

You may be wondering. Why `return 25` and not `x*x`. Again, we write enough code to pass the tests. We will write more tests that will tell if our implementation is correct or not.

## TDD Fourth Step - All tests pass
We build and run our tests with ceedling.

```sh
./ceedling test:rechner
Test 'test_rechner.c'
---------------------
Generating runner for test_rechner.c...
Compiling test_rechner_runner.c...
Compiling test_rechner.c...
Compiling unity.c...
Compiling rechner.c...
Compiling cmock.c...
Linking test_rechner.out...
Running test_rechner.out...

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  1
PASSED:  1
FAILED:  0
IGNORED: 0
```

All our tests are successful.

## TDD Fifth Step - Refactoring
 There is nothing to refactor yet. So we skip this step and we start again the cycle. 

**Note**:From here this tutorial will skip some explanations. You will be able to follow the workflow.

## And Repeat
Let's write a second test:
```c
/* test_rechner.c */
void test_SquareOfAnyNumber(void)
{
    int16_t result;
    int16_t x = 20;

    result = mySqrt(x);

    TEST_ASSERT_EQUAL_INT16(400, result);
}
```

Build and run with ceedling.

```sh
./ceedling test:rechner

Clobbering all generated files...
(For large projects, this task may take a long time to complete)



Test 'test_rechner.c'
---------------------
Generating runner for test_rechner.c...
Compiling test_rechner_runner.c...
Compiling test_rechner.c...
Compiling unity.c...
Compiling rechner.c...
Compiling cmock.c...
Linking test_rechner.out...
Running test_rechner.out...

-------------------
FAILED TEST SUMMARY
-------------------
[test_rechner.c]
  Test: test_SquareOfAnyNumber
  At line (29): "Expected 400 Was 25"

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  2
PASSED:  1
FAILED:  1
IGNORED: 0

---------------------
BUILD FAILURE SUMMARY
---------------------
Unit test failures.
```

There it is. The second test shows us an error. 

Let's update our code to make the test pass.

```c
/* rechner.c */
#include "rechner.h"

int16_t  mySqrt(int16_t x){
    return x*x;
}
```

Build and run our tests:
```sh
./ceedling test:rechner

Clobbering all generated files...
(For large projects, this task may take a long time to complete)



Test 'test_rechner.c'
---------------------
Generating runner for test_rechner.c...
Compiling test_rechner_runner.c...
Compiling test_rechner.c...
Compiling unity.c...
Compiling rechner.c...
Compiling cmock.c...
Linking test_rechner.out...
Running test_rechner.out...

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  2
PASSED:  2
FAILED:  0
IGNORED: 0
```

Let's write another test. Testing boundaries is always a good idea.

```c
/* test_rechner.c */
void test_SquareOfABigNumber(void)
{
    int32_t result;
    int16_t x = 1000;

    result = mySqrt(x);

    TEST_ASSERT_EQUAL_INT32(1000000, result);
}
```

Build and run our test.

```sh
./ceedling test:rechner

Clobbering all generated files...
(For large projects, this task may take a long time to complete)



Test 'test_rechner.c'
---------------------
Generating runner for test_rechner.c...
Compiling test_rechner_runner.c...
Compiling test_rechner.c...
Compiling unity.c...
Compiling rechner.c...
Compiling cmock.c...
Linking test_rechner.out...
Running test_rechner.out...

-------------------
FAILED TEST SUMMARY
-------------------
[test_rechner.c]
  Test: test_SquareOfABigNumber
  At line (39): "Expected 1000000 Was 16960"

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  3
PASSED:  2
FAILED:  1
IGNORED: 0

---------------------
BUILD FAILURE SUMMARY
---------------------
Unit test failures.
```

It seems like an overflow problem. Let's update our code. We change `int16_t` to `int32_t`.

```c
/* rechner.c */
#include "rechner.h"

int32_t  mySqrt(int16_t x){
    return x*x;
}

/* rechner.h */
#ifndef _RECHNER_H_
#define _RECHNER_H_

#include <stdint.h>

int32_t  mySqrt(int16_t x); 

#endif /* _RECHNER_H_ */
```

Run and build out tests.
```sh
./ceedling test:rechner

Clobbering all generated files...
(For large projects, this task may take a long time to complete)



Test 'test_rechner.c'
---------------------
Generating runner for test_rechner.c...
Compiling test_rechner_runner.c...
Compiling test_rechner.c...
Compiling unity.c...
Compiling rechner.c...
Compiling cmock.c...
Linking test_rechner.out...
Running test_rechner.out...

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  3
PASSED:  3
FAILED:  0
IGNORED: 0
```

That's it. That's the idea behind TDD.

## Conclusion
Now you know the basic idea behind TDD and have a basic understanding of `Unity` and `CMock`. 
**Test-Driven Development** is not perfect. But it increases our odds of writing adaptable and testable code. 

But. If TDD helps you to develop better code in less time. Why don't use it?

Get one copy [Test driven development for embedded C](https://amzn.to/2wsWFnp) on amazon.

