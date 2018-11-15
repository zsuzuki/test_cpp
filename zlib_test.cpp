//
//
//

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
  buff   src;
  buff   dst;
  size_t wsz;

public:
  zinp(const buff& i, const buff& o) : src(i), dst(o), wsz(0) {}
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
  void complete(bool ok, size_t tws) override
  {
    std::cout << "write: " << tws << ", " << (ok ? "SUCCESS" : "FAILES") << std::endl;
    wsz = tws;
  }
  void error() override { std::cerr << "error!" << std::endl; }

  size_t getTotalSize() const { return wsz; }
};
} // namespace

//
int
main(int argc, char** argv)
{
  std::string ifname, ofname;

  bool uncompress = false;
  for (int i = 1; i < argc; i++)
  {
    std::string a = argv[i];
    if (a == "-d")
      uncompress = true;
    else if (ifname.empty())
      ifname = a;
    else if (ofname.empty())
      ofname = a;
  }
  if (ifname.empty() || ofname.empty())
  {
    std::cerr << "no input filename." << std::endl;
    return 1;
  }

  std::ifstream ifs{ifname, std::ios::binary | std::ios::ate};
  if (ifs.good() == false)
  {
    std::cerr << "open error<input>: " << ifname << std::endl;
    return 1;
  }
  std::ofstream ofs{ofname, std::ios::binary};
  if (ofs.good() == false)
  {
    std::cerr << "open error<output>: " << ofname << std::endl;
    return 1;
  }
  auto isz = ifs.tellg();
  ifs.seekg(0);
  std::cout << "input file size: " << isz << std::endl;
  std::vector<Bytef> ibuffer;
  ibuffer.resize(isz);
  ifs.read((char*)ibuffer.data(), ibuffer.size());

  std::vector<Bytef> obuffer;
  obuffer.resize(isz * 4);

  ZLIB::zlib zlib;
  zinp::buff isrc{ibuffer}, idst{obuffer};
  zinp       zi{isrc, idst};
  if (uncompress)
    zlib.uncompress(zi);
  else
    zlib.compress(zi);
  std::cout << "work-size: " << zlib.getWorkSize() << std::endl;
  ofs.write((char*)obuffer.data(), zi.getTotalSize());

  return 0;
}
//
// End
//
