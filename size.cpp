//
//
//
#include <cstdint>
#include <iostream>
#include <random>

namespace
{
struct A64
{
  int64_t a : 12;
  int64_t b : 8;
};
struct A32
{
  int32_t a : 12;
  int32_t b : 8;
};
struct ANR
{
  unsigned a : 12;
  unsigned b : 8;
};
} // namespace

int
main(int argc, char** argv)
{
  std::random_device rnd;

  A64 a64;
  A32 a32;
  ANR anr;

  a64.a = rnd();
  a64.b = rnd();
  a32.a = rnd();
  a32.b = rnd();
  anr.a = rnd();
  anr.b = rnd();
  std::cout << "A64: " << sizeof(A64) << "/" << a64.a << "," << a64.b << std::endl;
  std::cout << "A32: " << sizeof(A32) << "/" << a32.a << "," << a32.b << std::endl;
  std::cout << "AN : " << sizeof(ANR) << "/" << anr.a << "," << anr.b << std::endl;

  return 0;
}
