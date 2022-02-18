---
layout: post
title: Top 5 Books About Best Practices in Embedded Systems 
category: programming
description: top 5 books about best practices embedded systems C 
---

Best practices are methods that have been demonstrated to be effective and can be replicable in other contexts. 

![top 5 books best practices C embedded](/images/posts/5-books-best-practices.jpg)

* 
{:toc}

# Forgotten Gem: The Pragmatic Programmer
["The Pragmatic Programmer - From Journeyman to Master"](https://amzn.to/3acWAmz)  by Andrew Hunt teaches you the best practices in software development. Andrew gives you a list of tips that you can implement right away to improve the speed, quality, and performance of your work.

Some tips are:
- Think! about your work: never run on auto-pilot. Think about what you are doing
- Critically analyze what you read and hear
- DRY - Don't Repeat Yourself
- Estimate to Avoid Surprises
- Don't Assume It - Prove It
- Test Early. Test Often. Test Automatically.

# Keep Bug Out
Coding standards are developed to minimize bugs in firmware by focusing on practical rules that keep bugs out while also improving the maintainability and portability of embedded software. I already talked about why your company must have a coding standard and the benefits in this [post]({% post_url programming/2021-04-29-automatic-c-formatting %}).


The [Embedded C Coding Standard ](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard) from Barr Group is an entry point to get in touch with embedded coding standards. If your company doesn't have any coding standard, you could propose the one from Barr Group to start with.
You can find an HTML and PDF version, or you can also buy the book. 


# Jack of All Trades
["Code Complete - A Practical Handbook of Software Construction"](https://amzn.to/3dra0gT) by Steve McConnell synthesizes the most effective techniques and must-know principles into clear, pragmatic guidance. No matter what your experience level, development environment, or project size, this book will inform and stimulate your thinking and help you build the highest quality code.  

This book is not focused on embedded systems, but the software principles showed in this book can be easily applied to embedded systems.


# On Writing Clean Code
Even bad code can work. But if code isn’t clean, it can bring a development organization to its knees. Every year, countless hours and significant resources are lost because of poorly written code. But it doesn’t have to be that way.

If you also thought that you could have done faster your work, only but only if the code were easier to code and understand, then you should recommend ["Clean Code - A Handbook of Agile Software Craftsmanship"](https://amzn.to/2J7mMTE) by Robert Martin.
You can apply immediately most of the tips and pieces of advice given in this book. I said most of the tips because this book was focused on Java developers.

# The Power of Ten
This is not a book, but a paper written by G. J. Holzmann in 2006. At that time he was the lead developer at the "Jet Propulsion Laboratory" in "NASA". 
He wrote the paper ["The Power of 10: Rules for Developing Safety-Critical Code"](https://web.eecs.umich.edu/~imarkov/10rules.pdf), a set of 10 verifiable coding rules that can make the analysis of critical software components more reliable.

The ten rules are:
- Avoid complex flow constructs, such as goto and recursion.
- All loops must have fixed bounds. This prevents runaway code.
- Avoid heap memory allocation.
- Restrict functions to a single printed page.
- Use a minimum of two runtime assertions per function.
- Restrict the scope of data to the smallest possible.
- Check the return value of all non-void functions, or cast to void to indicate the return value is useless.
- Use the preprocessor sparingly.
- Limit pointer use to a single dereference, and do not use function pointers.
- Compile with all possible warnings active; all warnings should then be addressed before the release of the software.

if you want to know the reason behind each rule, just download the [paper](https://web.eecs.umich.edu/~imarkov/10rules.pdf). 

# Conclusion
I gave you 5 books that will help you write better code, easier to understand and read, and reduce considerably the amount of bugs in your code. 
Remember, these books are a compilation of best practices, that means you could apply them  in a different context in the software development or even outside the software industry.

Have fun! And I hope you read these books! 





