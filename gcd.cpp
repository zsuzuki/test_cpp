//
// Copyright 2025 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <dispatch/dispatch.h>
#include <iostream>

void
task1(int num)
{
  for (int i = 0; i < 3; i++)
  {
    std::cout << "Hello: " << num << "\n";
  }
}

void
task2(int num)
{
  for (int i = 0; i < 3; i++)
  {
    std::cout << "World: " << num << "\n";
  }
}

int
main(int argc, char** argv)
{
  auto group           = dispatch_group_create();
  auto concurrentQueue = dispatch_queue_create("com.example.serialQueue", DISPATCH_QUEUE_CONCURRENT);

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

  return 0;
}
