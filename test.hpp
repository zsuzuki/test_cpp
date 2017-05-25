//
//
#pragma once

#include <iostream>
#include <string>

namespace Test
{
class Dump
{
  int id = 0;

public:
  void put(std::string msg)
  {
    std::cout << id << ":" << msg << std::endl;
    id++;
  }
};

} // namespace Test
//
