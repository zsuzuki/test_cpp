//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#include <array>
#include <cstdint>
#include <iostream>
#include <random>

namespace
{
const uint64_t random_table[] = {
    0x07c667b4, 0x0113a0c7, 0x059e95d5, 0x00f8906b, 0x06c19c92, 0x0492616c, 0x03fd9fac, 0x00ff7ab4, 
    0x0031a32e, 0x0451a094, 0x01cae570, 0x019abd2f, 0x07890755, 0x077483a3, 0x060b7bec, 0x0101b5e0, 
    0x024a79dc, 0x05d5e211, 0x00d49adf, 0x002a9014, 0x001d4d7e, 0x047f0b48, 0x039ee552, 0x04bfbdac, 
    0x03b96e2e, 0x07b19f81, 0x02c40a61, 0x056e07fe, 0x021ea4c0, 0x03d1bc1d, 0x02a5ac4c, 0x0577c6e4, 
    0x02f7c379, 0x06265213, 0x055e1650, 0x0604c8b8, 0x011f571f, 0x070ee9e1, 0x00f2cef0, 0x0370582a, 
    0x05cf36bb, 0x05f9f53f, 0x07151ca0, 0x074df9cc, 0x01789f63, 0x030db566, 0x039ec702, 0x021a7d48, 
    0x00392787, 0x00db70b7, 0x04a295eb, 0x0469f5e7, 0x07bbfd19, 0x0263a38f, 0x077a6acd, 0x00d8a520, 
    0x02988d2b, 0x007afa39, 0x02537ff7, 0x07b56de0, 0x073c78d5, 0x062edb6e, 0x04c5ea2b, 0x0573a839, 
};
std::array<const uint64_t, 32> shift_table = {
    7, 8, 6, 12, 1, 24, 28, 20, 19, 0, 3, 13, 9, 2, 14, 10, 4, 5, 17, 29, 15, 16, 21, 22, 25, 11, 18, 27, 30, 23, 26, 31,
};

uint64_t
rotate(uint64_t n, uint64_t n_sh = 16)
{
  uint64_t m1 = (1ULL << n_sh) - 1;
  uint64_t m2 = 0x07ffffffffffffffULL ^ m1;
  uint64_t v1 = (n >> (59 - n_sh)) & m1;
  uint64_t v2 = (n << n_sh) & m2;
  return v1 | v2;
}

uint64_t
encode(uint32_t num)
{
  static int shift_count = 0;

  uint64_t code = num;
  auto     s    = shift_table[shift_count++ % shift_table.size()];
  code ^= random_table[s + 1];
  code *= random_table[s + 2];
  code = rotate(code, s);
  code = ((code << 5) & 0xffffffff00000000ULL) | (code & 0x07ffffffULL) | (s << (32 - 5));

  return code;
}

uint32_t
decode(uint64_t code)
{
  uint64_t num = (code & 0x07ffffffULL) | ((code >> 5) & 0x07fffffff8000000ULL);
  uint64_t s   = (code >> (32 - 5)) & 31;
  num          = rotate(num, 59 - s);
  num /= random_table[s + 2];
  num ^= random_table[s + 1];

  return num;
}

} // namespace

int
main(int argc, char* argv[])
{
  std::random_device      rnd;
  std::array<uint32_t, 8> base;
  for (int i = 0; i < base.size(); i++)
    base[i] = rnd();

  for (auto b : base)
  {
    uint64_t num = b;
    auto code = encode(b);
    auto ret  = decode(code);
    std::cout << "C: " << b << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
  }
}
// End
