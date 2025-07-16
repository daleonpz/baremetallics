---
layout: post
title: Lock, Unlock, Blocked? 
category: programming
description: Learn how to manage threading in C++ with mutexes and condition variables. Avoid common pitfalls in multi-threaded applications by optimizing lock usage and preventing queue blocking during long operations.
tags: [story, debug, c++, threading]
---

I’ve been working on a multi-threaded C++ app with threads. It’s pretty simple on paper: think of it like a mail service that gets letters (requests) and puts them in a queue. Then there’s a worker thread who picks up the letters and checks if they’re formatted right. Easy, right? Just throw a mutex on the queue, and call it a day. Yeah, not quite!

This bug had me banging my head on the wall for hours :(

**Protip**: When you’re stuck, try applying the same logic to a simpler, but similar case. If it makes sense, great. If it doesn’t, you’ve found the problem.

* 
{:toc}

# The Mail Dispatcher Setup

The "mailroom" has two parts:

1. **Letter Queue (Request Queue)**: Incoming letters (requests) get stored here. Every time a new letter comes in, it gets added to the queue. We need to protect this queue with a mutex since multiple threads (other mail workers) might try to add or remove letters at the same time.

2. **Mail Worker (Thread)**: This worker constantly watches the queue, grabs letters, and processes them (making sure they’re correctly formatted).

# What I Forgot to Ask Myself

Should the Mail Worker be independent from the queue? Not really, because if there are no letters, the worker has nothing to do. But here’s the kicker: the worker shouldn’t block the queue while processing. **That** realization cost me 5 hours.

Here’s what I learned: **Processing letters shouldn't hold up the queue**. New letters need to keep coming in while the worker is doing its thing.

Here’s how the implementation looks:

```cpp
class MailDispatcher{
public:
    void AddToQueue(auto req) {
        // Protect shared resources while adding to the queue
        std::lock_guard<std::mutex> lock(mutex_);
        requestQueue_.push(req);
        cond_var_.notify_one();  // Signal worker thread about new request
    }

private:
    std::queue<auto> requestQueue_;
    std::mutex mutex_;
    std::condition_variable cond_var_;

    void MailWorkerThreadFunction() {
        while (true) {
            // Wait for a new request
            std::unique_lock<std::mutex> lock(mutex_);
            cond_var_.wait(lock, [&] { return !requestQueue_.empty(); });
            auto req = requestQueue_.front();
            requestQueue_.pop();
            lock.unlock();  // Release the lock so other threads can access the queue

            // Process the request independently
            ProcessRequest(req);
        }
    }
};
```

# Takeaways

- **Keep the critical section short**: Only lock the mutex while accessing shared data (like the queue). This prevents unnecessary blocking.
- **Don’t lock during long operations**: Once the letter is out of the queue, release the lock. Processing can happen outside of the critical section.
- **Unlock early**: Free up the queue as soon as possible so other threads can add or retrieve requests without waiting.
