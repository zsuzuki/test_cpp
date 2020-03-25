#include <algorithm>
#include <array>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

namespace
{
//
std::string
calc_md5(std::string path)
{
  using boost::uuids::detail::md5;

  std::ifstream infile(path, std::ios::binary);
  md5           hash;
  while (infile.eof() == false)
  {
    std::array<char, 1024> buff;
    infile.read(buff.data(), buff.size());
    auto nb = infile.gcount();
    hash.process_bytes(buff.data(), nb);
  }
  md5::digest_type digest;
  hash.get_digest(digest);
  char md5string[64];
  std::snprintf(md5string, sizeof(md5string), "%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3]);
  return md5string;
}
} // namespace

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "need filename." << std::endl;
    return -1;
  }

  std::string fname = argv[1];
  auto        hash  = calc_md5(fname);
  std::cout << "MD5 (" << fname << ") = " << hash << std::endl;

  return 0;
}
