//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

#include "number_encode.h"

namespace
{
//
uint64_t
unpack_h64(const char* str)
{
  int l = std::strlen(str);
  if (l <= 8)
  {
    return std::strtol(str, nullptr, 16);
  }

  std::string n(str);
  auto        n1    = n.substr(0, l - 8);
  auto        n2    = n.substr(l - 8);
  uint64_t    upper = std::strtol(n1.c_str(), nullptr, 16);
  uint64_t    lower = std::strtol(n2.c_str(), nullptr, 16);
  return upper << 32 | lower;
}
} // namespace

int
main(int argc, char* argv[])
{
  std::random_device shift_random;

  std::array<uint32_t, 8> base;
  for (int i = 0; i < base.size(); i++)
    base[i] = shift_random();

  int wmax = shift_random() & 15;
  for (int i = 0; i < wmax; i++)
    SimpleNumberEncode::encode(0);

  if (argc >= 2)
  {
    if (argc > 2)
    {
      uint64_t n1  = unpack_h64(argv[1]);
      uint64_t n2  = unpack_h64(argv[2]);
      int      cmd = argc > 3 ? argv[3][0] : '+';

      if (cmd == '-')
      {
        auto code = SimpleNumberEncode::sub(n1, n2);
        auto ret  = SimpleNumberEncode::decode(code);
        std::cout << "Sub: " << ret << "/" << std::hex << code << std::dec << std::endl;
      }
      else if (cmd == '=')
      {
        auto ret = SimpleNumberEncode::equal(n1, n2);
        auto c1  = SimpleNumberEncode::decode(n1);
        auto c2  = SimpleNumberEncode::decode(n2);
        std::cout << "Equal: " << (ret ? "YES" : "NO") << "/" << c1 << " : " << c2 << std::endl;
      }
      else
      {
        auto code = SimpleNumberEncode::add(n1, n2);
        auto ret  = SimpleNumberEncode::decode(code);
        std::cout << "Add: " << ret << "/" << std::hex << code << std::dec << std::endl;
      }
    }
    else
    {
      uint32_t n    = std::atoi(argv[1]);
      auto     code = SimpleNumberEncode::encode(n);
      auto     ret  = SimpleNumberEncode::decode(code);
      std::cout << "N: " << n << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
    }
  }
  else
  {
    for (auto b : base)
    {
      auto code = SimpleNumberEncode::encode(b);
      auto ret  = SimpleNumberEncode::decode(code);
      std::cout << "C: " << b << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
    }
  }
}
// End
