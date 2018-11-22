//
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

int
main(int argc, char** argv)
{
  auto vname{argc > 1 ? argv[1] : "Random"};
  auto n = argc > 2 ? std::atoi(argv[2]) : 32;
  auto s = argc > 3 ? std::atoi(argv[3]) : 32;

  std::random_device rnd;
  std::mt19937       mtrand{rnd()};
  std::mt19937_64    mtrand64{rnd()};

  auto mask   = s == 64 ? ~0 : (1ULL << s) - 1;
  auto column = (s + 3) / 4;
  std::cout << std::hex << std::setfill('0');
  std::cout << "// mask = " << mask << std::endl;
  if (s > 32)
    std::cout << "unsigned long long ";
  else
    std::cout << "unsigned int ";
  std::cout << vname << "[] = {";
  for (int i = 0; i < n; i++)
  {
    int cnt = i % 4;
    std::cout << (cnt == 0 ? "\n  0x" : " 0x");
    if (s > 32)
      std::cout << std::setw(column) << (mtrand64() & mask);
    else
      std::cout << std::setw(column) << (mtrand() & mask);
    std::cout << ",";
  }
  std::cout << "\n};" << std::endl;

  return 0;
}
