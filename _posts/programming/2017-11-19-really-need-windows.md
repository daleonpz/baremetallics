---
layout: post
title: If you really need to use Windows
category: programming
tags: [windows, linux, tools, vim]
---

I'm doing a masters in Germany but I don't have a _portable_ laptop. It is falling to pieces, so basically my laptop is used as a desktop. so weit, so gut... I am a **Linux** user, my keyboard layout is **programmer Dvorak**  and a use **vim** a lot as well.  However, sometimes I need to use Windows laptops or desktops to get my uni related things done when I am at the University.

During the last month I have been struggling and cursing the university for using windows. For gods sake, we are studying computer science and embedded systems!... Anyway, I was thinking in buying some low-end laptop just to use Linux and get my job done, but in my bottom of my heart I knew that it weren't the best solution.

Main problems with windows machines in my university

* They use windows
* Cannot use terminal, I will need Cygwin to do so (it will take some time to install)
* I don't have Administrative rights
* In Windows it is not available the _Programmers Dvorak_ layout
* Development environments are highly mouse depended, and slow in terms of workflow


After digging in the web for 3 hours I found a solution that:

* Is portable 
* Is fast to set up, one minute max
* Is cheap, almost 0 euros
* Do no required Administrative rights

The solution includes:
* **Portable keyboard layout - PKL** ([link](https://github.com/renxida/pkl-dvp)): I couldn't' stand using just Dvorak layout to do my coding, I really needed the programmer Dvorak layout. A common solutions requires the use of _Microsoft keyboard layout creator_, and I think it wouldn't be an efficient solution. However, I found this _pkl_ that it is basically plug and play.
* **gVim** ([link](https://vim.sourceforge.io/download.php)): I chose not to use any kind of terminal, because the tasks in the university are not that difficult so I can stand clicking in time to time in order to compile my job, but at least my I can type as fast as I do in Linux. And also it allows me to use my `.vimrc` setup.
* **USB-stick**: to use these programs from there

# Conclusion
I ended up minimizing my anger and discomfort of using Windows to do my coding at the University with a highly effective solution. I hope this post will others that hate using Windows for coding.


