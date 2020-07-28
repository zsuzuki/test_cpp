#include <boost/filesystem.hpp>
#include <boost/range/istream_range.hpp>
#include <iostream>

namespace fs = boost::filesystem;

int
main(int argc, char** argv)
{
  if (argc < 1)
  {
    std::cout << "need path" << std::endl;
    return 1;
  }

  for (int i = 1; i < argc; i++)
  {
    fs::path p    = argv[i];
    auto     rstr = p.generic_string();
    auto     rlen = rstr.length();
    auto     dir  = boost::make_iterator_range(fs::recursive_directory_iterator(p), {});
    for (const auto& e : dir)
    {
      if (!fs::is_directory(e))
      {
        auto path = e.path();
        auto pstr = path.generic_string();
        auto len  = pstr[rlen] == '/' ? rlen + 1 : rlen;
        auto rels = pstr.substr(len);
        std::cout << path << ":\n"
                  << "\tREL:\t" << path.relative_path() << "\n"
                  << "\tREL2:\t" << rels << "\n"
                  << "\tFILE:\t" << path.filename() << "\n"
                  << "\tROOT:\t" << path.root_path() << "\n"
                  << "\tROOT:\t" << path.root_directory() << "\n"
                  << "\tPAR:\t" << path.parent_path() << "\n"
                  << "\tRN:\t" << path.root_name() << "\n"
                  << std::endl;
      }
    }
  }

  return 0;
}
