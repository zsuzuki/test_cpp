//
// Copyright 2024 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include "fnv1.h"
#include <iostream>
#include <random>
#include <sstream>

std::vector<char> charList;
std::vector<int>  countList;

int
main(int argc, char** argv)
{

  for (char ch = 'A'; ch <= 'Z'; ch++)
  {
    charList.push_back(ch);
    charList.push_back(ch + ('a' - 'A'));
  }
  for (char num = '0'; num <= '9'; num++)
  {
    charList.push_back(num);
  }
  countList.resize(charList.size());
  std::fill(countList.begin(), countList.end(), 0);

  std::random_device rnd;
  std::mt19937_64    mtrand{rnd()};

  for (;;)
  {
    auto getIndex = [&](int rnum, int range) { return fnv1::generate(rnum) % range; };

    std::stringstream passwd;
    for (int i = 0; i < 16; i++)
    {
      for (;;)
      {
        int  type  = getIndex(mtrand(), 100);
        auto sz    = charList.size() - ((i == 0) ? 10 : 0);
        auto idx   = getIndex(mtrand(), sz);
        auto cnt   = countList[idx];
        auto retry = getIndex(mtrand(), 100);
        if (retry >= cnt * 40)
        {
          passwd << (char)charList[idx];
          countList[idx]++;
          break;
        }
      }
    }
    auto makeStr  = passwd.str();
    int  haveUp   = 0;
    int  haveDown = 0;
    int  haveNum  = 0;
    for (auto ch : makeStr)
    {
      haveUp += isupper(ch);
      haveDown += islower(ch);
      haveNum += isnumber(ch);
    }
    if (haveUp > 3 && haveDown > 3 & haveNum > 0)
    {
      std::cout << passwd.str();
      break;
    }
    std::fill(countList.begin(), countList.end(), 0);
  }

  return 0;
}
