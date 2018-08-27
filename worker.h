//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#pragma once

#include "atomic_queue.h"
#include <atomic>
#include <thread>
#include <vector>

using work_size_t = uint16_t;

// ワーカー
class Worker
{
public:
  virtual ~Worker()  = default;
  virtual void run() = 0;

  virtual work_size_t getNextBuffer() const = 0;
};

// ワーカースレッド呼び出し
class WorkerThread
{
  using atomic_work_size_t = std::atomic<work_size_t>;

  bool                     enabled{true};
  std::atomic_int          launch{0};
  std::vector<uint8_t>     buffer;
  atomic_work_size_t       index{0};
  atomic_work_size_t       free_index{0};
  std::atomic_bool         buffer_full{false};
  Queue<Worker>            queue;
  std::vector<std::thread> thread_list;

  // スレッド本体
  void execute(int index)
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
  }

  // ワーカー用メモリ確保
  std::pair<void*, work_size_t> alloc(size_t sz)
  {
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
  WorkerThread(size_t bsz = 200) : queue(bsz / 32)
  {
    auto max_th = std::max(1U, std::thread::hardware_concurrency() - 1);
    thread_list.resize(max_th);
    for (int id = 0; id < max_th; id++)
    {
      auto& th = thread_list[id];
      th       = std::thread([&, id] { execute(id); });
    }

    buffer.resize(bsz);
    while (launch < max_th)
      std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  ~WorkerThread()
  {
    enabled = false;
    for (auto& th : thread_list)
    {
      th.join();
    }
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
