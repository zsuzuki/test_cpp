//
//
//
#include <execinfo.h>
#include <future>
#include <iostream>
#include <thread>

void
print_backtrace()
{
  void*  array[10];
  int    size;
  char** strings;
  size_t i;

  size    = backtrace(array, 10);
  strings = backtrace_symbols(array, size);

  printf("Obtained %d stack frames.\n", size);

  for (i = 0; i < size; i++)
    printf("%s\n", strings[i]);

  free(strings);
}

void
call_thread()
{
  std::cout << "thread start" << std::endl;
  print_backtrace();
}

int
main()
{
  auto thft = std::async(std::launch::async, []() { call_thread(); });

  thft.wait();

  return 0;
}
