---
layout: post
title: Embedded Ada Journey Extras - Ravenscar / SPARK
category: programming
---

If your google something about Ada programming language you will encounter yourself with the following terms **Ravenscar Profile** and **SPARK**.
So what are these terms:

# SPARK
 It's a subset of the ADA language specifically designed for engineering high-reliability applications, with strong features to guarantee reliability, safety, and security with formal methods.
It uses _contracts_ (pre- and postconditions) to describe the specification of components in a form that is suitable for both static and dynamic verification.

For example, a simple Ada program:
```ruby
procedure Swap_X_And_Y ( X,Y: in out Integer);
```

What are the restrictions of the procedure? X and Y can virtually be anything.
But in SPARK: 

```ruby
procedure Swap_X_And_Y (X,Y: in out Integer) 
with
  Globals => (In_Out => (X, Y)),
  --  Swap_X_And_Y modifies the global variables X and Y.
  Depends => (X => Y, Y => X);
  --  The final value of X depends only on the initial value of Y 
  --  and the final value of Y depends only on the initial value of X.
```

In this case it's explicit that X and  Y are global variables and that the value of X will depend only on Y and viceversa.

# Ravenscar Profile
As the name suggests, Ravenscar is a profile unlike SPARK that is a programming language. 
Ravenscar can be seen as a set of restrictions specified as **compiler directives**.
The motivation for Ravenscar profile primarily comes from the need to be able to verify concurrent real-time programs, and to have these programs implemented reliably and efficiently.


The ravenscar profile is based on the following pragmas
```ruby
pragma Task_Dispatching_Policy (FIFO_Within_Priorities);
pragma Locking_Policy (Ceiling_Locking);
pragma Detect_Blocking;
pragma Restrictions (
                 No_Abort_Statements,
                 No_Dynamic_Attachment,
                 No_Dynamic_Priorities,
                 No_Implicit_Heap_Allocations,
                 No_Local_Protected_Objects,
                 No_Local_Timing_Events,
                 No_Protected_Type_Allocators,
                 No_Relative_Delay,
                 No_Requeue_Statements,
                 No_Select_Statements,
                 No_Specific_Termination_Handlers,
                 No_Task_Allocators,
                 No_Task_Hierarchy,
                 No_Task_Termination,
                 Simple_Barriers,
                 Max_Entry_Queue_Length => 1,
                 Max_Protected_Entries  => 1,
                 Max_Task_Entries       => 0,
                 No_Dependence => Ada.Asynchronous_Task_Control,
                 No_Dependence => Ada.Calendar,
                 No_Dependence => Ada.Execution_Time.Group_Budget,
                 No_Dependence => Ada.Execution_Time.Timers,
                 No_Dependence => Ada.Task_Attributes);
```

Whenever you want to use the Ravenscar profile you just uses the compiler directive

```ruby
pragma Profile (Ravenscar);
```

More information about Ravenscar Profile can be found [here](http://www.open-std.org/JTC1/SC22/WG9/n424.pdf)

# SPARK + Ravenscar
It seems that it's possible to work with SPARK and Ravenscar at the same time. 
[This post entry](https://blog.adacore.com/spark-2016-supports-ravenscar) on Adacore blog show some functionalities of SPARK 2016 that has support for Ravenscar.
Also the is an official documentation of the **SPARK Ravenscar Profile** given by the SPARK team. These documentation can be found [here](http://docs.adacore.com/sparkdocs-docs/Examiner_Ravenscar.htm#_Toc311793138). 
