---
layout: post
title: Object Oriented Programming in C - Linux Kernel Style
category: programming
description: Understanding how to implement Object Oriented Programming concepts in C, inspired by Linux kernel practices.
tags: [programming, c, linux, oop]
---

I'm working lately a lot with embedded linux, and most of the time is either tuning the device tree spec (dts) or patching the kernel.   So while debugging and patching the kernel I learned a lot about how linux organize the code and its use of the Object Oriented Programming or OOP.  I 'll share what I learned.

* 
{:toc}

# Object oriented

Object oriented programming have some advantages like encapsulation and modularity, but there is no native support in C. But  that doesn't mean it's not possible to use object oriented programming in C.  

The following example is a simple example of OOP.  A header file `foo.h`, a source code or implementation file `foo.c` and the application `main.c`.

```c
/********************************************/
/*                    foo.h                 */
/********************************************/
#pragma once

/*   Foward declarations  */
typedef struct _public public_t;

/*   Data Structures  */
struct _public {
    int p;
};

/*  Public Interface  */
public_t * init();
void destroy(public_t *self);
void set_c(public_t * self, int value);

/********************************************/
/*                    foo.c                 */
/********************************************/
#include <stdlib.h>
#include "foo.h"

/*   Public Implementation   */
public_t * init(){
    public_t* self = (public_t*)malloc(sizeof(public_t));
    self->p = 0;
    return self;
}

void set_c(public_t * self, int value){
    self->p = value;
}

void destroy(public_t* self){
    free(self);
}

/********************************************/
/*                   main.c                 */
/********************************************/
#include "foo.h"
 int main (){ 
	 public_t * pub; 
	 pub = init(); 
	 set_c(pub, 20); 
	 destroy(pub); 
}
```

and if you don't want to use pointers notation directly `public_t *`

```c 
typedef struct _public * PUBLIC_HANDLE;
PUBLIC_HANDLE init();
void destroy(PUBLIC_HANDLE h);
void set_c(PUBLIC_HANDLE h, int value);

```

## Things to notice
- The use of `struct` to define a "class"
- The object instantation is usually with pointers, either as dynamic (malloc or calloc, heap) or static memory (stack) .

```c
/* example 1 */
void test() {     
	Device dev2 = { .id = 2, .print = Device_print };     
	dev2.print(&dev2);  /* Pass as pointer, no malloc */
}

/* example  2 */
#define MAX_DEVICES 10
static Device devices[MAX_DEVICES];

Device* Device_create(int id) {
    static int next = 0;
    if (next < MAX_DEVICES) {
        devices[next].id = id;
        devices[next].print = Device_print;
        return &devices[next++];
    }
    return NULL; /* pool exhausted */
}
```

- Each methods takes the object instance defined by `XX_HANDLER` or a `pointer to struct` as first argument. 

For example in c++ you would do something like

```cpp
my_obj.my_method(arg1, arg2,...);
```

but in c it would be
```c
my_method( my_obj, arg1, arg2, ...);
```

## FAQ
- Is that the only way to define classes and methods in C?  Nope, I'll present more complicated examples, but the main idea stays the same.  Use `struct` to emulate typical cpp or python classes, and pass the object instance as first argument to the  method.
- Is it possible to have private class methods and members in C? Yes, you'll see in the following sections how. 

# Public and private methods and interfaces

Another characteristic in OOP in cpp and python is the possibility to use private members and methods.  Lucky us, that's possible in C. 
Look the following example where I expand `foo.h` and `foo.c`

```c
/**********************************/
/*              foo.h             */
/**********************************/
#pragma once

/*  Forward Declarations  */
typedef struct _internal internal_t;
typedef struct _public public_t;

/*  Data Structures  */
struct _public {
    int p;
    internal_t * priv;
};

/*  Public API */
public_t * init();
void destroy(public_t *self);
void set_c(public_t * self, int value);

/**********************************/
/*              foo.c             */
/**********************************/
#include <stdlib.h>
#include "foo.h"

/*  Private Members */
struct _internal {
    int h;
    char c;
};

/*  Implementation of Public Methods */
public_t * init(){
    public_t* self = (public_t*)malloc(sizeof(public_t));
    self->priv = (struct _internal *)malloc(sizeof (struct _internal));
    self->p = 0;
    self->priv->h = 0;
    self->priv->c = 0;
    return self;
}

void set_c(public_t * self, int value){
    self->priv->h = value;
}

void destroy(public_t* self){
    free(self->priv);
    free(self);
}
```

## Things to notice
1. `struct _public` has a member called `internal_t * priv` , which is a pointer to a struct defined in the implementation file `foo.c` 
2. The implementation of the public class methods in `foo.c` manipulates the internal variables in `struct _internal`. 

```c
/* main.c */
#include "foo.h"

int main (){
    public_t * pub;

    pub = init();
    pub->p = 10;

   //  pub->priv->h = 10;  /* Incomplete definition of type 'internal_t' (aka 'struct _internal') */

    set_c(pub, 20);
    destroy(pub);
}
```

if we uncomment the line `pub->priv->h = 10;` we'll get `Incomplete definition of type 'internal_t' (aka 'struct _internal')`  which is expected since `priv` is a pointer to `internal_t *`  but it's internal details are not public.

## FAQ
Here’s the short explanation of why it works

- **Header files (`*.h`)**: Define the **interface / API** — what the outside world is allowed to see and use.
- **Source files (`*.c`)**: Contain the **implementation details** — the actual code that fulfills the contract. They can hide private data structures, helper functions, and internal logic.
- **Why this split matters**:
    - **Encapsulation** → users only see what they need, not internals.
    - **Flexibility** → you can change internals without breaking user code (as long as the interface remains the same).
    - **Faster builds** → changes in `.c` don’t force recompiling all code that includes the `.h`.

##  Headers and Source Files: The Copy-Paste Mental Mode

 Whenever you import a header file with `#include`. For example, if you add `#include <foo.h>` in `main.c`, the preprocessor literally **copy-pastes the content of `foo.h`** into your `.c` file before the compiler ever sees it.

Given `foo.h` and `foo.c`
```c
/*********************************/
/*             foo.h             */
/*********************************/
#pragma once

typedef struct _internal internal_t;
typedef struct _public public_t;

struct _public {
    int p;
    internal_t * priv;
};

public_t * init();
void destroy(public_t *self);
void set_c(public_t * self, int value);

/*********************************/
/*             foo.c             */
/*********************************/
#include <stdlib.h>
#include "foo.h"

struct _internal {
    int h;
    char c;
};
public_t * init(){
    public_t* self = (public_t*)malloc(sizeof(public_t));
    self->priv = (struct _internal *)malloc(sizeof (struct _internal));
    self->p = 0;
    self->priv->h = 0;
    self->priv->c = 0;
    return self;
}

void set_c(public_t * self, int value){
    self->priv->h = value;
}

void destroy(public_t* self){
    free(self->priv);
    free(self);
}
```

and `main.c`

```c
/* main.c */
#include "foo.h"

int main (){ 
    public_t * pub; 
    pub = init(); 
    set_c(pub, 20); 
    destroy(pub); 
}
```

The preprocessor would do the following for `main.c`

```c
/* Preprocessed main.c — what the compiler sees */

/* content of foo.h copied here */
typedef struct _internal internal_t;
typedef struct _public public_t;

struct _public {
    int p;
    internal_t * priv;  
};

public_t * init();
void destroy(public_t *self);
void set_c(public_t * self, int value);

int main (){
    public_t * pub; 
    pub = init(); 
    set_c(pub, 20); 
    destroy(pub); 
}
```

As you can see, the code written in `foo.c` is not visible in `main.c`

```c
/* From foo.c *
struct _internal {
    int h;
    char c;
};
```

That's why the compilers throws and error with

```c
// main.c
pub->priv->h = 10;  // Incomplete definition of type 'internal_t' (aka 'struct _internal')
```

If you want to understand what happens after compilation, a while ago I wrote a post about [linking basics]({% post_url programming/2017-02-11-linking-basics %}) where I talked about ELF files and their structure. Also there is a post about [linking scripts]({% post_url programming/2019-03-11-Linker-scripts %}) where I show how after compiling the code the linker combines all object files into a single executable. 

## Example in Linux code

Here's how the modem manager uses private and public code to define 3GPP profiles

```c
/*********************************************/
/* ModemManager/libmm-glib/mm-3gpp-profile.h */
/*********************************************/

/*     Forward declarations     */
typedef struct _MM3gppProfile        MM3gppProfile;
typedef struct _MM3gppProfileClass   MM3gppProfileClass;
typedef struct _MM3gppProfilePrivate MM3gppProfilePrivate;

/*     Public object     */
struct _MM3gppProfile {
    GObject parent;
    /* Pointer to private data */
    MM3gppProfilePrivate *priv;
};

struct _MM3gppProfileClass {
    GObjectClass parent;
};

/*     Public API     */
/* -- Setters -- */
void mm_3gpp_profile_set_ip_type  (MM3gppProfile *self,
                                   MMBearerIpFamily ip_type);

void mm_3gpp_profile_set_apn_type (MM3gppProfile *self,
                                   MMBearerApnType apn_type);

/* -- Getters -- */
MMBearerIpFamily mm_3gpp_profile_get_ip_type  (MM3gppProfile *self);
MMBearerApnType  mm_3gpp_profile_get_apn_type (MM3gppProfile *self);

/*********************************************/
/* ModemManager/libmm-glib/mm-3gpp-profile.c */
/*********************************************/

/*   Private struct   */
struct _MM3gppProfilePrivate {
    gint                          profile_id;
    gchar                        *profile_name;
    gchar                        *apn;
    MMBearerIpFamily              ip_type;
    MMBearerApnType               apn_type;
    MMBearerAccessTypePreference  access_type_preference;
    gboolean                      enabled;
    gboolean                      enabled_set;
    MMBearerRoamingAllowance      roaming_allowance;
    MMBearerProfileSource         profile_source;

    /* Optional authentication settings */
    MMBearerAllowedAuth  allowed_auth;
    gchar               *user;
    gchar               *password;
};

/*     Getter implementation     */
MMBearerIpFamily
mm_3gpp_profile_get_ip_type (MM3gppProfile *self)
{
    g_return_val_if_fail (MM_IS_3GPP_PROFILE (self), MM_BEARER_IP_FAMILY_NONE);
    return self->priv->ip_type;
}

/* (other setters/getters follow the same pattern) */

/*********************************************/
/* ModemManager/src/mm-modem-helpers.c */
/*********************************************/
/*     Example usage of public API     */
gint
mm_3gpp_profile_list_find_best (GList                  *profile_list,
                                MM3gppProfile          *requested,
                                GEqualFunc              cmp_apn,
                                MM3gppProfileCmpFlags   cmp_flags,
                                gint                    min_profile_id,
                                gint                    max_profile_id,
                                gpointer                log_object,
                                MM3gppProfile         **out_reused,
                                gboolean               *out_overwritten)
{
    GList            *l;
    MMBearerIpFamily  requested_ip_type;
    gint              prev_profile_id = 0;
    gint              unused_profile_id = 0;
    gint              max_found_profile_id = 0;
    gint              max_allowed_profile_id = 0;
    gint              blank_profile_id = 0;

    g_assert (out_reused);
    g_assert (out_overwritten);

    /* Access via public getter, not directly through priv */
    requested_ip_type = mm_3gpp_profile_get_ip_type (requested);
}
```

Summary of the code:

- `mm-3gpp-profile.h` = API (public setters/getters)  
- `mm-3gpp-profile.c` = implementation (private struct + real logic)
- `mm-modem-helpers.c` =  uses the public interfaces defined in `mm-3gpp-profile.h`


# Conclusion
C doesn’t give you classes, inheritance, or access modifiers out of the box, but it's possible to implement OOP concepts. I wanted to show you how linux kernel and modem manager do it, so you could see real world examples.

I encourage you to explore a bit the [linux kernel code](https://github.com/torvalds/linux) or [zephyr RTOS code](https://github.com/torvalds/linux). You may find interesting patterns and tricks.


# References
- [Fluent C by Christopher Preschern](https://www.oreilly.com/library/view/fluent-c/9781492097273/ )
- [Mastering Algorithms with C by Kyle Loudon]( https://www.oreilly.com/library/view/mastering-algorithms-with/1565924533/)
- [Linux Modem Manager](https://github.com/linux-mobile-broadband/ModemManager/)


