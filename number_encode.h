//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#pragma once

#include <cstdint>
#include <utility>

namespace SimpleNumberEncode
{

//
std::pair<uint64_t, uint64_t>
get_encode_number(int idx)
{
  static const uint64_t encode_table[] = {
      0x07c667b4, 0x0113a0c7, 0x059e95d5, 0x00f8906b, 0x06c19c92, 0x0492616c, 0x03fd9fac, 0x00ff7ab4, 0x0031a32e,
      0x0451a094, 0x01cae570, 0x019abd2f, 0x07890755, 0x077483a3, 0x060b7bec, 0x0101b5e0, 0x024a79dc, 0x05d5e211,
      0x00d49adf, 0x002a9014, 0x001d4d7e, 0x047f0b48, 0x039ee552, 0x04bfbdac, 0x03b96e2e, 0x07b19f81, 0x02c40a61,
      0x056e07fe, 0x021ea4c0, 0x03d1bc1d, 0x02a5ac4c, 0x0577c6e4, 0x02f7c379,
  };
  return std::make_pair(encode_table[idx], encode_table[idx + 1]);
}

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
  static uint64_t shift_count = 0;

  uint64_t code = num;
  auto     s    = ++shift_count & 31;
  auto     en   = get_encode_number(s);
  code ^= en.first;
  code *= en.second;
  code = rotate(code, s);
  code = (code & 0xffffffff) << 32 | ((code >> 32) & 0x07ffffff) | (s << (32 - 5));

  return code;
}

//
uint32_t
decode(uint64_t code)
{
  uint64_t num = (code & 0x07ffffff) << 32 | ((code >> 32) & 0xffffffff);
  uint64_t s   = (code >> (32 - 5)) & 31;
  num          = rotate(num, 59 - s);
  auto en      = get_encode_number(s);
  num /= en.second;
  num ^= en.first;

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
sub(uint64_t a, uint64_t b)
{
  return encode(decode(a) - decode(b));
}

//
bool
equal(uint64_t a, uint64_t b)
{
  return decode(a) == decode(b);
}

//
bool
greater(uint64_t a, uint64_t b)
{
  return decode(a) > decode(b);
}

//
bool
less(uint64_t a, uint64_t b)
{
  return decode(a) < decode(b);
}
} // namespace SimpleNumberEncode