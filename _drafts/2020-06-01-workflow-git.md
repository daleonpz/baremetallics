---
title:
---

```sh
$ mkdir NewProject
$ cd NewProject
$ ceedling new code
$ cd code/
$ mkdir include/

```

modify `project.yml`
At `:path:` add `:include:` 

```yml
:paths:
  :test:
    - +:test/**
    - -:test/support
  :source:
    - src/**
  :include:
    - include/
  :support:
    - test/support
```

At the end add

```yml
:module_generator:
  :project_root: ./
  :source_root: src/
  :inc_root: include/
  :test_root: test/
```

To create a new module

```sh
$ ceedling module:create[MyModule]
File src/MyModule.c created
File include/MyModule.h created
File test/test_MyModule.c created
Generate Complete
```

To run the test. By default test files have only ignored tests.
```sh
$ ceedling clobber test:all


Test 'test_MyModule.c'
----------------------
Generating runner for test_MyModule.c...
Compiling test_MyModule_runner.c...
Compiling test_MyModule.c...
Compiling unity.c...
Compiling MyModule.c...
Compiling cmock.c...
Linking test_MyModule.out...
Running test_MyModule.out...

--------------------
IGNORED TEST SUMMARY
--------------------
[test_MyModule.c]
  Test: test_MyModule_NeedToImplement
  At line (15): "Need to Implement MyModule"

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  1
PASSED:  0
FAILED:  0
IGNORED: 1
```


We need some sort of building system. I'll use rakefiles.
Go back to root directory

```sh
$ cd ..
$ pwd 
/home/me/Documents/git/test/NewProject
```

Let's create rakefile
```ruby

```
