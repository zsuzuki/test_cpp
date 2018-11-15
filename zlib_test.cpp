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
  buff           src;
  buff           dst;
  size_t         wsz;
  std::ofstream& ofstream;

public:
  zinp(const buff& i, const buff& o, std::ofstream& ofs) : src(i), dst(o), wsz(0), ofstream(ofs) {}
  void*  getSource() override { return src.ptr; }
  void*  getDestination() override { return dst.ptr; }
  size_t getRemainSize() const override { return src.sz; }
  size_t getWritableSize() const override { return dst.sz; }
  bool   runOut(size_t rsize, size_t wsize) override
  {
    ofstream.write((char*)dst.ptr, wsize);
    src.update(rsize);
    std::cout << "read: " << rsize << ", write: " << wsize << std::endl;
    return src.sz == 0;
  }
  void flush(size_t wsize) override
  {
    ofstream.write((char*)dst.ptr, wsize);
    std::cout << "flush: " << wsize << std::endl;
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

  std::array<Bytef, 4096> obuffer;

  ZLIB::zlib zlib;
  zinp::buff isrc{ibuffer}, idst{obuffer};
  zinp       zi{isrc, idst, ofs};
  if (uncompress)
    zlib.uncompress(zi);
  else
    zlib.compress(zi);
  std::cout << "work-size: " << zlib.getWorkSize() << std::endl;

  return 0;
}
//
// End
//
