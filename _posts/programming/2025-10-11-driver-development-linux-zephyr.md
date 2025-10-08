---
layout: post
title: Driver development in the Linux kernel and Zephyr RTOS
category: programming
description: A look into how Linux and Zephyr RTOS use polymorphism and auto-registration for driver development.
tags: [programming, c, linux, zephyr, embedded, drivers, gpio]
---

In a previous post, I mentioned that I've been working a lot with the Linux and Zephyr RTOS code lately. Most of the time, I'm either tuning the device tree spec (DTS) or patching the kernel or writing drivers.  I learned a lot about how Linux organizes its code and uses object-oriented programming, or OOP. I'll go over the tricks and concepts that Linux uses for driver development.

For driver development, Linux uses something called "auto-registration" to combine it with polymorphism.  
**Note**: Linux and Zephyr RTOS have a lot in common when it comes to code, so I'm going to focus on Linux, but I'll show some code from Zephyr as an example.

* 
{:toc}

# Polymorphism 

Polymorphism lets you define a base class and extend it. In C, this is done with **structs + function pointers**.
Example: a base class `figure` has its “methods” grouped inside `figure_ops`. 

```c
/********************************************/
/*                 figure.h                 */
/********************************************/
#ifndef FIGURE_H
#define FIGURE_H

/*       Methods to be adapted or expanded        */
struct figure_ops {
    double (*get_area)(void *self);
    int    (*get_num_vertices)(void *self);
};

/*       Base class definition          */
struct figure {
    const struct figure_ops *ops;
};
```

Now we can create a derived class `circle` by assigning its own functions to the base ops.

```c
/********************************************/
/*                 circle.h                 */
/********************************************/
#include "figure.h"
#include "initcalls.h"
#include <math.h>
#include <stdio.h>

/*           Circle Class     */
struct circle {
    struct figure base;  /* Inclution of base class */
    double radius;
};

/*        Functions for Circle   */
static double circle_get_area(void *self) {
    struct circle *c = self;
    return M_PI * c->radius * c->radius;
}

static int circle_get_vertices(void *self) {
    return 0;
}

/*         Adapting figure methods       */
static const struct figure_ops circle_ops = {
    .get_area = circle_get_area,
    .get_num_vertices = circle_get_vertices,
};

```

## Things to notice
- The base class defines methods as function pointers inside a struct.
- The derived class assigns its own functions to those pointers.
- The first member of the derived class **MUST BE** the base class. Because in C, the address of a struct is the same as the address of its first member. This allows us to cast between base and derived types easily.

# Auto-registration
Linux drivers also register themselves automatically at boot. This avoids hardcoding and makes driver discovery consistent.
Auto-registration is the trick to load drivers in a standard way at boot using macros like `arch_initcall()`. These macros are like a **sign-in sheet at an event**.  Each guest writes their name when they arrive, so the host automatically knows who’s there.

So let's create our macros in `initcalls.h`

```c
/********************************************/
/*                 initcalls.h              */
/********************************************/
#ifndef INITCALLS_H
#define INITCALLS_H

typedef int (*initcall_t)(void);

/* We keep a list of registered initcalls */
#define MAX_INITCALLS 16
extern initcall_t __initcall_list[MAX_INITCALLS];
extern int __initcall_count;

/* Mimic Linux’s arch_initcall by placing fn into a list */
#define arch_initcall(fn) \
    static void __attribute__((constructor)) __register_##fn(void) { \
        if (__initcall_count < MAX_INITCALLS) { \
            __initcall_list[__initcall_count++] = fn; \
        } \
    }

#endif
```

## Things to notice
- Definition of a new data type `initcall_t`.  It's just a name for a function pointer that returns `int` and take no arguments. Nothing fancy. It just make the code easier to read.
- There’s a list (`__initcall_list`) that can hold up to 16 such functions.
- `arch_initcall(fn)` is a macro you use on a function. When the program starts, it **automatically adds that function to the list** (thanks to the `constructor` attribute).  This attribute tells the compiler: **“Run this function automatically before `main()` starts.”** Which is important because the drivers should be loaded before the applications start.
- If a function calls `arch_initcall()` the macro expands into something like

```c
static void __attribute__((constructor)) __register_my_init_function(void) {
    if (__initcall_count < MAX_INITCALLS) {
        __initcall_list[__initcall_count++] = my_init_function;
    }
}
```
- That function simply appends `my_init_function` into the global list `__initcall_list`.

# Auto-registration in action
Let's add auto-registration to our `circle.h` using `initcalls.h`, and expand `figure.h`
```c
/********************************************/
/*                 circle.h                 */
/********************************************/
#include "figure.h"
#include "initcalls.h"
#include <math.h>
#include <stdio.h>

/* Circle definition */
struct circle {
    struct figure base;
    double radius;
};

/* Methods definition */
static double circle_get_area(void *self) {
    struct circle *c = self;
    return M_PI * c->radius * c->radius;
}

static int circle_get_vertices(void *self) {
    return 0;
}

/*  Polymorphism  */
static const struct figure_ops circle_ops = {
    .get_area = circle_get_area,
    .get_num_vertices = circle_get_vertices,
};

/**********************************/
/*   Auto - Registration          */
/**********************************/
/* Static instance of a circle (like a statically defined driver) */
static struct circle global_circle = {
    .base.ops = &circle_ops,
    .radius = 5.0,
};

/* Init function that registers the circle with the system */
static int circle_init(void) {
    printf("circle_init(): registering circle...\n");
    register_figure((struct figure *)&global_circle);
    return 0;
}

/* Auto-register with "arch_initcall" */
arch_initcall(circle_init);


/********************************************/
/*                 figure.h                 */
/********************************************/
#ifndef FIGURE_H
#define FIGURE_H

struct figure_ops {
    double (*get_area)(void *self);
    int    (*get_num_vertices)(void *self);
};

struct figure {
    const struct figure_ops *ops;
};

/* A global registration function (like kernel’s driver model) */
void register_figure(struct figure *f);
void list_figures(void);

#endif

```

## Things to notice
* In `circle.h`
    - Initialization of the driver parameters in `global_circle`
    - The use of `circle_init(void)`, a callback, to register the figure with `register_figure`, which is declared in `figure.h`
    - Call `arch_initcall()` with `circle_init` as argument
- In `figure.h`  
    - Declaration of `register_figure()` and `list_figures()`. These functions are going to be overwritten elsewhere, in our case in `main.c` and it's the one that defines how registration actually is going to be performed, but the declaration of these functions in `figure.h` is important because the derived classes use `register_figure()` for auto-registration.

## Driver loader
The next step is to create a simple driver loader in `main.c` that simulates the kernel running all initcalls and then lists all registered figures.

```c
/********************************************/
/*                  main.c                  */
/********************************************/
#include <stdio.h>
#include "figure.h"
#include "initcalls.h"

/* Storage for initcalls */
initcall_t __initcall_list[MAX_INITCALLS];
int __initcall_count = 0;

/* Storage for registered figures */
#define MAX_FIGURES 8
static struct figure *figures[MAX_FIGURES];
static int figure_count = 0;

/*     Definition of auto-register functions  */
void register_figure(struct figure *f) {
    if (figure_count < MAX_FIGURES)
        figures[figure_count++] = f;
}

void list_figures(void) {
    for (int i = 0; i < figure_count; i++) {
        printf("Figure %d -> Area: %.2f, Vertices: %d\n",
               i,
               figures[i]->ops->get_area(figures[i]),
               figures[i]->ops->get_num_vertices(figures[i]));
    }
}

/*          main loop        */
int main() {
    /* Simulate kernel running all initcalls */
    for (int i = 0; i < __initcall_count; i++) {
        __initcall_list[i]();
    }

    /* After initcalls, list all registered figures */
    list_figures();

    return 0;
}
```
Now we can compile and run the code

```
$ gcc -o geom main.c circle.c -lm
./geom 
circle_init(): registering circle...
Figure 0 -> Area: 78.54, Vertices: 0
```

# Real Examples in Linux and Zephyr RTOS

Now it's time to see real-world examples. Let's recap what we have seen so far.  

- A base class defined in files like `driver.h` or `gpio.h`
- Set the polymorphism in the derived class in files  like `gpio-vf610.c` or `gpio_fxl6408.h
- Set the auto-registration using a system macro like `builtin_platform_driver` or `DEVICE_DEFINE`
### NXP Linux kernel 

Let's take a look at the [gpio folder](https://github.com/nxp-imx/linux-imx/tree/lf-6.12.y/drivers/gpio). There you'll find [`driver.h`](https://github.com/nxp-imx/linux-imx/blob/lf-6.12.y/include/linux/gpio/driver.h#L417 ) where the base class `gpio_chip` is defined

```c
/*****************************************/
/*              driver.h                 */
/*****************************************/

/*        Base Class  with methods    */
struct gpio_chip {
    const char      *label;
    struct gpio_device  *gpiodev;
    struct device       *parent;
    struct fwnode_handle    *fwnode;
    struct module       *owner;

    int         (*request)(struct gpio_chip *gc, unsigned int offset);
    void        (*free)(struct gpio_chip *gc, unsigned int offset);
    int         (*get_direction)(struct gpio_chip *gc, unsigned int offset);
    int         (*direction_input)(struct gpio_chip *gc, unsigned int offset);
    int         (*direction_output)(struct gpio_chip *gc, unsigned int offset, int value);
    ...
};
```

Now let's look at the [driver of vf610 device](https://github.com/nxp-imx/linux-imx/blob/lf-6.12.y/drivers/gpio/gpio-vf610.c#L376) . As expected, the polymorphism and auto-registration are defined in `gpio-vf610.c`. Polymorphism in the `vf610_gpio_probe()` function and auto-registration with `builtin_platform_driver(vf610_gpio_driver);`

```c
/*****************************************/
/*              gpio-vf610.c             */
/*****************************************/
static int vf610_gpio_probe(struct platform_device *pdev)
{
    /*    Polymorphism   */
    ...
    struct gpio_chip *gc;
    ...
    gc->request = gpiochip_generic_request;
    gc->free = gpiochip_generic_free;
    gc->direction_input = vf610_gpio_direction_input;
    gc->get = vf610_gpio_get;
    gc->direction_output = vf610_gpio_direction_output;
    gc->set = vf610_gpio_set;
    ...
    return devm_gpiochip_add_data(dev, &g->gc, g);
}

/*     Driver initialization  */    
static struct platform_driver vf610_gpio_driver = {
    .driver = 
    {
        .name   = "gpio-vf610",
        .of_match_table = vf610_gpio_dt_ids,
    },
    .probe  = vf610_gpio_probe, /* the function from above */
};

/*     Auto - registration      */
builtin_platform_driver(vf610_gpio_driver); 

/*********************************************/
/*              platform_driver.h            */
/*  Responsable to manage auto-registrations */ 
/*********************************************/
struct platform_driver {
    int (*probe)(struct platform_device *);

    union {
        void (*remove)(struct platform_device *);
        void (*remove_new)(struct platform_device *);
    };

    void (*shutdown)(struct platform_device *);
    int (*suspend)(struct platform_device *, pm_message_t state);
    int (*resume)(struct platform_device *);
    struct device_driver driver;
    const struct platform_device_id *id_table;
    bool prevent_deferred_probe;
    bool driver_managed_dma;
};
```

For more info you could check the code of the [platform driver](https://github.com/nxp-imx/linux-imx/blob/lf-6.12.y/include/linux/platform_device.h#L236) and the official documentation of the [linux driver-api](https://docs.kernel.org/driver-api/gpio/driver.html)

##  Zephyr RTOS 
The same pattern can be found in Zephyr RTOS.
First, we need a base class, which is defined in [`gpio.h`](https://github.com/zephyrproject-rtos/zephyr/blob/7837f5e5091f6cc740ee1a8b0d0c84af8975bfd7/include/zephyr/drivers/gpio.h#L809)

```c
/*****************************************/
/*                gpio.h                 */
/*****************************************/
__subsystem struct gpio_driver_api {
    int (*pin_configure)(const struct device *port, gpio_pin_t pin, gpio_flags_t flags);
    int (*pin_get_config)(const struct device *port, gpio_pin_t pin, gpio_flags_t *flags);
    int (*port_get_raw)(const struct device *port, gpio_port_value_t *value);
    int (*port_set_masked_raw)(const struct device *port, gpio_port_pins_t mask, gpio_port_value_t value);
    int (*port_set_bits_raw)(const struct device *port, gpio_port_pins_t pins);
    int (*port_clear_bits_raw)(const struct device *port, gpio_port_pins_t pins);
   ...
};
```

Now let's look at the [`gpio_fxl6408.c`](https://github.com/zephyrproject-rtos/zephyr/blob/main/drivers/gpio/gpio_fxl6408.c#L396) driver file. You'll see that Zephyr uses a different approach than NXP Linux. Zephyr uses macros such as `DEVICE_API` to define the polymorphism, and `DEVICE_DT_INST_DEFINE` and `DT_INST_FOREACH_STATUS_OKAY` for auto-registration.

```c
/*****************************************/
/*             gpio_fxl6408.h            */
/*****************************************/

/*       Polymorphism     */
static DEVICE_API(gpio, gpio_fxl_driver) = {
    .pin_configure = gpio_fxl6408_pin_config,
    .port_get_raw = gpio_fxl6408_port_get_raw,
    .port_set_masked_raw = gpio_fxl6408_port_set_masked_raw,
    .port_set_bits_raw = gpio_fxl6408_port_set_bits_raw,
    .port_clear_bits_raw = gpio_fxl6408_port_clear_bits_raw,
    .port_toggle_bits = gpio_fxl6408_port_toggle_bits,
};

/*      Auto - Registration    */
#define GPIO_FXL6408_DEVICE_INSTANCE(inst)                                     \
    static const struct gpio_fxl6408_config gpio_fxl6408_##inst##_cfg = {  \
        .common = {                                                    \
            .port_pin_mask = GPIO_PORT_PIN_MASK_FROM_DT_INST(inst),\
        },                                                             \
        .i2c = I2C_DT_SPEC_INST_GET(inst)                              \
    };                                                                     \
\
    static struct gpio_fxl6408_drv_data gpio_fxl6408_##inst##_drvdata = {  \
        .reg_cache = {                                                 \
            .input = 0x0,                                          \
            .output = 0x00,                                        \
            .dir = 0x0,                                            \
            .high_z = 0xFF,                                        \
            .pud_en = 0xFF,                                        \
            .pud_sel = 0x0                                         \
        }                                                              \
    };                                                                     \
\
    DEVICE_DT_INST_DEFINE(inst, gpio_fxl6408_init, NULL,                   \
        &gpio_fxl6408_##inst##_drvdata,                                \
        &gpio_fxl6408_##inst##_cfg, POST_KERNEL,                       \
        CONFIG_GPIO_FXL6408_INIT_PRIORITY,                             \
        &gpio_fxl_driver);


DT_INST_FOREACH_STATUS_OKAY(GPIO_FXL6408_DEVICE_INSTANCE)

```

You can find more information about the macros in the [`device.h`](https://github.com/zephyrproject-rtos/zephyr/blob/main/include/zephyr/device.h#L1340) file and the [official driver documentation](https://docs.zephyrproject.org/latest/kernel/drivers/index.html). But the following snippet provides a short description of some macros.

```c
/**
 * @brief Wrapper macro for declaring device API structs inside iterable sections.
 */
#define DEVICE_API(_class, _name) const STRUCT_SECTION_ITERABLE(Z_DEVICE_API_TYPE(_class), _name)

/**
 * @brief Create a device object and set it up for boot time initialization.
 *
 * This macro defines a @ref device that is automatically configured by the
 * kernel during system initialization. This macro should only be used when the
 * device is not being allocated from a devicetree node. If you are allocating a
 * device from a devicetree node, use DEVICE_DT_DEFINE() or
 * DEVICE_DT_INST_DEFINE() instead.
 */
#define DEVICE_DEFINE(dev_id, name, init_fn, pm, data, config, level, prio,    \
              api)                                                     \
    Z_DEVICE_STATE_DEFINE(dev_id);                                         \
    Z_DEVICE_DEFINE(DT_INVALID_NODE, dev_id, name, init_fn, pm, data,      \
            config, level, prio, api,                              \
            &Z_DEVICE_STATE_NAME(dev_id))

```

# Conclusions

Both **Linux** and **Zephyr RTOS** run on many different boards and chips, each with its own special hardware. Instead of rewriting the whole system for every new device, they use a **common interface + auto-registration pattern**.

- The **common interface** (like `struct pinmux_ops` in Linux or `DEVICE_API` in Zephyr) defines what functions a driver must provide (e.g., set a pin, send data, enable a sensor).
- The **auto-registration** (`arch_initcall` in Linux, `DEVICE_DEFINE` or `DEVICE_DT_INST_DEFINE` in Zephyr) makes sure each driver “plugs itself in” during startup without the kernel/RTOS needing to know about it in advance.

This way:

- The core system only talks to the **interface**, not the specific driver.
- Developers can add new hardware support just by writing a driver and registering it.
- The kernel/RTOS discovers all drivers automatically at boot.

 In simple terms: _it’s like having a universal socket — the system defines the shape, and each driver brings its own plug that fits in automatically._
