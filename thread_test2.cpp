//
#include "worker2.h"
#include <atomic>
#include <iostream>
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

} // namespace

//
int
main(int argc, char** argv)
{
  test1();
  test2();

  return 0;
}