//
// 指定メソッド呼び出しテスト
//
// Y.Suzuki
//

#include <iostream>
#include <list>

#include "test.hpp"

Test::Dump dump;

//
struct Context
{
  int phase;
  int count;
};

// listener base
class ListenerBase
{
public:
  virtual void callFunc(Context*) = 0;
};
// template Listener
template <class T, class Tc>
class Listener : public ListenerBase
{
  T* obj;
  void (T::*func)(Tc*);

public:
  Listener(T* o, void (T::*f)(Tc*))
  {
    obj  = o;
    func = f;
  }
  void callFunc(Tc* c) { (obj->*func)(c); }
};
// function list
class FunctionList
{
  std::list<ListenerBase*> l;

public:
  template <class T>
  void push(T* o, void (T::*f)(Context*))
  {
    l.push_back(new Listener<T, Context>(o, f));
  }
  void call(Context* c)
  {
    std::list<ListenerBase*>::iterator li = l.begin();
    while (li != l.end())
    {
      ListenerBase* lb = *li;
      lb->callFunc(c);
      li++;
    }
  }
};

//
// main method
//
using namespace std;

// local classes
class HexDisp
{
  int num;

public:
  HexDisp(int n) : num(n) {}
  void disp(Context* ctx) { cout << hex << (num + ctx->count) << endl; }
};
class DecDisp
{
  int num;

public:
  DecDisp(int n) : num(n) {}
  void disp(Context* ctx) { cout << dec << (num + ctx->count) << endl; }
};
class PhaseDisp
{
public:
  PhaseDisp() {}
  void disp(Context* ctx)
  {
    if (ctx->phase == 0)
      dump.put("Start!");
    else if (ctx->phase == 2)
    {
      dump.put("Finish!");
    }
  }
};

//
// start up routine
//
int
main(int argc, char* argv[])
{
  FunctionList fl;

  dump.put("setup");

  HexDisp   h(1234);
  DecDisp   d(0x2323);
  PhaseDisp p;
  fl.push(&p, &PhaseDisp::disp);
  fl.push(&h, &HexDisp::disp);
  fl.push(&d, &DecDisp::disp);

  // 10回登録メソッドを呼び出してみる
  for (int cnt = 0; cnt <= 10; cnt++)
  {
    Context ctx;
    ctx.phase = (cnt == 0) ? 0 : (cnt == 10) ? 2 : 1;
    ctx.count = cnt;
    fl.call(&ctx);
  }

  dump.put("done");
}
// End
