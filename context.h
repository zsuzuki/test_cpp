//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#pragma once

#include <map>
#include <memory>
#include <cstring>
#include <iostream>

//
class Context
{
  class Module
  {
  public:
    virtual ~Module() = default;
  };
  using ModPtr       = std::shared_ptr<Module>;
  using LabelCompare = bool (*)(const char*, const char*);
  using LabelMap     = std::map<const char*, ModPtr, LabelCompare>;
  LabelMap mod_list{[](auto* a, auto* b) { return std::strcmp(a, b) < 0; }};

public:
  // 登録(引数有りコンストラクタ)
  template <class T, class... Args>
  std::shared_ptr<T> create(const char* name, Args... a)
  {
    struct M : public T, Module
    {
      M(Args... a) : T(a...) { std::cout << "Module sizeof:" << sizeof(M) << std::endl; }
    };
    auto Tptr      = std::make_shared<M>(a...);
    mod_list[name] = Tptr;
    return Tptr;
  }
  // 登録(引数無しコンストラクタ)
  template <class T>
  std::shared_ptr<T> create(const char* name)
  {
    struct M : public T, Module
    {
      M() { std::cout << "Module sizeof:" << sizeof(M) << std::endl; }
    };
    auto Tptr      = std::make_shared<M>();
    mod_list[name] = Tptr;
    return Tptr;
  }
  // 取得
  template <class T>
  std::shared_ptr<T> get(const char* name)
  {
    auto p = mod_list[name];
    if (p)
    {
      return std::dynamic_pointer_cast<T>(p);
    }
    return nullptr;
  }
};
