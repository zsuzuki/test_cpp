#include <atomic>
#include <boost/fiber/all.hpp>
#include <iostream>

std::atomic_int count0{0};
std::atomic_int count1{0};

//
void
fiber_0()
{
  count0.fetch_add(1);
  auto cnt = count0.load();
  if ((cnt % 100) == 0)
  {
    std::cout << "Hello:" << cnt << std::endl;
  }
}

//
void
fiber_1()
{
  count1.fetch_add(1);
  auto cnt = count1.load();
  if ((cnt % 100) == 0)
  {
    std::cout << "World:" << cnt << std::endl;
  }
}

//
int
main(int argc, char** argv)
{
  boost::fibers::fiber c0{[]()
                          {
                            for (int i = 0; i < 1000; i++)
                            {
                              fiber_0();
                              boost::this_fiber::yield();
                            }
                          }};
  boost::fibers::fiber c1{[]()
                          {
                            for (int i = 0; i < 1000; i++)
                            {
                              fiber_1();
                              boost::this_fiber::yield();
                            }
                          }};

  c0.detach();
  c1.join();
  return 0;
}
//
