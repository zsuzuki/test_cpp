#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iomanip>
#include <iostream>

int
main(int argc, char** argv)
{
  auto uuid = boost::uuids::random_generator()();
  std::cout << "uuid version: " << uuid.version() << std::endl;
  std::cout << std::hex << "---" << std::endl;
  std::cout << "ITER:";
  for (auto& c : uuid)
  {
    std::cout << (int)c;
  }
  std::cout << std::dec << std::endl;
  std::string s;
  s = boost::lexical_cast<std::string>(uuid);
  std::cout << "CAST:" << s << std::endl;
  return 0;
}
