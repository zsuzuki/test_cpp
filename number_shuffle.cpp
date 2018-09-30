//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#include <array>
#include <cstdint>
#include <iostream>
#include <random>

using namespace std;

namespace
{
uint64_t random_table[] = {
    0x98316e0,  0xf0e784f,  0x1910061b, 0x170e1b8f, 0x3f5184c,  0x9428c8f,  0x10d06c89, 0x19dba286, 0x1ea63fa4, 0x12a1f311,
    0x1a8213a6, 0x172560f2, 0x7c76d09,  0xca6c360,  0x33f7632,  0x1f02846e, 0x16525761, 0x17233a99, 0x416e239,  0x1387edbe,
    0x9e110e1,  0x4c905a7,  0x1c4a76b0, 0x1f615f9e, 0xb5c021e,  0x3277d14,  0x2d594ac,  0xf20b046,  0x1ce04452, 0x15e5967e,
    0x10b1c257, 0x1d0055f0, 0x14eb3218, 0x6074635,  0xa4ff10d,  0x46f1b2f,  0xf9d473e,  0xe097475,  0x15d5768,  0xd24e22a,
    0x1573f8e9, 0xa9b8db4,  0x38a5b06,  0x525ace9,  0x1cbc44b5, 0x25c92d4,  0x1755896a, 0x5fb4221,  0x3d10299,  0xa985326,
    0x1bb26595, 0x1637b3b2, 0x1cb0ae04, 0x281e420,  0x2c9acb0,  0xf0b05ee,  0x162098e7, 0x1ed0d60c, 0x45fd83a,  0x3998f45,
    0xe87fca1,  0x1e76e3eb, 0x135bdb4a, 0x42a031e,
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
  static int shift_table[] = {
      7, 8, 6, 12, 1, 24, 28, 20, 19, 0, 3, 13, 9, 2, 14, 10, 4, 5, 17, 29, 15, 16, 21, 22, 25, 11, 18, 27, 30, 23, 26, 31,
  };
  static int shift_count = 0;

  uint64_t code = num;
  auto     s    = shift_table[(shift_count + 1) % (sizeof(shift_table) / sizeof(int))];
  code ^= random_table[s + 1];
  code *= random_table[s + 2];
  code = rotate(code, s);
  code = ((code << 5) & 0xffffffff00000000) | (code & 0x07ffffff) | (s << (32 - 5));

  return code;
}

uint32_t
decode(uint64_t code)
{
  uint64_t num = (code & 0x07ffffff) | ((code >> 5) & 0x07fffffff8000000);
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
    uint64_t s   = ((uint64_t)((double)rnd() / (double)(0x8000000)));
    cout << "Original: ";
    cout.width(10);
    cout << num << "(rnd:";
    cout.width(2);
    cout << s << ") ";
    cout << hex;
    num ^= random_table[s + 1];
    num *= random_table[s + 2];
    num = rotate(num, s);
    num = ((num << 5) & 0xffffffff00000000) | (num & 0x07ffffff) | (s << (32 - 5));
    cout << "Result: ";
    cout.width(16);
    cout << num;
    uint64_t r_num = (num & 0x07ffffff) | ((num >> 5) & 0x07fffffff8000000);
    uint64_t r_s   = (num >> (32 - 5)) & 31;
    r_num          = rotate(r_num, 59 - r_s);
    r_num /= random_table[r_s + 2];
    r_num ^= random_table[r_s + 1];
    cout << dec;
    cout << " Reverse:";
    cout.width(10);
    cout << r_num << "(" << r_s << ")" << endl;

    {
      auto code = encode(b);
      auto ret  = decode(code);
      std::cout << "C: " << b << "/" << ret << "/" << std::hex << code << std::dec << std::endl;
    }
  }
}
// End
