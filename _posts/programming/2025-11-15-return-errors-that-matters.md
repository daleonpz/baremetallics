---
layout: post
title: Returning Error Information That Matters
category: programming
description: How to return error information that the caller can actually do something about.
tags: [c, error-handling, design, embedded]
---

Error handling is tricky. On one side, the caller needs to react to errors. On the other side, the more error details you pass back, the more complex the caller code becomes. Complex code means harder to read, harder to maintain, and more bugs. And let's be real, does the caller always really need to know why a API or function failed or does the caller sometimes only need to know if a function failed?

So the rule of thumb is simple: **Only return error information if the caller can actually do something about it.**

If the caller can’t react to the error, don’t bother them with it.

* 
{:toc}

# Example: Sensor Reading

Imagine you have a function to read a temperature sensor.

## Return every possible error

```c
typedef enum {
    OK,
    SENSOR_TIMEOUT,
    SENSOR_NOT_FOUND,
    SENSOR_CRC_ERROR,
    SENSOR_I2C_ERROR,
    SENSOR_UNEXPECTED_DATA
} SensorError;

SensorError read_temperature(int *value);
```

This dumps every possible internal failure on the caller. But what is the caller supposed to do with all that?

* Retry?
* Log?
* Ignore?

Most of the time, the caller only cares about:

* Can I use this value?
* Should I abort?

## Return only what matters

```c
typedef enum {
    OK,
    SENSOR_ERROR,
    FATAL_ERROR
} SensorStatus;

SensorStatus read_temperature(int *value);
```

Implementation:

```c
SensorStatus read_temperature(int *value) {
    if (minorIssue()) {
        return SENSOR_ERROR;   /* caller may retry */
    } else if (majorIssue()) {
        return FATAL_ERROR;    /* caller must abort */
    }
    *value = read_sensor_register();
    return OK;
}
```

Caller’s code is now much simpler:

```c
int temp;
SensorStatus status = read_temperature(&temp);

if (status == FATAL_ERROR) {
    system_abort();  /* stop everything */
} else if (status == SENSOR_ERROR) {
    retry_read();    /* handle gracefully */
} else {
    use_temperature(temp);
}
```

All the low-level errors (CRC, I2C, timeout, etc.) are collapsed into **relevant categories**. If you need debugging info, just log it inside `read_temperature()` instead of forcing the caller to deal with it.

# Conclusions 

* Caller only handles errors that matter to them.
* Less code, fewer bugs, easier to test.
* API is cleaner, and adding new internal errors doesn’t break callers.
* The risk: maybe you forget an error the caller *should* care about. But you can always extend your status codes later.

The key idea: **Only return error information if the caller can actually do something about it.**. Ask yourself: **What should the caller do when this error happens?** If the answer is "nothing" or "log it", then don't return that error.

