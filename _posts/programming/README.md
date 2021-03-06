---
layout: post
title: Creating a Post
category: programming
description: Small post description with 160 characters max. 
---

To make your blog post, just create a new markdown file in the `_posts/programming/` directory with the title format `YYYY-MM-DD-title.md`. Posts follow the simple [markdown syntax](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) and can include a wide array of multimedia formats.

# General Notes

- **Focus only on one topic**. One idea. We try to keep our posts short but well explained.  
    For example, if you want to talk about compilers. Some posts would be "what's a compiler?", "why we need a different compile for ARM?", "Compiling using CMake". 
- **Think about your reader**. Give a clear and concise idea. **Make it easy to learn**. 
- **Avoid** as much as possible **passive voice** when you write.
- **Reduce the number of adverbs**. This code runs _silently_ in the background.  _silently_ doesn't add any new information. Removing it won't affect the meaning of the sentence. 
- **Keep your paragraphs short**. As well as your sentences. Paragraphs no longer than 3-4 sentences.  
- **Keep it casual, but well explained**. Avoid as much as possible complicated words. Instead of _retrieve_ use _get_. Instead of _Nevertheless_ use _but_. I think you got the idea. We recommended this [book](https://amzn.to/2XtlRoX) if you want to improve your writing. 
- Use tools like [Grammarly](https://www.grammarly.com/) and [Hemingwayapp.com](http://www.hemingwayapp.com/) to check your grammar and keep your writing simple.
- **Make your own pictures**. Google Drawings is a simple and free solution. The font we use is **Comfortaa Bold**.
- **Add** always a **conclusion or summary**. 

Here are some examples of how to format your Markdown file.

# This is a header

And this is a simple paragraph.

## Smaller header

This is how you **bold** & *italicize* text.

Here is some `inline code`.

Here some code with line numbers
{% highlight c linenos %}
blockCode() {
	return true;
}
{% endhighlight %}

some code :
```c
blockCode() {
	return true;
}
```

# Another header

And here's a list:

* Item 1
* Item 2
* Item 3

# Images 

To include images 

![Image description](/images/post/<image-name>.png)

Of course you should also send the image or images you use. 

# Quotes

Read my quote. 

> Here a quote 
