//
// algorithm from wikipedia
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
//
#pragma once

#include <cinttypes>

namespace fnv1
{

constexpr uint64_t prime        = 0x100000001b3;
constexpr uint64_t offset_basis = 0xcbf29ce484222325;

template <class T>
uint64_t
generate(T value)
{
  auto hash = offset_basis;
  auto len  = sizeof(T);
  auto num  = (uint64_t)(value);
  for (uint64_t i = 0; i < len; i++)
  {
    hash ^= num & 0xff;
    hash *= prime;
    num >>= 8;
  }
  return hash;
}

} // namespace fnv1
