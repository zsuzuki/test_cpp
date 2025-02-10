//
// Copyright 2025 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <dispatch/dispatch.h>
#include <iostream>

class Semaphore
{
  dispatch_semaphore_t sem = dispatch_semaphore_create(1);

public:
  ~Semaphore() { dispatch_release(sem); }

  void lock() { dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER); }
  void unlock() { dispatch_semaphore_signal(sem); }
};

Semaphore sem;

void
task1(int num)
{
  for (int i = 0; i < 3; i++)
  {
    sem.lock();
    std::cout << "Hello: " << num << ":" << i << "\n";
    sem.unlock();
  }
}

void
task2(int num)
{
  for (int i = 0; i < 3; i++)
  {
    sem.lock();
    std::cout << "World: " << num << ":" << i << "\n";
    sem.unlock();
  }
}

int
main(int argc, char** argv)
{
  auto group           = dispatch_group_create();
  auto concurrentQueue = dispatch_queue_create("queueTest", DISPATCH_QUEUE_CONCURRENT);

  for (int i = 0; i < 20; i++)
  {
    dispatch_group_async(group, concurrentQueue, ^{
      if (i & 1)
      {
        task1(i);
      }
      else
      {
        task2(i);
      }
    });
  }

  dispatch_group_notify(group, concurrentQueue, ^{
    std::cout << "barrier\n";
  });

  std::cout << "in wait\n";
  dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
  std::cout << "completed.\n";

  dispatch_release(concurrentQueue);
  dispatch_release(group);

  return 0;
}
