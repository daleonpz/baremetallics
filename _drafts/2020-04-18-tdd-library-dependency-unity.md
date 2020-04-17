---
layout: post
title: TDD with third-party libraries dependency with Unity
category: programming
description: Test-Driven Development. Breaking third-party dependencies with Unity
---

This is the fourth post in our Test-Driven Development journey. 
At the beginning of the project nothing is well-defined.
Your team is not sure about the microprocessor or communication protocol.
But you still need to start with the development.  

**Image about breaking up - Here** 

We already talked about [how to use TDD to break hardware dependency]({% post_url programming/2020-04-11-tdd-cmock %}). Today, you'll learn how to apply TDD to free yourself from external libraries.

The concepts you'll read here are from the book [Test-driven development for embedded C](https://amzn.to/2wsWFnp).
We highly recommend you read it. 
You can find great books in our curated [booklist]({{site.baseurl}}/books).

# What are third-party libraries?
Nowadays is common to use libraries or code from external sources. Github. BitBucket. Software development kits (SDKs).  Just to mention some. 
These libraries are known as **third-party libraries**.

> Don't Reinvent The Wheel. Use third-party libraries.

Let's say you use third-party libaries for the I2C bus, UART bus and real-time operation system (RTOS). 
Look at the picture to have a better idea. 

![library dependency](/images/posts/library-dependency.png)

You will build your application based on this libraries. You are ready to apply TDD. **But what if they change?**

How can you make your application and test code bulletproof? 

# Bullteproof Code. Breaking Third-party libraries dependency.
In our previous [post]({% post_url programming/2020-04-11-tdd-cmock %}) you learned about how to use [CMock](http://www.throwtheswitch.org/cmock) to **emulate libraries' behaviors**

The idea with CMock was to use an alternative version of the original library. But only during testing. Check the next picture. 

![cmock](/images/posts/cmock-idea-test.png)

When you run your tests you won't use the original library. Instead you will use the mock version (dashed lines). 

Now, we are going one level higher. Check the next picture.

![fake library](/images/posts/fake-functions.png)

Let's say that your code may use the I2C or UART bus. But you are not sure. How can you still apply TDD?

The answer is using a **pointer to functions**. This will be the lord of the libraries. One pointer to rule them all.

# Small Tutorial
You can find the code for this tutorial **here - create repo**.
Remember, you aren't sure about which bus you'll use. I2C or UART. Also there are many implementations around there on the internet. 
It's a hard situation, right?

Let's start by writing our test.

```sh
STATIC uint8_t ReadI2CRegister( const uint8_t RegisterToRead)
{
    uint8_t value;
    i2c_device dev;

    dev = ad_i2c_open(LSM303AH_ACC);
    ad_i2c_transact( dev, &RegisterToRead, sizeof(RegisterToRead),
                &value, sizeof(value));
    ad_i2c_close(dev);

    return value;
}

```


In this post, you worked only with one test. You can find the full tests in the [source code](https://github.com/daleonpz/baremetallics/tree/master/_code/posts/tdd-intro/cmock-tutorial).

## Summary
Now you know the basic idea behind TDD and have a basic understanding of `Unity` and `CMock`. This approach is not perfect. But it increases our odds of writing testable code. And most importantly, easy to adapt and understand by computers and humans. 

But. If TDD helps you to develop better code in less time. Why don't use it?

Get one copy [Test-driven development for embedded C](https://amzn.to/2wsWFnp) on Amazon.


