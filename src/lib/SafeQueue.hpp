
#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include "pch.hpp"

// A threadsafe-queue.
template <class T>
class SafeQueue
{

private:
  std::queue<T> mQueue;
  mutable std::mutex mMutex;
  std::condition_variable mCondVar;

public:
  SafeQueue(void) : mQueue(), mMutex(), mCondVar()
  {
  }

  ~SafeQueue(void)
  {
  }

  // Add an element to the queue's end.
  void push(T t)
  {
    std::lock_guard<std::mutex> lock(mMutex);
    mQueue.push(t);
    mCondVar.notify_one();
  }

  // Get the "front"-element.
  // If the queue is empty, wait till a element is avaiable.
  T pop(void)
  {
    std::unique_lock<std::mutex> lock(mMutex);
    while (mQueue.empty())
    {
      mCondVar.wait(lock); // release lock as long as the wait and reaquire it afterwards.
    }
    T val = mQueue.front();
    mQueue.pop();
    return val;
  }

  unsigned int size() const
  {
    return mQueue.size();
  }
};
#endif