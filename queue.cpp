#include "atomic_queue.h"
#include <iostream>

int
main(int argc, char** argv)
{
  std::vector<int> v;
  Queue<int>       q(20);

  v.resize(10);
  for (size_t i = 0; i < v.size(); i++)
  {
    v[i] = i;
    if (q.push(&v[i]) == false)
    {
      std::cerr << "not pushed." << std::endl;
    }
  }

  while (int* v = q.pop())
  {
    std::cout << *v << std::endl;
  }

  return 0;
}
