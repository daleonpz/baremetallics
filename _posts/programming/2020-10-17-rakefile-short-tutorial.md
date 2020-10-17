---
layout: post
title: Rakefile 101 for Embedded Projects
category: programming
description: rake for C embedded projects
---

This is a short introduction to Rakefiles.

I use rakefiles for building C projects because it's easier than CMake and Makefiles.

# What are rakefiles?

Rakefiles is a ruby version of Makefiles. It uses Ruby syntax.

In a more strict sense, 
> Rake is a software task management and build automation tool.

You can find [here](https://martinfowler.com/articles/rake.html) a great tutorial about "How to use Rake Build Language". 

For this tutorial you will need to know two things:

- Tasks/Rules
- Dependency Based Programming

# Rake Tasks/Rules
Rake uses tasks as building blocks. 
You must define tasks to take full advantage of rake.

Each task definition consists of:

- The name that identifies the task
- The code to be executed by the task

For example:

```ruby
task :task_name do
    # Your code goes here
end
```

Rake also uses something known as **rules**.  

Rules are similar to tasks. 
But **rules define filenames** and a block of Ruby code. 

> For each matching file, the rule creates and runs a new task with the specified code.

You may use either names or regular expressions to define the files that match the rule.

For example:

```ruby
rule '.o' => '.c' do |task|
    sh "gcc -c #{task.source}"
end
```

In this case, this rule matches all files with `*.o` extension.  

Things to note:
- The `task` is the object representing the task itself.
- `task.source` is a reference to the source files of this rule.

# Dependency Based Programming

Let's say you want to build and test your code.
It makes sense that before testing you must build your code.

That means that `test` depends on `build`.

In rake, you can define dependencies using the `=>` symbol followed by `[:dependency_1, :dep_2]`.

For example:
 
```ruby
task :build => do
  #do the compilation
end

task :test => [:build] do
  # run the tests
end
```

I hope you got the basic idea about rakefiles.

In future post, we'll take this concepts to automate builds and tests for embedded projects 
