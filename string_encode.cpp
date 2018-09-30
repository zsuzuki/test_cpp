//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#include <cstdint>
#include <iostream>

#include "string_encode.h"

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << argv[0] << ": strings..." << std::endl;
    return 1;
  }

  for (int i = 1; i < argc; i++)
  {
    auto* v = argv[i];
    std::cout << v << std::endl;
    auto code = SimpleStringEncode::encode(v);
    auto str  = SimpleStringEncode::decode(code);
    std::cout << std::hex << code << "/" << str << std::endl;
  }

  return 0;
}
