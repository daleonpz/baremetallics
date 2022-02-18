---
layout: post
title: TDD with third-party libraries dependency
category: programming
description: Test-Driven Development. Breaking third-party dependencies
---

This is the fourth post in our Test-Driven Development journey. 
At the beginning of a project, nothing is well-defined.
Your team is not sure about the microprocessor or communication protocol.
But you still need to start with the development.  

![library dependency break up](/images/posts/library-dependency-breakup.png)


We already talked about [how to use TDD to break hardware dependency]({% post_url programming/2020-04-11-tdd-cmock %}). Today, you'll learn how to apply TDD to free yourself from external libraries.

The concepts you'll read here are from the book [Test-driven development for embedded C](https://amzn.to/2wsWFnp).
We recommend you read it. 
And you can find more great books in our curated [booklist]({{site.baseurl}}/books).

# What are third-party libraries?
Nowadays is common to use libraries or code from external sources. Github. BitBucket. Software development kits (SDKs).  Just to mention some. 
These libraries are known as **third-party libraries**.

> Third-Party Libraries Are Great. You Shouldn't Reinvent The Wheel.

For example. Let's say you use third-party libraries for the I2C bus, UART bus and real-time operating system (RTOS). 
Look at the picture to have a better idea. 

![library dependency](/images/posts/library-dependency.png)

You will build your application based on these libraries. You are ready to apply TDD. Everything sounds great. 

**But what if they change?** What if they don't have support anymore? What if your client decided to change the communication protocol?

So... How can you make your application and test code bulletproof? 

# Bulletproof Code. Breaking Third-party libraries' dependency.
In our previous [post]({% post_url programming/2020-04-11-tdd-cmock %}), you learned about how to use [CMock](http://www.throwtheswitch.org/cmock) to **emulate libraries' behaviors**

The idea behind CMock was to use an alternative version of the original library. But only during testing. Check the next picture. 

![cmock](/images/posts/cmock-idea-test.png)

When you run your tests you won't use the original library. Instead, you will use the mock version (dashed lines). 

Now, we are going one level higher. Check the next picture.

![fake library](/images/posts/fake-functions.png)

Let's say that your code may use the I2C or UART bus. But you are not sure. How can you still apply TDD?

The answer is using a **pointer to a function**. This will be the lord of the libraries. One pointer to rule them all.

# Example with Code
You can find the code for this tutorial [on github](https://github.com/daleonpz/tdd-break-library-dependency).
Remember, you aren't sure about which bus you'll use. I2C or UART. Also, there are many implementations around there on the internet. 
It's a hard situation, right?

## Application with Third-party libraries 
Let's assume that the header file `BusDriver.h` of our application looks like this.

```c
/* code/include/BusDriver.h */
#ifndef D_BUS_DRIVER_H
#define D_BUS_DRIVER_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "i2c_adapter.h"
#include "uart_adapter.h"


uint8_t ReadFromI2CBus(void);
uint8_t ReadFromUARTBus(void);
uint8_t GetData(void);

#endif  /* D_BUS_DRIVER_H */
```

Here are two problems that make the code not testable.  
- We are depending on two third-party libraries `i2c_adapter.h` and `uart_adapter.h`.
- We are wasting time focusing on something that may change in the near future.

You know that the trick here is using **pointers to functions**. But why?
The goal is to create a common interface for all possible scenarios. That's called [Polymorphism](https://en.wikipedia.org/wiki/Polymorphism_(computer_science)).

##  Pointer to function
So let's assume that all functions that implement communication buses follow this pattern.

```c
uint_8  FunctionName(void);
```

So the pointer to function would be.

```c
uint_8 (*ReadBus)(void);
```

## First Test
Here you will learn how to use pointers to functions with TDD.
Like any other TDD cycle. Let's create first a test.

```c
/* code/test/test_BusDriver.c */
#include "unity.h"
#include "BusDriver.h"
#include "FakeBusDriver.h"

void setUp(void)
{
}

void tearDown()
{
}


void test_GetData()
{
    uint8_t expected = 0x0E;
    uint8_t dataFromFunction = 0;
    ReadBus = FakeReadBus;

    dataFromFunction = GetData();

    TEST_ASSERT_EQUAL_UINT8(expected, dataFromFunction);
}
```

Things to notice:
- `#include "FakeBusDriver.h"`. 
- In `test_GetData` test. `ReadBus = FakeReadBus;`.

What is `FakeReadBus.h`? 
This header includes all the possible scenarios. I2C? UART? SPI?. Any generic scenario? No problem. 
Because what it's important isn't the protocol. But the data. That's why... 

We only need something that gives you data back. 

```c
/* code/test/support/FakeBusDriver.h */
#ifndef _FAKE_BUS_DRIVER_H_ 
#define _FAKE_BUS_DRIVER_H_

#include <stdint.h>
#include <stdlib.h>

uint8_t FakeReadBus(void);

#endif 
```

In this case is `FakeReadBus`;

```c
/* code/test/support/FakeBusDriver.c */
#include "FakeBusDriver.h"

uint8_t FakeReadBus(void)
{
    return 0xDE;
}
```

**You will see in the next section the reason behind** `ReadBus = FakeReadBus`. But for now. It makes `test_GetData` call `FakeReadBus` function. 

If you want to emulate more data outputs. You just need to add more **fake functions**. 
And call them in your tests following this syntax `ReadBus = <FAKE-FUNCTION>`.


## Coding our application 
Since we want a bulletproof code. We need to minimize third-party libraries dependency.  

Let's start with the header  `BusDriver.h`.

```c
#ifndef _BUS_DRIVER_H_
#define _BUS_DRIVER_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint8_t GenericBus(void);
uint8_t (*ReadBus)(void);
uint8_t GetData(void);

#endif  /* _BUS_DRIVER_H */
```

As you can see. No third-party libraries. Only standard libraries. 

And the initial implementation of our application would be.

```c
#include "BusDriver.h"

uint8_t GenericBus(void)
{
    return 0x00;
}

uint8_t (*ReadBus)(void) = GenericBus; 

uint8_t GetData(void)
{
    uint8_t data = 0;
    data = ReadBus();
    data &= 0x0F;
    return data;
}
```

Everything seems to make sense but for one thing. `GenericBus`. This function is there only to initialize the **pointer to function** `ReadBus`. That's all.  

But during testing. `ReadBus` points to `FakeReadBus`. That makes your code adaptable. As we mentioned before. `FakeReadBus` could be virtually any bus.  

## Testing the code

```sh
./RunTestOnDocker
Test 'test_BusDriver.c'
-----------------------
Generating runner for test_BusDriver.c...
Compiling test_BusDriver_runner.c...
Compiling test_BusDriver.c...
Compiling unity.c...
Compiling BusDriver.c...
Compiling FakeBusDriver.c...
Compiling cmock.c...
Linking test_BusDriver.out...
Running test_BusDriver.out...

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  1
PASSED:  1
FAILED:  0
IGNORED: 0
```

Now you have a functional and tested code without third-party libraries dependency.


## Summary
At the beginning of a project, nothing is well-defined. Your team is not sure about the microprocessor or communication protocol. But you still need to start with the development.
Using a simple **pointer to function**  you can start your development under those circumstances.

You learned how to still apply TDD even under uncertainty. If you want to learn ever more about TDD. 
Then get one copy of [Test-driven development for embedded C](https://amzn.to/2wsWFnp) on Amazon.


