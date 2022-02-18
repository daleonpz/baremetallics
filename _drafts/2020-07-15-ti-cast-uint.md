---
title: Always cast. A Left-shift story
---

I'm working on a small project with the Texas Instruments microcontroller MSP430FR2433. 

And I wanted to convert an array with hex values to its proper integer value.

For example, if we have a two-byte array like this:

```sh
| 0x01 | 0x00 |
```

That would be 256 ("0x0100"). 

All the integer values are `unsigned`. In my case `uint32_t`.

However, I found a bug when the array is `| 0x89 | 0x35 |`.  I got 4294936885 instead of 35125. 

So.. what's the difference between `| 0x89 | 0x35 |` and `| 0x11 | 0xA5 |` ?  BTW, `| 0x11 | 0xA5 |` works.

To find it out ... keep reading. 

# Setup of the function

- Inputs: 
    - An array with "hex" values. `uint8_t *hex_array`.
    - Array length. `uint8_t len`.
- Output:
    - Unsigned integer of 32 bits. `uint32_t`

The function goes as follows:

```c
uint32_t usart_hex_array_to_u32(uint8_t *hex_array, uint8_t len)
{
    uint32_t sum = 0;
    uint8_t displacement = 0;;

    for( uint8_t i=0; i<len; i++)
    {
        uart_debug("i:%i %i",i, hex_array[i]);
        displacement = 8*(len - 1 - i);
        sum += ( hex_array[i] << displacement );
    }

    return sum;
}
```

# Assumptions

In this line `sum += ( hex_array[i] << displacement );`

Before actually the Left-shift operator (<<) take part. There is a implicit casting before. 

So. The compiler casts `hex_array[i]` from `uint8_t` to `uint32_t`.  

And also the compiler casts `displacement` to `uint32_t`.

This is because the compiler does an **Implicit Conversion** or **Data Promotion** when in an expression more than one data type is present. 

## Implicit Conversion or Data Promotion
The goal of **Implicit Conversion** is to avoid lose of data. 

The gcc compiler has the following hierarchy or ranking.

```c
/* From         ->  To*/
bool            -> char 
char            -> short int 
short int       -> int 
int             -> unsigned int 
unsigned int    -> long 
long            -> unsigned 
unsigned        -> long long 
long long       -> float 
float           -> double 
double          -> long double
```

In this post [Details of C that every noob forgets ]({% post_url programming/2017-04-10-C-noob-details %}). You can read more about promotions. 

# Debugging

At the beginning I said I found a bug when the array is `| 0x89 | 0x35 |`. 
I got 4294936885 instead of 35125. 

The same applies when the array is `| 0xD1 | 0xC3 |`. I got 4294955459 instead of 53699.

The error lies at the implicit data conversion. 

Because when I cast `hex_array[i]` to `uint32_t`. It works.

```c
uint32_t usart_hex_array_to_u32(uint8_t *hex_array, uint8_t len)
{
    uint32_t sum = 0;
    uint8_t displacement = 0;;

    for( uint8_t i=0; i<len; i++)
    {
        uart_debug("i:%i %i",i, hex_array[i]);
        displacement = 8*(len - 1 - i);
        sum += ( ( (uint32_t)hex_array[i]) << displacement );
    }

    return sum;
}
```

# The Experiment
Array input is `| 0x89 | 0x35 |`.
Output is 4294936885. 

I convert 4294936885 to hex. I got `0xFFFF8935`. 

**Aha!....**

In theory I got the "right value". Because `0x8935` is 35125. The expected value.

But the question is...

Why... `0xFFFF8935`?

if we follow the **Data Promotions**.  `char -> short int -> int -> unsigned int`

In the embedded world it would be `uint8_t -> int16_t -> uint16_t -> int32_t -> uint32_t`

Look! From the data promotion always goes from **unsigned type with less numbers of bits** to **signed type with more bits**. Example `uint8_t -> int16_t`.

So..

The compiler casts `0x89`(`0b10001001`) to `int16_t`. 

**NOTE**: Compiler uses 2'complement to represent **signed** integers. 
In 2'complement `0x89` is -119. For the complier -119 should stay as -199 after promotion.
So -199 as `int16_t` is `0xFF89`.

**NOTE**: Compiler **DOESN'T**  use 2'complier to represent **unsigned** integers. 
So, `0xFF89` as `uint16_t` is still `0xFF89`.


At the end -199 as `int32_t` is `0xFFFFFF89`.

when the code executes `hex_array[0] << 8`.. it does `0xFFFFFF89 << 8`. The result is then `0xFFFF8900`.

That's why I got 4294936885 (`0xFFFF8935`).


# Why casting works
when I do `(uint32_t) hex_array[i]`. what I'm saying is `hex_array[i]` is direct a 32-bit value.

For example, if `hex_array[i]` is `0x89`. then `(uint32_t)hex_array[i]` would be `0x00000089`. 

# Conclusion
Practice defense C programming. With this bug you learned to **avoid implicit casting**.

Always cast! 


