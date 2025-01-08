//
#include "worker2.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
//
std::atomic_int keeper{100};
void
test1()
{
  class testWorker : public SimpleWorker::Worker
  {
    std::atomic_int* num;

  public:
    void set(std::atomic_int* np) { num = np; }
    void run() override
    {
      auto prev = num->fetch_add(1);
      if (prev % 10 == 0)
      {
        std::cout << "run: " << prev << std::endl;
        keeper.fetch_sub(1);
      }
    }
  };
  std::vector<testWorker> twList;
  std::atomic_int         count{0};

  class keeperWorker : public SimpleWorker::Worker
  {
  public:
    void run() override { std::cout << "keeper run" << std::endl; }
  };
  keeperWorker kpw;

  {
    SimpleWorker::WorkerThread wt{};
    wt.push(&keeper, kpw);
    twList.resize(1000);
    for (int i = 0; i < twList.size(); i++)
    {
      auto& w = twList[i];
      w.set(&count);
      wt.push(w);
    }

    wt.execute();
    wt.wait();
  }

  std::cout << count.load() << std::endl;
}

//
class ChildWorker : public SimpleWorker::Worker
{
public:
  void run() override { std::cout << "child" << std::endl; }
};
class CheckWorker : public SimpleWorker::Worker
{
public:
  void run() override { std::cout << "check" << std::endl; }
};
std::atomic_int          mainCount{};
std::atomic_int          childCount{};
std::vector<ChildWorker> children;
CheckWorker              wkCheck;
void
test2()
{
  class MainWorker : public SimpleWorker::Worker
  {
    SimpleWorker::WorkerThread& wt;

  public:
    MainWorker(SimpleWorker::WorkerThread& w) : wt(w) {}
    void run() override
    {
      std::cout << "start" << std::endl;

      children.resize(100);
      childCount = children.size();

      mainCount.fetch_add(1);
      wt.push(&childCount, wkCheck, &mainCount);

      for (auto& ch : children)
      {
        wt.push(ch, &childCount);
      }
    }
  };
  class PostWorker : public SimpleWorker::Worker
  {
  public:
    void run() override { std::cout << "post" << std::endl; }
  };

  SimpleWorker::WorkerThread wt{};

  MainWorker wkMain{wt};
  PostWorker wkPost;

  mainCount = 1;
  wt.push(&mainCount, wkPost);
  wt.push(wkMain, &mainCount);

  wt.execute();
  wt.wait();
}

//
std::atomic_int  exec{0};
std::atomic_int  start{0};
std::atomic_int  end{0};
std::vector<int> buffer;
//
bool
push(int p)
{
  int n, e;
  do
  {
    e = end.load();
    n = (e + 1) % buffer.size();
    if (n == start)
    {
      exec = 0;
      return false;
    }
  } while (end.compare_exchange_weak(e, n) == false);
  buffer[e] = p;
  exec      = 0;
  return true;
}

int
pop()
{
  int s = start;
  if (s == end)
  {
    return -1;
  }

  int n = (s + 1) % buffer.size();
  if (start.compare_exchange_weak(s, n) == false)
  {
    return -1;
  }

  auto p    = buffer[s];
  buffer[s] = -1;
  return p;
}

void
test3()
{
  buffer.resize(500);
  std::fill(buffer.begin(), buffer.end(), -1);

  std::condition_variable cond;
  std::mutex              mtx;
  std::atomic_bool        en;

  auto run = [&]()
  {
    {
      std::unique_lock lk(mtx);
      cond.wait(lk);
    }
    for (int i = 0; i < buffer.size() / 3; i++)
    {
      push(i);
      std::cout << "push" << std::endl;
    }
  };

  auto get = [&]()
  {
    {
      std::unique_lock lk(mtx);
      cond.wait(lk);
    }
    while (en)
    {
      for (;;)
      {
        int n = pop();
        if (n == -1)
        {
          break;
        }
        else
        {
          std::cout << n << std::endl;
        }
      }
    }
  };

  en      = true;
  auto t4 = std::thread([&] { get(); });
  auto t1 = std::thread([&] { run(); });
  auto t2 = std::thread([&] { run(); });
  auto t3 = std::thread([&] { run(); });

  std::this_thread::sleep_for(std::chrono::microseconds(500));
  cond.notify_all();

  t1.join();
  t2.join();
  t3.join();
  en = false;
  t4.join();
}

} // namespace

//
int
main(int argc, char** argv)
{
  test1();
  test2();
  test3();

  return 0;
}