well designde interfaces plug holes of ill-defined or undefined operations, define what is undefined, and make explicit decisions about behaviors thta the language specifies as undefined.


- 4.1.5 Noncompliant Code Example
The order of evaluation for function arguments is unspecified. This noncompliant code example exhibits unspecified behavior but not undefined behavior:

```c
extern void c(int i, int j);
int glob;
int a(void) {
return glob + 10;
}
int b(void) {
glob = 42;
return glob;
}
void func(void) {
c(a(), b());
}
```

compliant solution
```c
extern void c(int i, int j);
int glob;
int a(void) {
return glob + 10;
}
int b(void) {
glob = 42;
return glob;
}
void func(void) {
int a_val, b_val;
a_val = a();
b_val = b();
c(a_val, b_val);
}
```

- EXP33-C. Do not read uninitialized memory
```c
int is_negative(int number) {
int sign;
set_flag(number, &sign);
return sign < 0;
}

// Compliant Solution
int is_negative(int number) {
int sign = 0; /* Initialize for defense-in-depth */
set_flag(number, &sign);
return sign < 0;
}
```

In this noncompliant code example described in “More Randomness or Less” [Wang 2012](https://kqueue.org/blog/2012/06/25/more-randomness-or-less/), the
process ID, time of day, and uninitialized memory junk is used to seed a random number genera-
tor. This behavior is characteristic of some distributions derived from Debian Linux that use unin-
itialized memory as a source of entropy because the value stored in junk is indeterminate. How-
ever, because accessing an indeterminate value is undefined behavior, compilers may optimize
out the uninitialized variable access completely, leaving only the time and process ID and result-
ing in a loss of desired entropy.
```c
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
void func(void) {
struct timeval tv;
unsigned long junk;
gettimeofday(&tv, NULL);
srandom((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ junk);
}

// Compliant Solution, This compliant solution seeds the random number generator by using the CPU clock and the real- time clock instead of reading uninitialized memory. TRNG is even better.
void func(void) {
double cpu_time;
struct timeval tv;
cpu_time = ((double) clock()) / CLOCKS_PER_SEC;
gettimeofday(&tv, NULL);
srandom((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ cpu_time);
}
```

EXP34-C. Do not dereference null pointers
```c
#include <string.h>
#include <stdlib.h>
void f(const char *input_str) {
size_t size = strlen(input_str) + 1;
char *c_str = (char *)malloc(size);
memcpy(c_str, input_str, size);
/* ... */
free(c_str);
c_str = NULL;
/* ... */
}
```

- Compliant Solution
```c
#include <string.h>
#include <stdlib.h>
void f(const char *input_str) {
size_t size;
char *c_str;
if (NULL == input_str) {
/* Handle error */
}
size = strlen(input_str) + 1;
c_str = (char *)malloc(size);
if (NULL == c_str) {
/* Handle error */
}
memcpy(c_str, input_str, size);
/* ... */
free(c_str);
c_str = NULL; // <- Tip: set to NULL after freeing
/* ... */
}
```

Setting variable to NULL after free: 
Setting unused pointers to NULL is a defensive style, protecting against dangling pointer bugs. If a dangling pointer is accessed after it is freed, you may read or overwrite random memory. If a null pointer is accessed, you get an immediate crash on most systems, telling you right away what the error is.

- use bit fields to save memory, but be aware of the padding bits

```c
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t region_num : 4;  // Only 4 bits (0-15)
    bool enabled : 1;        // Only 1 bit (0 or 1)
    uint8_t user_write : 1;  // Only 1 bit (0 or 1)
} SimpleRegion;

int main() {
    SimpleRegion region = {0};

    // Assigning a value that is too large for user_write (1-bit field)
    region.user_write = 0xF0;  // 0b11110000

    // Assigning a value that is too large for region_num (4-bit field)
    region.region_num = 20;  // 0b10100 (out of range for 4 bits)

    // Output results
    printf("user_write = %u\n", region.user_write);   // Expected: 0 (truncated)
    printf("region_num = %u\n", region.region_num);   // Expected: 4 (truncated)

    return 0;
}
```
```sh
main.c: In function ‘main’:
main.c:15:25: warning: unsigned conversion from ‘int’ to ‘unsigned char:1’ changes value from ‘240’ to ‘0’ [-Woverflow]
   15 |     region.user_write = 0xF0;  // 0b11110000
      |                         ^~~~
main.c:18:25: warning: unsigned conversion from ‘int’ to ‘unsigned char:4’ changes value from ‘20’ to ‘4’ [-Woverflow]
   18 |     region.region_num = 20;  // 0b10100 (out of range for 4 bits)
      |                         ^~
user_write = 0
region_num = 4
```

- Problem with padding bits
```c
struct test {
unsigned a : 1;
unsigned : 0;
unsigned b : 4;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = { .a = 1, .b = 10 };
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```
In the above example, the unnamed bit-field `:0` causes no further bit-fields to be packed into the same storage unit. This can lead to padding bits being used within the structure due to the bit-field member lengths not adding up to the number of bits in an unsigned object. 

```
| a (1-bit) | <padding> | (next int starts) | b (4-bits) | <padding> |
```

Non-portable but compliant solution, This solution is not portable, however, because it depends on the implementation and target memory architecture
```c
#include <assert.h>
#include <limits.h>
#include <stddef.h>
struct test {
unsigned a : 1;
unsigned padding1 : sizeof(unsigned) * CHAR_BIT - 1;
unsigned b : 4;
unsigned padding2 : sizeof(unsigned) * CHAR_BIT - 4;
};
/* Ensure that we have added the correct number of padding bits. */
static_assert(sizeof(struct test) == sizeof(unsigned) * 2,
"Incorrect number of padding bits for type: un-
signed");
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = { .a = 1, .padding1 = 0, .b = 10, .padding2 = 0
};
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```

- Avoid side effects in arguments to unsafe macros
```c
#define ABS(x) (((x) < 0) ? -(x) : (x))
void func(int n) {
/* Validate that n is within the desired range */
int m = ABS(++n);
/* ... */
}

// expanded to
m = (((++n) < 0) ? -(++n) : (++n));
```

compliant solution 1:
```c
#define ABS(x) (((x) < 0) ? -(x) : (x)) /* UNSAFE */
void func(int n) {
/* Validate that n is within the desired range */
++n;
int m = ABS(n);
/* ... */
}
```

compliant solution 2:
```c
#include <complex.h>
#include <math.h>
static inline int iabs(int x) {
return (((x) < 0) ? -(x) : (x));
}
void func(int n) {
/* Validate that n is within the desired range */
int m = iabs(++n);
/* ... */
}
```

compliant solution 3:
```c
#include <complex.h>
#include <math.h>
static inline long llabs(long long v) {
return v < 0 ? -v : v;
}
static inline long labs(long v) {
return v < 0 ? -v : v;
}
static inline int iabs(int v) {
return v < 0 ? -v : v;
}
static inline int sabs(short v) {
return v < 0 ? -v : v;
}
static inline int scabs(signed char v) {
return v < 0 ? -v : v;
}
#define ABS(v) _Generic(v, signed char : scabs, \
short : sabs, \
int : iabs, \
long : labs, \
long : llabs, \
float : fabsf, \
double : fabs, \
long double : fabsl, \
double complex : cabs, \
float complex : cabsf, \
long double complex : cabsl)(v)
void func(int n) {
/* Validate that n is within the desired range */
int m = ABS(++n);
/* ... */
}
```

Generic selections were introduced in C11 and are not available in C99 and earlier editions of the C Standard.
more about that [here](https://en.cppreference.com/w/c/language/generic)

- struct

```c
typedef struct point_{ // struct point_ is the tag name
    int x;
    int y;
} point; // point is the type name
```

- DCL39-C. Avoid information leakage when passing a structure across a trust boundary
```c
#include <stddef.h>
struct test {
int a;
char b;
int c;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = {.a = 1, .b = 2, .c = 3};
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```

```c
#include <string.h>
struct test {
int a;
char b;
int c;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg;
/* Set all bytes (including padding bytes) to zero */
memset(&arg, 0, sizeof(arg));
arg.a = 1;
arg.b = 2;
arg.c = 3;
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```

Compliant Solution `_attribute__((__packed__))`
```
#include <stddef.h>
struct test {
int a;
char b;
int c;
} __attribute__((__packed__));
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = {.a = 1, .b = 2, .c = 3};
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```

Compliant solution
```c
#include <stddef.h>
#include <string.h>
struct test {
int a;
char b;
int c;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = {.a = 1, .b = 2, .c = 3};
/* May be larger than strictly needed */
unsigned char buf[sizeof(arg)];
size_t offset = 0;
memcpy(buf + offset, &arg.a, sizeof(arg.a));
offset += sizeof(arg.a);
memcpy(buf + offset, &arg.b, sizeof(arg.b));
offset += sizeof(arg.b);
memcpy(buf + offset, &arg.c, sizeof(arg.c));
offset += sizeof(arg.c);
copy_to_user(usr_buf, buf, offset /* size of info copied */);
}
```

- Noncompilant
This noncompliant code example also runs in kernel space and copies data from struct test to user space. However, padding bits will be used within the structure due to the bit-field member lengths not adding up to the number of bits in an unsigned object. 
Further, there is an unnamed bit-field that causes no further bit-fields to be packed into the same storage unit. These padding bits may contain sensitive information, which may then be leaked when the data is copied to user space. 
For instance, the uninitialized bits may contain a sensitive kernel space pointer value that can be trivially reconstructed by an attacker in user space.
```c
#include <stddef.h>
struct test {
unsigned a : 1;
unsigned : 0;
unsigned b : 4;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = { .a = 1, .b = 10 };
copy_to_user(usr_buf, &arg, sizeof(arg));
}#include <stddef.h>
struct test {
unsigned a : 1;
unsigned : 0;
unsigned b : 4;
};
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = { .a = 1, .b = 10 };
copy_to_user(usr_buf, &arg, sizeof(arg));
}   
```

- Compliant but not portable. From this situation, it can be seen that special care must be taken because no solution to the bit- field padding issue will be 100% portable.
```c

#include <assert.h>
#include <limits.h>
#include <stddef.h>
struct test {
unsigned a : 1;
unsigned padding1 : sizeof(unsigned) * CHAR_BIT - 1;
unsigned b : 4;
unsigned padding2 : sizeof(unsigned) * CHAR_BIT - 4;
};
/* Ensure that we have added the correct number of padding bits. */
static_assert(sizeof(struct test) == sizeof(unsigned) * 2,
"Incorrect number of padding bits for type: un-
signed");
/* Safely copy bytes to user space */
extern int copy_to_user(void *dest, void *src, size_t size);
void do_stuff(void *usr_buf) {
struct test arg = { .a = 1, .padding1 = 0, .b = 10, .padding2 = 0
};
copy_to_user(usr_buf, &arg, sizeof(arg));
}
```

- Pointers

For portability, we should never assume that pointers has a specific size . For example, the size of a pointer is 4 bytes on a 32-bit system and 8 bytes on a 64-bit system. 
Also, it's important to remembedr that when we declare a poniter, space is allocated for the pointer itself, not the object it points to. 

```c
int *p;
```
In the above example, `p` is a pointer to an integer, and it takes up space in memory. The size of `p` is 4 bytes on a 32-bit system and 8 bytes on a 64-bit system.

when casting pointers, one issue we need to be aware of is data alignment in memory. For example, on a 32-bit system, an integer is 4 bytes long, and it is aligned on a 4-byte boundary. If we cast a pointer to an integer, the pointer must be aligned on a 4-byte boundary. If it is not, the program may crash or behave unexpectedly.

Do not cast pointers into more strictly aligned pointer types

```c
#include <assert.h>
void func(void) {
char c = 'x';
int *ip = (int *)&c; /* This can lose information */
char *cp = (char *)ip;
/* Will fail on some conforming implementations */
assert(cp == &c);
}
```

a compliant solution
```c
#include <assert.h>
void func(void) {
char c = 'x';
int i = c;
int *ip = &i;
assert(ip == &i);
}
```

exception: Some hardware architectures have relaxed requirements with regard to pointer
alignment. Using a pointer that is not properly aligned is correctly handled by the architecture, alt-
hough there might be a performance penalty, which is a risk for portability. 

 If a pointer is known to be correctly aligned to the target type, then a cast to that
type is permitted. There are several cases where a pointer is known to be correctly aligned to the
target type. The pointer could point to an object declared with a suitable alignment specifie
```c
#include <stdalign.h>
#include <assert.h>
void func(void) {
/* Align c to the alignment of an int */
alignas(int) char c = 'x';
int *ip = (int *)&c;
char *cp = (char *)ip;
/* Both cp and &c point to equally aligned objects */
assert(cp == &c);
}
```






Use `assert` to check for conditions that should never happen. check old post, 
offensive vs defensive programming.
```c
#include <assert.h>

int my_func(char *str) {
    assert(str != NULL);
    // do something

    switch (str[0]) {
        case 'a':
            // do something
            break;
        case 'b':
            // do something
            break;
        default:
            assert(0); // should never happen
    }
}
```

You could disable `assert` in production code by defining `NDEBUG` during compilation. because `assert(expr)` is an expression, so most versions of `assert.h` are logically equivalent to:
```c
#ifdef NDEBUG
  #define assert(expr) ((void)0)
#else
  extern void assert(int e);
  #define assert(e) ((void)((e) || \
    (fprintf(stderr, "%s:%d: Assertion failed: %s\n", \
    __FILE__, __LINE__, #e), abort(), 0)))A
#endif
```


2.2.6 Noncompliant Code Example (assert())
```c
#include <assert.h>
#include <stddef.h>

void process(size_t index) {
assert(index++ > 0); /* Side effect */
/* ... */
}
```
As mentioned before if NDEBUG is defined, the assert() macro is removed from the code, and the side effect, index increment `index++`, is also removed. This can lead to different behavior in the production code than in the test code 

2.2.7 Compliant Solution (assert())
```c
#include <assert.h>
#include <stddef.h>

void process(size_t index) {
assert(index > 0); /* No side effect */
index++;
/* ... */
}
```

# References
- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [Mastering Algorithms with C by Kyle Loudon](https://www.oreilly.com/library/view/mastering-algorithms-with/1565924533/)
- [Pebble source code](https://github.com/google/pebble)
