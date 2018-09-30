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
      0xfe9494af, 0x4d80291b, 0x83baf026, 0x57d54135, 0xb799056d, 0xeb23d7ef, 0x4dc2eb5b, 0xb384ac7e, 0xa60623b5,
      0x7b230661, 0xe40f92b0, 0x020340db, 0xac136d61, 0x2aa0314d, 0xcedc6fca, 0x094369d2, 0x932ee623, 0x6271fc98,
      0x416bf52f, 0xfb805cb3, 0x96bf1b47, 0x0763e831, 0x7dcddced, 0xe0250084, 0x0791188e, 0xa5b949c4, 0x7741375a,
      0xa09dd89b, 0xd71ae460, 0xcca469f4, 0x6a759117, 0xb003a6fe, 0x0577dda0,
  };
  return std::make_pair(encode_table[idx], encode_table[idx + 1] & 0x07ffffffUL);
}

//
uint64_t
rotate(uint64_t n, uint64_t n_sh)
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
  code = (code & 0xffffffffUL) << 32 | ((code >> 32) & 0x07ffffffUL) | (s << (32 - 5));

  return code;
}

//
uint32_t
decode(uint64_t code)
{
  uint64_t num = (code & 0x07ffffffUL) << 32 | ((code >> 32) & 0xffffffffUL);
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