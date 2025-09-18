---
layout: post
title: V-Model for Embedded Systems - Tools and Testing Workflow
category: programming
description: Learn how to apply the V-Model in embedded systems development. From requirements traceability to unit, integration, and acceptance testing using Zephyr, PyTest, and Robot Framework.
tags: [embedded, testing, v-model, zephyr, pytest, robot-framework]
---

The **V-Model** is a product development lifecycle that maps design phases on one side and testing phases on the other. It’s simple, easy to follow, and works well for small and medium projects like smart appliances or wearables.

In this post, I’ll walk you through the tools I use to apply this model in real projects.

* 
{:toc}

![v-model](/images/posts/v-model.png)

# Tools

You don’t need a massive toolchain to build a product your customers will enjoy. For small projects, I keep it simple and focus on tools that give me traceability, repeatability, and automation.


## Requirements and Traceability

You can manage requirements in a spreadsheet or even a notepad if the project is small. But there are two things you absolutely need:

* **Traceability**: link requirements to acceptance tests (both ways). That means you should always see which requirements are untested and which tests don’t map to a requirement.
* **Unique IDs**: every requirement should have its own identifier.

I’ve tried many tools, but most are either overkill or don’t cover these basics. So I built my own: [**Raiz**](https://github.com/daleonpz/raiz). It stores requirements in YAML files and works directly from the console. Since everything is version-controlled in Git, requirements can be reviewed during merge requests just like code.

![raiz](/images/posts/raiz-cli.png)

## Unit Testing

I’ve already written about [unit testing]({% post_url programming/2024-09-18-unit-tests %}) in other posts ( [here]({% post_url programming/2020-03-28-tdd-intro %}), [here]({% post_url programming/2020-04-25-tdd-with-docker %}), [here]({% post_url programming/2020-04-04-basic-tdd-ceedling %}) [here]({% post_url programming/2020-04-11-tdd-cmock %})  and [here]({% post_url programming/2020-04-18-tdd-library-dependency %})).  For my current projects on **Zephyr RTOS**, I use **[ZTest](https://docs.zephyrproject.org/latest/develop/test/ztest.html)** as the testing framework.

Unit tests run:
* Locally, every time I build.
* Automatically in CI/CD, triggered by every `git push`.

These tests are lightweight, so I run them as often as possible.

## Integration and System Tests

For this, I use a mix of **PyTest** and **ZTest**. Tests run in two stages:

1. **On Linux (PC or CI/CD VM)** – fast feedback without hardware.
2. **On real hardware (HIL: Hardware-in-the-Loop)** – the device under test connects to a programmable power supply, Raspberry pi and automation software. Test logs are collected via UART, parsed, and analyzed.

Because HIL testing consumes more resources, I only run it before merging a branch and before releases.

![hil-testing](/images/posts/hil-testing.png)

## Acceptance Testing

Acceptance testing is the most intensive step. I use a combination of [**Raiz**](https://github.com/daleonpz/raiz) and [**Robot Framework**](https://robotframework.org/):

1. **Traceability check (CI/CD):** Raiz ensures requirements and acceptance tests match.
2. **Automated tests (CI/CD):** Robot Framework coordinates and executes acceptance tests on the system.
3. **Customer testing:** the product goes to the customer for real-world use. This phase requires close observation and is the most time-consuming, but also the most valuable.

![v-model-testing](/images/posts/v-model-testing.png)

# Conclusions

Building embedded devices requires a well-defined process if you want quality results. It doesn’t have to be the **V-Model**, but it must ensure your final product works and delivers value to customers.

Testing is not always fun, but it’s essential. Setting up frameworks, scripts, devices, and CI/CD takes effort, but once everything is in place, the process runs smoothly and saves a lot of headaches.
