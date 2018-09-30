//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#include <array>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <cstring>

namespace
{
const uint64_t random_table[] = {
    0x07c667b4, 0x0113a0c7, 0x059e95d5, 0x00f8906b, 0x06c19c92, 0x0492616c, 0x03fd9fac, 0x00ff7ab4, 
    0x0031a32e, 0x0451a094, 0x01cae570, 0x019abd2f, 0x07890755, 0x077483a3, 0x060b7bec, 0x0101b5e0, 
    0x024a79dc, 0x05d5e211, 0x00d49adf, 0x002a9014, 0x001d4d7e, 0x047f0b48, 0x039ee552, 0x04bfbdac, 
    0x03b96e2e, 0x07b19f81, 0x02c40a61, 0x056e07fe, 0x021ea4c0, 0x03d1bc1d, 0x02a5ac4c, 0x0577c6e4, 
    0x02f7c379,
};
uint64_t shift_count = 0;

//
uint64_t
rotate(uint64_t n, uint64_t n_sh = 16)
{
  uint64_t m1 = (1ULL << n_sh) - 1;
  uint64_t m2 = 0x07ffffffffffffffULL ^ m1;
  uint64_t v1 = (n >> (59 - n_sh)) & m1;
  uint64_t v2 = (n << n_sh) & m2;
  return v1 | v2;
}

//
uint64_t
encode(uint32_t num)
{
  uint64_t code = num;
  auto     s    = (shift_count++ >> 10) & 31;
  code ^= random_table[s + 0];
  code *= random_table[s + 1];
  code = rotate(code, s);
  code = ((code << 5) & 0xffffffff00000000ULL) | (code & 0x07ffffffULL) | (s << (32 - 5));

  return code;
}

//
uint32_t
decode(uint64_t code)
{
  uint64_t num = (code & 0x07ffffffULL) | ((code >> 5) & 0x07fffffff8000000ULL);
  uint64_t s   = (code >> (32 - 5)) & 31;
  num          = rotate(num, 59 - s);
  num /= random_table[s + 1];
  num ^= random_table[s + 0];

  return num;
}

//
uint64_t
add(uint64_t a, uint64_t b)
{
  return encode(decode(a) + decode(b));
}

//
uint64_t
unpack_h16(const char* str)
{
  int l = std::strlen(str);
  if (l <= 8)
  {
    return std::strtol(str, nullptr, 16);
  }
  
  std::string n(str);
  auto n1 = n.substr(0, l - 8);
  auto n2 = n.substr(l - 8);
  uint64_t upper = std::strtol(n1.c_str(), nullptr, 16);
  uint64_t lower = std::strtol(n2.c_str(), nullptr, 16);
  return upper << 32 | lower;
}

} // namespace

int
main(int argc, char* argv[])
{
  std::random_device shift_random;
  shift_count = shift_random();

  std::array<uint32_t, 8> base;
  for (int i = 0; i < base.size(); i++)
    base[i] = shift_random();

  if (argc >= 2)
  {
    if (argc > 2)
    {
      uint64_t n1 = unpack_h16(argv[1]);
      uint64_t n2 = unpack_h16(argv[2]);
      auto code = add(n1, n2);
      auto ret  = decode(code);
      std::cout << "Add: " << ret << "/(" << decode(n1) << ")" << std::hex << code << std::dec << std::endl;
    }
    else
    {
      uint32_t n = std::atoi(argv[1]);
      auto code = encode(n);
      auto ret  = decode(code);
      std::cout << "N: " << n << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
    }
  }
  else
  {
    for (auto b : base)
    {
      uint64_t num = b;
      auto code = encode(b);
      auto ret  = decode(code);
      std::cout << "C: " << b << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
    }
  }
}
// End
