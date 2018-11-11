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

  // ワーカー
  class Worker
  {
  public:
    virtual ~Worker()  = default;
    virtual void run() = 0;
  };

  //
  std::atomic_bool         enabled{true};
  std::atomic_int          launch{0};
  std::atomic_int          exec_count{0};
  std::vector<uint8_t>     buffer;
  atomic_work_size_t       buffer_index{0};
  Queue<Worker>            queue;
  std::vector<std::thread> thread_list;

  // スレッド本体
  void execute()
  {
    launch++;
    do
    {
      auto* p = queue.pop();
      if (p)
      {
        exec_count++;
        p->run();
        --exec_count;
      }
      else
      {
        std::this_thread::sleep_for(std::chrono::microseconds(0));
      }
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
      ~m() override = default;
      void run() override { func(arg); }
    };
    auto r = alloc(sizeof(m));
    if (r.first)
    {
      add(new (r.first) m{f, a});
      return true;
    }
    return false;
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
      ~m() override = default;
      void run() override { func(); }
    };
    auto r = alloc(sizeof(m));
    if (r.first)
    {
      add(new (r.first) m{f});
      return true;
    }
    return false;
  }
};
