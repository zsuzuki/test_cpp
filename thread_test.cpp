//
// threadテスト
//
#include "atomic_queue.h"
#include "worker.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>

namespace
{
// async呼び出し
int
mul(int a, int b)
{
  return a * b;
}
} // namespace

//
//
//
int
main()
{
  struct T
  {
    int a = 5;
    int b = 7;
  };

  printf("Hello\n");
  int  a = 8;
  int  b = 3;
  int  c = 12;
  auto t = std::async(std::launch::async, mul, a, b);

  WorkerThread th;

  printf("threads: %d\n", std::thread::hardware_concurrency());
  th.push([](const char* p) { printf("%s\n", p); }, "worker one.");
  th.push([](int n) { printf("%d\n", n); }, 9);
  th.push([](const T& t) { printf("%d/%d\n", t.a, t.b); }, T());
  th.push([&]() { printf("lambda:%d,%d\n", b, c); });
  c = a + b;

  for (int i = 0; i <= 100; ++i)
  {
    if ((i % 10) == 0)
    {
      th.push(
          [i, &th](int d) {
            printf("ten : %d %d\n", d, i);
            th.push([i] { printf("chain job %d\n", i); });
          },
          i / 10);
      printf("ten counter\n");
    }
    else if (i & 1)
      th.push([i]() { printf("odd : %d\n", i); });
    else
      th.push([i]() { printf("even: %d\n", i); });
  }

  printf("result:%d/%d\n", t.get(), c);

  printf("Good-bye.\n");
  return 0;
}
// End
