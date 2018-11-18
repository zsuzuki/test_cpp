//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#pragma once

#include "atomic_queue.h"
#include <atomic>
#include <thread>
#include <vector>

// ワーカースレッド呼び出し
class WorkerThread
{
private:
  //
  using work_size_t        = uint16_t;
  using atomic_work_size_t = std::atomic<work_size_t>;
  using count_t            = std::atomic_int;

  // ワーカー
  class Worker
  {
  public:
    virtual ~Worker()      = default;
    virtual void     run() = 0;
    virtual count_t* getCounter() { return nullptr; }
  };
  class CWorker : public Worker
  {
    count_t& count;

  public:
    CWorker(count_t& c) : count(c) {}
    count_t* getCounter() override { return &count; }
  };

  //
  std::atomic_bool         enabled{true};
  std::atomic_int          launch{0};
  std::atomic_int          exec_count{0};
  std::vector<uint8_t>     buffer;
  atomic_work_size_t       buffer_index{0};
  Queue<Worker>            queue;
  std::vector<std::thread> thread_list;
  
  //
  void add_atomic(std::atomic_int& i, int a)
  {
    int c, n;
    do
    {
      c = i;
      n = c + a;
    } while (i.compare_exchange_weak(c, n) == false);
  }

  // スレッド本体
  void execute()
  {
    add_atomic(launch, 1);
    do
    {
      add_atomic(exec_count, 1);
      auto* p = queue.pop();
      if (p)
      {
        auto* c = p->getCounter();
        if (c == nullptr || *c <= 0)
          p->run();
        else
          add(p);
      }
      else
      {
        std::this_thread::sleep_for(std::chrono::microseconds(0));
      }
      add_atomic(exec_count, -1);
    } while (enabled);
  }

  // ワーカー用メモリ確保
  std::pair<void*, work_size_t> alloc(size_t sz)
  {
    work_size_t next, cidx;
    do
    {
      cidx = buffer_index;
      next = cidx + sz;
      if (next > buffer.size())
      {
        // no memory
        return std::make_pair(nullptr, 0);
      }
    } while (buffer_index.compare_exchange_weak(cidx, next) == false);

    auto buff = reinterpret_cast<void*>(&buffer[cidx]);
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

  // ワーカー生成
  template <typename F>
  bool registerWorker(size_t sz, F func)
  {
    auto r = alloc(sz);
    if (r.first)
    {
      add(func(r.first));
      return true;
    }
    return false;
  }

public:
  WorkerThread(size_t bsz = 4096) : queue(bsz / 32)
  {
    auto max_th = std::max(1U, std::thread::hardware_concurrency() - 1);
    thread_list.resize(max_th);
    for (int id = 0; id < max_th; id++)
    {
      auto& th = thread_list[id];
      th       = std::thread([&] { execute(); });
    }

    buffer.resize(bsz);
    while (launch < max_th)
      std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  virtual ~WorkerThread()
  {
    enabled = false;
    for (auto& th : thread_list)
    {
      th.join();
    }
  }

  // ワーカーをもう一度使えるようにする(checkCompleteで確認した後に呼ぶ)
  void clear() { buffer_index = 0; }

  // 全てのワーカーが終了しているか
  bool checkComplete() const { return exec_count == 0 && queue.empty(); }

  // 登録実行(引数付き)
  template <typename F, typename A>
  bool push(F f, A a)
  {
    class m : public Worker
    {
      F func;
      A arg;

    public:
      m(F f, A a) : func(f), arg(a) {}
      void run() override { func(arg); }
    };
    return registerWorker(sizeof(m), [&](auto* p) { return new (p) m{f, a}; });
  }
  template <typename F, typename A>
  bool push(count_t& c, F f, A a)
  {
    class m : public CWorker
    {
      F func;
      A arg;

    public:
      m(count_t& c, F f, A a) : CWorker(c), func(f), arg(a) {}
      void run() override { func(arg); }
    };
    return registerWorker(sizeof(m), [&](auto* p) { return new (p) m{c, f, a}; });
  }
  // 登録実行(引数無し)
  template <typename F>
  bool push(F f)
  {
    class m : public Worker
    {
      F func;

    public:
      m(F f) : func(f) {}
      void run() override { func(); }
    };
    return registerWorker(sizeof(m), [&](auto* p) { return new (p) m{f}; });
  }
  template <typename F>
  bool push(count_t& c, F f)
  {
    class m : public CWorker
    {
      F func;

    public:
      m(count_t& c, F f) : CWorker(c), func(f) {}
      void run() override { func(); }
    };
    return registerWorker(sizeof(m), [&](auto* p) { return new (p) m{c, f}; });
  }
};
