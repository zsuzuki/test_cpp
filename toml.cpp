//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//

#include <cpptoml.h>
#include <iostream>
#include <string>

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << argv[0] << ": toml-file" << std::endl;
    return 1;
  }

  auto config      = cpptoml::parse_file(argv[1]);
  auto struct_name = config->get_as<std::string>("name");
  if (!struct_name)
  {
    std::cerr << "struct has no name" << std::endl;
    return 1;
  }

  auto includes = config->get_array_of<std::string>("include");
  for (const auto& inc : *includes)
  {
    std::cout << "#include <" << inc << ">" << std::endl;
  }

  std::cout << std::endl << "struct " << *struct_name << " {" << std::endl;

  auto contents = config->get_table_array("contents");
  for (const auto& table : *contents)
  {
    auto name = table->get_as<std::string>("name");
    auto type = table->get_as<std::string>("type");
    if (!name || !type)
    {
      std::cerr << "this content have no name." << std::endl;
      continue;
    }
    std::string line;
    auto        bits = table->get_as<int64_t>("bits");
    if (bits)
    {
      // bit field
      line = *type + " " + *name + ": ";
      line += std::to_string(*bits);
    }
    else
    {
      auto array = table->get_as<int64_t>("size");
      if (array)
      {
        // array
        line = *type + " " + *name + "[";
        line += std::to_string(*array);
        line += "]";
      }
      else
      {
        line = *type + " " + *name;
      }
    }
    std::cout << "\t" << line << ";" << std::endl;
  }
  std::cout << "};" << std::endl;

  return 0;
}
