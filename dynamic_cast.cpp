
#include "context.h"
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace
{
//
// test struct
//
struct Test
{
  std::string name;
  std::string msg;
  Test(std::string n, std::string m) : name(n), msg(m) { std::cout << "sizeof(Test):" << sizeof(Test) << std::endl; }
  virtual ~Test() { std::cout << "destruct: " << name << std::endl; }
  const char* get() const { return msg.c_str(); }
};
struct Test2 : public Test
{
  Test2(std::string n, std::string m) : Test(n, m) { std::cout << "test2" << std::endl; }
  ~Test2() { std::cout << "Test2" << std::endl; }
};
struct Sample
{
  std::string msg;
  Sample(std::string m) : msg(m) { std::cout << "sizeof(Sample):" << sizeof(Sample) << std::endl; }
  virtual ~Sample() { std::cout << "destruct sample" << std::endl; }
  const char* get() const { return msg.c_str(); }
};
struct NoArgs
{
  NoArgs() { std::cout << "sizeof(NoArgs):" << sizeof(NoArgs) << std::endl; }
  ~NoArgs() { std::cout << "destruct noargs" << std::endl; }
  const char* get() const { return "no args"; }
};

// 存在チェック
template <class T>
void
check(Context& ctx, const char* n)
{
  auto t = ctx.get<T>(n);
  if (t)
    std::cout << n << "=" << t->get() << std::endl;
  else
    std::cout << "no " << n << std::endl;
}

} // namespace

int
main()
{
  using namespace std::literals::string_literals;
  Context ctx;
  auto    t0 = ctx.create<Test>("test1", u8"Hello"s, u8"World"s);
  ctx.create<Sample>("sample1", u8"Sample"s);
  ctx.create<Test2>("test2", u8"pointer"s, u8"gogo"s);
  ctx.create<NoArgs>("args1");
  std::cout << "start: " << t0->get() << std::endl;
  check<Test>(ctx, "test1");
  check<Test>(ctx, "test2");
  check<Test>(ctx, "test3");
  check<Test>(ctx, "sample1");
  check<Sample>(ctx, "sample1");
  check<Sample>(ctx, "args1");
  check<NoArgs>(ctx, "args1");
  check<NoArgs>(ctx, "args2");
  return 0;
}
