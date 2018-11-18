#include "atomic_queue.h"
#include <iostream>

int
main(int argc, char** argv)
{
  std::vector<int> v;
  Queue<int>       q(20);
  int              cnt = 0;

  std::cout << "Pointer queue" << std::endl;
  v.resize(10);
  for (int l = 0; l < 4; l++)
  {
    for (size_t i = 0; i < v.size(); i++)
    {
      v[i] = cnt++;
      if (q.push(&v[i]) == false)
      {
        std::cerr << "not pushed." << std::endl;
      }
    }

    while (int* v = q.pop())
    {
      std::cout << *v << std::endl;
    }
    std::cout << (q.empty() ? "EMPTY" : "-") << std::endl;
  }

  std::cout << "Number queue" << std::endl;
  NQueue<int> nq{10, -1};
  for (int l = 0; l < 4; l++)
  {
    for (int i = 0; i < 10; i++)
    {
      auto n = l * 10 + i;
      if (nq.push(n) == false)
        std::cout << "failed push: " << n << std::endl;
    }
    while (auto r = nq.pop())
    {
      std::cout << r.value << std::endl;
    }
  }
  std::cout << "done." << std::endl;

  return 0;
}
