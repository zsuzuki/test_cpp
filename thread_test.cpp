//
// threadテスト
//
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <stdio.h>
#include <thread>

// async呼び出し
int
mul(int a, int b)
{
  return a * b;
}

using work_size_t = uint16_t;
// ワーカー
class Worker
{
public:
  virtual ~Worker()  = default;
  virtual void run() = 0;

  virtual work_size_t getNextBuffer() const = 0;
};

template <class T>
class Queue
{
  std::vector<T*> list;
  std::atomic_int start{0};
  std::atomic_int end{0};

public:
  Queue(size_t sz)
  {
    list.resize(sz);
    for (auto& p : list)
      p = nullptr;
  }

  bool push(T* p)
  {
    int n, e = end;
    do
    {
      n = (e + 1) % list.size();
      if (n == start)
        return false;
    } while (end.compare_exchange_weak(e, n) == false);
    list[e] = p;
    return true;
  }

  T* pop()
  {
    int s = start;
    if (s == end)
      return nullptr;
    int n = (s + 1) % list.size();
    if (start.compare_exchange_weak(s, n) == false)
      return nullptr;

    volatile auto p = list[s];
    while (p == nullptr)
      p = list[s];

    list[s] = nullptr;
    return p;
  }
  bool empty() const { return start == end; }
};

// ワーカースレッド呼び出し
class WorkerThread
{
  using atomic_work_size_t = std::atomic<work_size_t>;

  bool                 enabled{true};
  std::atomic_int      launch{0};
  std::vector<uint8_t> buffer;
  atomic_work_size_t   index{0};
  atomic_work_size_t   free_index{0};
  std::atomic_bool     buffer_full{false};
  Queue<Worker>        queue;
  std::thread          th;
  std::thread          th2;

  // スレッド本体
  void execute()
  {
    launch++;
    int count = 0;
    do
    {
      auto* p = queue.pop();
      if (p)
      {
        p->run();
        free_index = p->getNextBuffer();
        if (free_index != index)
          buffer_full = false;
        count++;
      }
    } while (enabled);
    printf("thread finish: %d\n", count);
  }

  // ワーカー用メモリ確保
  std::pair<void*, work_size_t> alloc(size_t sz)
  {
    // printf("request: %zu index: %d free:%d\n",sz,index.load(),free_index.load());
    work_size_t next, target = index;
    while (buffer_full)
      ;
    do
    {
      next = target + sz;
      if (target >= free_index)
      {
        if (next > buffer.size())
        {
          while (sz > free_index)
            ;
          next = sz;
        }
      }
      else
      {
        while (next > free_index)
          ;
      }
    } while (index.compare_exchange_weak(target, next) == false);

    if (target > next)
      target = 0;
    if (next == free_index)
      buffer_full = true;

    auto buff = reinterpret_cast<void*>(&buffer[target]);
    // printf("index: %d(%d) / sizeof %zu free: %d%s\n", target, next, sz, free_index.load(), buffer_full ? "(FULL)" : "");
    return std::make_pair(buff, next);
  }

  // キューに積む
  void add(Worker* w)
  {
    bool loop = true;
    do
    {
      loop = !queue.push(w);
    } while (loop);
  }

public:
  WorkerThread(size_t bsz = 200) : queue(bsz / 32), th([&] { execute(); }), th2([&] { execute(); })
  {
    buffer.resize(bsz);
    while (launch < 2)
      std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
  ~WorkerThread()
  {
    enabled = false;
    th.join();
    th2.join();
  }

  // 登録実行(引数付き)
  template <typename F, typename A>
  void push(F f, A a)
  {
    class m : public Worker
    {
      F           func;
      A           arg;
      work_size_t next;

    public:
      m(F f, A a, work_size_t n) : func(f), arg(a), next(n) {}
      ~m() override = default;
      void        run() override { func(arg); }
      work_size_t getNextBuffer() const override { return next; }
    };
    auto r = alloc(sizeof(m));
    add(new (r.first) m{f, a, r.second});
  }
  // 登録実行(引数無し)
  template <typename F>
  void push(F f)
  {
    class m : public Worker
    {
      F           func;
      work_size_t next;

    public:
      m(F f, work_size_t n) : func(f), next(n) {}
      ~m() override = default;
      void        run() override { func(); }
      work_size_t getNextBuffer() const override { return next; }
    };
    auto r = alloc(sizeof(m));
    add(new (r.first) m{f, r.second});
  }
};

//
//
//
int
main()
{
  struct T
  {
    int a = 5;
    int b = 7;
  };

  printf("Hello\n");
  int  a = 8;
  int  b = 3;
  int  c = 12;
  auto t = std::async(std::launch::async, mul, a, b);

  WorkerThread th;

  printf("threads: %d\n", std::thread::hardware_concurrency());
  th.push([](const char* p) { printf("%s\n", p); }, "worker one.");
  th.push([](int n) { printf("%d\n", n); }, 9);
  th.push([](const T& t) { printf("%d/%d\n", t.a, t.b); }, T());
  th.push([&]() { printf("lambda:%d,%d\n", b, c); });
  c = a + b;

  for (int i = 0; i <= 100; ++i)
  {
    if ((i % 10) == 0)
    {
      th.push(
          [i, &th](int d) {
            printf("ten : %d %d\n", d, i);
            th.push([i] { printf("chain job %d\n", i); });
          },
          i / 10);
      printf("ten counter\n");
    }
    else if (i & 1)
      th.push([i]() { printf("odd : %d\n", i); });
    else
      th.push([i]() { printf("even: %d\n", i); });
  }

  printf("result:%d/%d\n", t.get(), c);

  printf("Good-bye.\n");
  return 0;
}
// End
