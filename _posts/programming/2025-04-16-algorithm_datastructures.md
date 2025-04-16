---
layout: post
title: Algorithms and Data Structures
category: programming
description: 
---

## Basic Data Structures and Use cases
* Linked list
* Stack and queue
* Sets
* Hash tables
	Hash tables support one of the most efficient types of searching: hashing. Funda- mentally, a hash table consists of an array in which data is accessed via a special index called a key. The primary idea behind a hash table is to establish a mapping between the set of all possible keys and positions in the array using a hash func- tion. A hash function accepts a key and returns its hash coding, or hash value.  Keys vary in type, but hash codings are always integers.  Since both computing a hash value and indexing into an array can be performed in constant time, the beauty of hashing is that we can use it to perform constant- time searches. When a hash function can guarantee that no two keys will gener- ate the same hash coding, the resulting hash table is said to be directly addressed
* Trees:  In computing, a tree consists of elements called nodes organized in a hierarchical arrangement. The node at the top of the hierarchy is called the root. The nodes directly below the root are its children, which in turn usually have children of their own. With the exception of the root, each node in the hierarchy has exactly one parent, which is the node directly above it. The number of children a node may parent depends on the type of tree. This number is a tree’s branching factor, which dictates how fast the tree will branch out as nodes are inserted. This chapter focuses on the binary tree, a relatively simple but powerful tree with a branching factor of 2. It also explores binary search trees, binary trees organized specifically for searching.
	* Binary trees Trees containing nodes with up to two children. The binary tree is a very pop- ular type of tree utilized in a wide variety of problems. It provides the founda- tion for more sophisticated tree structures as wel Heaps and priority queues
	* Heaps: Trees organized so that we can determine the node with the largest value quickly. The cost to preserve this property is less than that of keeping the data sorted. We can also organize a heap so that we can determine the smallest value just as easily
	* Priority queues: Data structures naturally derived from heaps. In a priority queue, data is organized in a heap so that we can determine the node with the next highest priority quickly. The “priority” of an element can mean different things in different problems

* Graphs: Flexible data structures typically used to model problems defined in terms of relationships or connections between objects. Objects are represented by verti- ces, and the relationships or connections between the objects are represented by edges between the vertices.

## Algorithms and Data Structures
- Insertion sort
Although not the most efficient sorting algorithm, insertion sort has the virtue of simplicity and the ability to sort in place. Its best application is for incre- mental sorting on small sets of data.
- Quicksort
An in-place sorting algorithm widely regarded as the best for sorting in the general case. Its best application is for medium to large sets of data.
- Merge sort
An algorithm with essentially the same performance as quicksort, but with twice its storage requirements. Ironically, its best application is for very large sets of data because it inherently facilitates working with divisions of the origi- nal unsorted set.
- Counting sort
A stable, linear-time sorting algorithm that works with integers for which we know the largest value. Its primary use is in implementing radix sort.
- Radix sort
A linear-time sorting algorithm that sorts elements digit by digit. Radix sort is well suited to elements of a fixed size that can be conveniently broken into pieces, expressible as integers.
- Binary search
An effective way to search sorted data in which we do not expect frequent insertions or deletions. Since resorting a set of data is expensive relative to searching it, binary search is best when the data does not change.


[Ref](https://www.cs.yale.edu/homes/aspnes/classes/223/notes.html)
[Mastering Algorithms with C](https://www.oreilly.com/library/view/mastering-algorithms-with/1565924533/)

