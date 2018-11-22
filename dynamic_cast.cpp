
#include "string_encode.h"
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace
{
//
class Context
{
  class Module
  {
  public:
    virtual ~Module() = default;
  };
  using ModPtr       = std::shared_ptr<Module>;
  using LabelCompare = bool (*)(const char*, const char*);
  using LabelMap     = std::map<const char*, ModPtr, LabelCompare>;
  LabelMap mod_list{[](auto* a, auto* b) { return strcmp(a, b) < 0; }};

public:
  // 登録(引数有りコンストラクタ)
  template <class T, class... Args>
  std::shared_ptr<T> create(const char* name, Args... a)
  {
    struct M : public T, Module
    {
      M(Args... a) : T(a...) { std::cout << "sizeof:" << sizeof(M) << std::endl; }
    };
    auto Tptr      = std::make_shared<M>(a...);
    mod_list[name] = Tptr;
    return Tptr;
  }
  // 登録(引数無しコンストラクタ)
  template <class T>
  std::shared_ptr<T> create(const char* name)
  {
    struct M : public T, Module
    {
      M() { std::cout << "sizeof:" << sizeof(M) << std::endl; }
    };
    auto Tptr      = std::make_shared<M>();
    mod_list[name] = Tptr;
    return Tptr;
  }
  // 取得
  template <class T>
  std::shared_ptr<T> get(const char* name)
  {
    auto p = mod_list[name];
    if (p)
    {
      return std::dynamic_pointer_cast<T>(p);
    }
    return nullptr;
  }
};

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
  ctx.create<Test>("test2", u8"pointer"s, u8"gogo"s);
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
