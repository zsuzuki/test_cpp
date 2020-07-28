//

#include <cstdint>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

enum class Comp : int
{
  EQ,
  GT,
  LT
};
enum class Type : int
{
  Point,
  Number,
  Valid,
};

struct Info
{
  Comp  comp;
  Type  type;
  float value = 0;
};

struct State
{
  int  point  = 20;
  int  number = 50;
  bool valid  = true;
};

struct Base
{
  const Info& info;
  Base(const Info& i) : info(i) {}
  virtual ~Base() = default;

  virtual bool operator()(const State& s) const { return false; }
};

template <class F>
struct Check : public Base
{
  const F func;
  Check(const Info& i, const F f) : Base(i), func(f) {}

  bool operator()(const State& s) const override
  {
    float n = func(s);
    switch (info.comp)
    {
    case Comp::GT:
      std::cout << n << " > " << info.value << std::endl;
      return n > info.value;
    case Comp::LT:
      std::cout << n << " < " << info.value << std::endl;
      return n < info.value;
    default:
      break;
    }
    std::cout << n << " = " << info.value << std::endl;
    return n == info.value;
  }
};

template <class F>
std::unique_ptr<Check<F>>
make_ptr(const Info& i, const F f)
{
  return std::make_unique<Check<F>>(i, f);
}

int
main(int argc, char** argv)
{
  std::vector<Info> info_list = {
      {Comp::GT, Type::Number, 10.0f},
      {Comp::LT, Type::Point, 200.0f},
      {Comp::EQ, Type::Valid, 0.0f},
  };
  std::list<std::unique_ptr<Base>> vlist;

  for (auto& info : info_list)
  {
    switch (info.type)
    {
    case Type::Number:
      vlist.push_back(std::move(make_ptr(info, [](const State& s) { return s.point; })));
      break;
    case Type::Point:
      vlist.push_back(std::move(make_ptr(info, [](auto s) { return s.number; })));
      break;
    case Type::Valid:
      vlist.push_back(std::move(make_ptr(info, [](auto s) { return s.valid; })));
      break;
    }
  }

  State state;
  for (auto& v : vlist)
  {
    auto& e = *v;
    std::cout << "  " << (e(state) ? "OK" : "NG") << std::endl;
  }

  return 0;
}
