#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "no input file" << std::endl;
    return 1;
  }
  std::string   fname{argv[1]};
  std::ifstream input_file{fname, std::ios::ate};
  if (input_file.good() == false)
  {
    std::cerr << "bad file: " << fname << std::endl;
    return 1;
  }

  size_t sz = input_file.tellg();
  input_file.seekg(0);
  std::vector<char> buffer;
  buffer.resize(sz);
  std::cout << "input: " << fname << "\nsize: " << sz << std::endl;
  input_file.read(buffer.data(), buffer.size());
  std::array<int, 256> count{};
  for (auto& ch : buffer)
  {
    count[ch]++;
  }
  for (size_t i = 0; i < count.size(); i++)
  {
    auto c = count[i];
    if (c > 0)
    {
      if (i >= 0x20 && i < 0x7f)
        std::cout << std::hex << i << "(" << (char)i << "):" << std::dec << (int)c << std::endl;
      else
        std::cout << std::hex << i << ":" << std::dec << (int)c << std::endl;
    }
  }

  return 0;
}
