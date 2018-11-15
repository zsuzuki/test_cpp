//
//
//

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include "zlib.hpp"

namespace
{
unsigned char msg[] = "Hello,World\n"
                      "My name is POPOPOPO.\n"
                      "私の名前は中野です\n"
                      "play for futsal on today.\n"
                      "take your picture\n"
                      "明日は明日の風が吹く\n"
                      "convert on server\n"
                      "put on your mark\n"
                      "get set\n"
                      "ready\n"
                      "go";

class zinp : public ZLIB::input
{
public:
  struct buff
  {
    Bytef* ptr;
    size_t sz;

    buff() {}
    buff(const buff& b)
    {
      ptr = b.ptr;
      sz  = b.sz;
    }
    buff(Bytef* b, size_t s) : ptr(b), sz(s) {}
    template <class T>
    buff(T& v)
    {
      ptr = v.data();
      sz  = v.size();
    }

    void update(size_t s)
    {
      ptr += s;
      sz -= s;
    }
  };

private:
  buff src;
  buff dst;

public:
  zinp(const buff& i, const buff& o) : src(i), dst(o) {}
  void*  getSource() override { return src.ptr; }
  void*  getDestination() override { return dst.ptr; }
  size_t getRemainSize() const override { return src.sz; }
  size_t getWritableSize() const override { return dst.sz; }
  bool   runOut(size_t rsize, size_t wsize) override
  {
    src.update(rsize);
    dst.update(wsize);
    std::cout << "read: " << rsize << ", write: " << wsize << std::endl;
    return src.sz == 0;
  }
  void complete(bool ok, size_t tws) override { std::cout << "write: " << tws << std::endl; }
  void error() override { std::cerr << "error!" << std::endl; }
};
} // namespace

//
int
main(int argc, char** argv)
{
  using Buffer = std::array<Bytef, 1024>;
  Buffer     buffer;
  Buffer     testbuff;
  ZLIB::zlib zlib;
  zinp::buff isrc{msg, sizeof(msg)}, idst{buffer};
  zinp       zi{isrc, idst};
  zlib.compress(zi);
  std::cout << "deflate work-size: " << zlib.getWorkSize() << std::endl;

  zinp::buff osrc{buffer}, odst{testbuff};
  zinp       zo{osrc, odst};
  zlib.uncompress(zo);
  std::cout << "inflate work-size: " << zlib.getWorkSize() << std::endl;

  std::cout << testbuff.data() << std::endl;
  std::cout << "done." << std::endl;

  return 0;
}
//
// End
//
