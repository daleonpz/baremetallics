---
layout: post
title: Tips for Writing Embedded Unit Tests
category: programming
description: Unit testing helps ensure the reliability of software, especially in embedded systems where failures can be costly. In this post, we’ll cover behavior-driven testing, understanding test size and scope, and how to prepare your system for failures.
---

Unit testing helps ensure the reliability of software, especially in embedded systems where failures can be costly. In this post, we’ll cover behavior-driven testing, understanding test size and scope, and how to prepare your system for failures.

The information here is based on the book **Software Engineering at Google: Lessons Learned from Programming Over Time** by Titus Winters, Tom Manshreck, and Hyrum Wright. It provides insights into how Google approaches software development, including testing practices. So check it out if you have the chance! ... it's quite long, but worth it. :)

* 
{:toc}

# Behavior-Driven Testing: Focus on What the System Does

Instead of writing a test for each function or method, it's better to focus on how the system behaves. This makes your tests clearer and easier to maintain as the system grows. A good way to write behavior-driven tests is using the **Given/When/Then** structure:

* **Given**: Set up the conditions for the test.
* **When**: Define the action being tested.
* **Then**: Check the expected outcome.

Here’s a simple example using Catch2 for an embedded system that controls an LED based on temperature readings:

```cpp
TEST_CASE("LED should turn on when temperature is too high", "[LED]") {
    // Given the system is initialized and the temperature is normal
    System system;
    system.setTemperature(25);

    // When the temperature goes above 50 degrees
    system.setTemperature(60);

    // Then the LED should turn on
    REQUIRE(system.isLedOn() == true);
}
```

This test focuses on the behavior (LED turns on) rather than testing each function separately. 

# Test Size and Scope

At Google, tests are categorized by size (resources needed) and scope (how much code is tested). Here's a basic breakdown:

* **Small tests**: Focus on individual methods or small components (e.g., unit tests). This tests are usually fast and easy to write.
* **Medium tests**: Test how components interact (e.g., integration tests). This tests may involved python scripts, or other tools.
* **Large tests**: Test the entire system (e.g., end-to-end tests). This may include different hardware components.

A good strategy is to have a mix of:

* 80% small (unit) tests,
* 15% medium (integration) tests,
* 5% large (system) tests.

This ensures quick, reliable tests for most of the system, with broader tests to cover overall functionality.

# The Beyoncé Rule: Test What You Care About

Test everything that matters. If a system behavior is important, write an automated test for it. This includes not just regular functionality, but how the system handles failures.

> If you liked it, then you shoulda put a test on it.

Failures will happen, so make sure your tests include failure scenarios. For example, if you want to know how your system responds to a sensor failure, simulate that in your tests. This will prepare your system to handle problems in real-world conditions.

# Conclusion
In embedded systems are complex systems due to the hardware interaction, but unit testing can help ensure their reliability. And to be more effective you could implement at least a behavior-driven testing approach.
