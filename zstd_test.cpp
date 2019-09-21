#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>
#include <zstd.h>

class ex : public std::exception
{
  std::string msg;

public:
  ex(std::string m) : msg(m) {}
  ~ex() = default;
  const char* what() const _NOEXCEPT override { return msg.c_str(); }
};

//
int
main(int argc, const char** args)
{
  try
  {
    if (argc < 2)
      throw ex{"no input file: <infile>"};

    auto fname = std::string{args[1]};
    auto infst = std::ifstream(fname);
    if (!infst)
      throw ex{"input file open failed: " + fname};
    // input file read
    infst.seekg(0, std::ios::end);
    auto insz     = infst.tellg();
    auto readbuff = std::vector<char>(insz);
    infst.seekg(0);
    infst.read(readbuff.data(), insz);

    // compress
    auto cbsz  = ZSTD_compressBound(insz);
    auto cbuff = std::vector<char>(cbsz);
    auto csz   = ZSTD_compress(cbuff.data(), cbsz, readbuff.data(), insz, 3);
    std::cout << "F:" << fname << ":" << insz << "->" << csz << std::endl;

    // decompress
    auto dbsz  = ZSTD_getFrameContentSize(cbuff.data(), csz);
    auto dbuff = std::vector<char>(dbsz);
    auto dsz   = ZSTD_decompress(dbuff.data(), dbsz, cbuff.data(), csz);
    std::cout << "O: " << dbsz << "/" << dsz << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
//
