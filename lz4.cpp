#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <future>
#include <iostream>
#include <lz4.h>
#include <thread>
#include <vector>

namespace
{
constexpr size_t BLOCK_BYTES = 8 * 1024;
using BLOCK                  = std::array<char, LZ4_COMPRESSBOUND(BLOCK_BYTES)>;
using atmint                 = std::atomic_int;

struct WriteInfo
{
  char*      buff_;
  int        size_;
  std::mutex lock_;

  WriteInfo() = default;
  WriteInfo(const WriteInfo& o) { *this = o; }
  WriteInfo& operator=(const WriteInfo& o)
  {
    buff_ = o.buff_;
    size_ = o.size_;
    return *this;
  }
};

void
test_write(std::ofstream& ofst, WriteInfo& wi)
{
  std::lock_guard g(wi.lock_);
  ofst.write((const char*)&wi.size_, sizeof(wi.size_));
  ofst.write(wi.buff_, wi.size_);
}

void
test_compress(std::ofstream& ofst, std::ifstream& ifst)
{
  LZ4_stream_t  lz4Stream_body;
  LZ4_stream_t* lz4Stream = &lz4Stream_body;

  std::vector<BLOCK> inpBuf;
  std::vector<BLOCK> outBuf;
  inpBuf.resize(2);
  outBuf.resize(40);
  int    inpBufIndex = 0;
  int    outBufIndex = 0;
  atmint inPage      = -1;
  atmint outPage     = 0;

  std::vector<WriteInfo> wrlist;
  wrlist.resize(40);

  auto ntime = std::chrono::steady_clock::now();
  LZ4_initStream(lz4Stream, sizeof(*lz4Stream));

  auto f = std::async(std::launch::async, [&]() {
    while (inPage.load() != -2)
    {
      while (outPage.load() > inPage.load())
        ;
      int   page  = outPage.load() % outBuf.size();
      auto& winfo = wrlist[page];
      test_write(ofst, winfo);
      outPage++;
    }
    std::cout << "done" << std::endl;
  });

  for (;;)
  {
    int         pn     = inpBufIndex % 2;
    char* const inpPtr = inpBuf[pn].data();
    ifst.read(inpPtr, BLOCK_BYTES);
    const int inpBytes = ifst.gcount();
    if (0 == inpBytes)
      break;

    int       opage    = outBufIndex % outBuf.size();
    auto&     winfo    = wrlist[opage];
    auto&     cmpBuf   = outBuf[opage];
    const int cmpBytes = LZ4_compress_fast_continue(lz4Stream, inpPtr, cmpBuf.data(), inpBytes, cmpBuf.size(), 1);
    if (cmpBytes <= 0)
      break;
    {
      std::lock_guard g(winfo.lock_);
      winfo.size_ = cmpBytes;
      winfo.buff_ = cmpBuf.data();
      inPage      = outBufIndex;
    }
    outBufIndex++;
    inpBufIndex++;
    while ((inPage.load() - outPage.load()) > 5)
      ;
  }
  while (inPage.load() != outPage.load())
    ;
  inPage = -2;
  f.wait();

  ofst << 0;
  auto etime        = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(etime - ntime);
  std::cout << "Elapsed time: " << elapsed_time.count() << std::endl;
}

void
test_decompress(std::ofstream& ofst, std::ifstream& ifst)
{
  LZ4_streamDecode_t  lz4StreamDecode_body;
  LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

  std::vector<BLOCK> decBuf;
  decBuf.resize(4);
  int decBufIndex = 0;

  LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

  for (;;)
  {
    char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
    int  cmpBytes = 0;

    {
      ifst.read((char*)&cmpBytes, sizeof(cmpBytes));
      const size_t readCount0 = ifst.gcount();
      if (readCount0 < 1 || cmpBytes <= 0)
      {
        break;
      }

      ifst.read(cmpBuf, cmpBytes);
      const size_t readCount1 = ifst.gcount();
      if (readCount1 != (size_t)cmpBytes)
      {
        break;
      }
    }

    {
      char* const decPtr   = decBuf[decBufIndex % decBuf.size()].data();
      const int   decBytes = LZ4_decompress_safe_continue(lz4StreamDecode, cmpBuf, decPtr, cmpBytes, BLOCK_BYTES);
      if (decBytes <= 0)
      {
        break;
      }
      ofst.write(decPtr, decBytes);
    }

    decBufIndex++;
  }
  std::cout << "done" << std::endl;
}

} // namespace

//
//
//
int
main(int argc, char** argv)
{
  auto help = []() {
    std::cerr << "lz4: [d] input output\n"
                 "\td\tdecompress"
              << std::endl;
  };

  if (argc < 3)
  {
    help();
    return 1;
  }
  try
  {
    std::string arg = argv[1];
    if (arg == "d")
    {
      // decompress
      if (argc < 4)
      {
        std::cout << "need output file" << std::endl;
        help();
        return 1;
      }
      std::ifstream ifst(argv[2], std::ios::binary);
      std::ofstream ofst(argv[3], std::ios::binary);
      test_decompress(ofst, ifst);
    }
    else
    {
      // compress
      std::ifstream ifst(argv[1], std::ios::binary);
      std::ofstream ofst(argv[2], std::ios::binary);
      test_compress(ofst, ifst);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}