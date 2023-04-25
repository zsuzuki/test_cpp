//
#include "worker2.h"
#include <atomic>
#include <iostream>
#include <vector>

std::atomic_int keeper{100};

int
main(int argc, char** argv)
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

  return 0;
}