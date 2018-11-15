//
//
//
#pragma once

#include <thread>
#include <vector>
#include <zlib.h>

namespace ZLIB
{
constexpr size_t operator"" _MB(unsigned long long n) { return n * 1024 * 1024; }

class input
{
public:
  virtual void*  getSource()             = 0;
  virtual void*  getDestination()        = 0;
  virtual size_t getRemainSize() const   = 0;
  virtual size_t getWritableSize() const = 0;
  //
  virtual bool runOut(size_t read_size, size_t write_size) = 0;
  virtual void complete(bool ok, size_t write_size)        = 0;
  //
  virtual void error() = 0;
};

class zlib
{
  z_stream z;
  size_t   work_size = 0;

  static zlib* toself(void* opq) { return reinterpret_cast<zlib*>(opq); }

  void reset() { work_size = 0; }
  //
  template <typename F>
  size_t mainloop(input& inp, F func)
  {
    size_t total_write = 0;
    int    flush       = Z_NO_FLUSH;
    for (;;)
    {
      auto read_size  = inp.getRemainSize();
      auto write_size = inp.getWritableSize();
      z.next_in       = (Bytef*)inp.getSource();
      z.avail_in      = read_size;
      z.next_out      = (Bytef*)inp.getDestination();
      z.avail_out     = write_size;

      auto r = func(&z, flush);
      read_size -= z.avail_in;
      write_size -= z.avail_out;
      total_write += write_size;

      if (r == Z_STREAM_END)
        break;
      if (r == Z_STREAM_ERROR)
      {
        // error
        inp.error();
        break;
      }

      if (inp.runOut(read_size, write_size))
        flush = Z_FINISH;
    }
    return total_write;
  }

public:
  zlib()
  {
    z.zalloc = [](auto opq, auto items, auto nb) {
      auto*  self = toself(opq);
      size_t sz   = items * nb;
      self->work_size += sz;
      return malloc(sz);
    };
    z.zfree  = [](auto, auto ptr) { free(ptr); };
    z.opaque = this;
  }
  ~zlib() = default;

  size_t getWorkSize() const { return work_size; }

  //
  void compress(input& inp)
  {
    reset();
    deflateInit2(&z, Z_BEST_SPEED, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
    auto total_write = mainloop(inp, deflate);
    inp.complete(deflateEnd(&z) == Z_OK, total_write);
  }

  //
  void uncompress(input& inp)
  {
    reset();
    inflateInit2(&z, 47);
    auto total_write = mainloop(inp, inflate);
    inp.complete(inflateEnd(&z) == Z_OK, total_write);
  }
};

} // namespace ZLIB