//
// Copyright 2018 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace SimpleStringEncode
{
// characters
enum class CHCODE : uint64_t
{
  C_SHORT = 0x00,
  C_LONG  = 0x20,
  // short(5bits)
  C_0 = C_SHORT,
  C_1,
  C_2,
  C_3,
  C_4,
  C_5,
  C_6,
  C_7,
  C_8,
  C_9,
  C_Hyphen,
  C_Plus,
  C_Slash,
  C_LBrace,
  C_RBrace,
  C_Colon,
  // long(6bits)
  C_A = C_LONG,
  C_B,
  C_C,
  C_D,
  C_E,
  C_F,
  C_G,
  C_H,
  C_I,
  C_J,
  C_K,
  C_L,
  C_M,
  C_N,
  C_O,
  C_P,
  C_Q,
  C_R,
  C_S,
  C_T,
  C_U,
  C_V,
  C_W,
  C_X,
  C_Y,
  C_Z,
  C_Question,
  C_Dot,
  C_AtMark,
  C_Sharp,
  C_Dollar,
  C_Ampersand,
};

//
// Encode:
// auto v = encode("y-suzuki");
// std::cout << "V=" << std::hex << v;
// ---
// V=e15969ce95500008
//
uint64_t
encode(const char* str)
{
  static std::map<char, const CHCODE> encode_map = {
      {'0', CHCODE::C_0},      {'1', CHCODE::C_1},        {'2', CHCODE::C_2},         {'3', CHCODE::C_3},
      {'4', CHCODE::C_4},      {'5', CHCODE::C_5},        {'6', CHCODE::C_6},         {'7', CHCODE::C_7},
      {'8', CHCODE::C_8},      {'9', CHCODE::C_9},        {'a', CHCODE::C_A},         {'b', CHCODE::C_B},
      {'c', CHCODE::C_C},      {'d', CHCODE::C_D},        {'e', CHCODE::C_E},         {'f', CHCODE::C_F},
      {'g', CHCODE::C_G},      {'h', CHCODE::C_H},        {'i', CHCODE::C_I},         {'j', CHCODE::C_J},
      {'k', CHCODE::C_K},      {'l', CHCODE::C_L},        {'m', CHCODE::C_M},         {'n', CHCODE::C_N},
      {'o', CHCODE::C_O},      {'p', CHCODE::C_P},        {'q', CHCODE::C_Q},         {'r', CHCODE::C_R},
      {'s', CHCODE::C_S},      {'t', CHCODE::C_T},        {'u', CHCODE::C_U},         {'v', CHCODE::C_V},
      {'w', CHCODE::C_W},      {'x', CHCODE::C_X},        {'y', CHCODE::C_Y},         {'z', CHCODE::C_Z},
      {'A', CHCODE::C_A},      {'B', CHCODE::C_B},        {'C', CHCODE::C_C},         {'D', CHCODE::C_D},
      {'E', CHCODE::C_E},      {'F', CHCODE::C_F},        {'G', CHCODE::C_G},         {'H', CHCODE::C_H},
      {'I', CHCODE::C_I},      {'J', CHCODE::C_J},        {'K', CHCODE::C_K},         {'L', CHCODE::C_L},
      {'M', CHCODE::C_M},      {'N', CHCODE::C_N},        {'O', CHCODE::C_O},         {'P', CHCODE::C_P},
      {'Q', CHCODE::C_Q},      {'R', CHCODE::C_R},        {'S', CHCODE::C_S},         {'T', CHCODE::C_T},
      {'U', CHCODE::C_U},      {'V', CHCODE::C_V},        {'W', CHCODE::C_W},         {'X', CHCODE::C_X},
      {'Y', CHCODE::C_Y},      {'Z', CHCODE::C_Z},        {'-', CHCODE::C_Hyphen},    {'+', CHCODE::C_Plus},
      {'/', CHCODE::C_Slash},  {'(', CHCODE::C_LBrace},   {')', CHCODE::C_RBrace},    {'[', CHCODE::C_LBrace},
      {']', CHCODE::C_RBrace}, {'{', CHCODE::C_LBrace},   {'}', CHCODE::C_RBrace},    {'<', CHCODE::C_LBrace},
      {'>', CHCODE::C_RBrace}, {'?', CHCODE::C_Question}, {'*', CHCODE::C_Dot},       {'@', CHCODE::C_AtMark},
      {'#', CHCODE::C_Sharp},  {'$', CHCODE::C_Dollar},   {'&', CHCODE::C_Ampersand}, {':', CHCODE::C_Colon},
      {'_', CHCODE::C_Hyphen}, {';', CHCODE::C_Colon},    {'.', CHCODE::C_Dot},       {',', CHCODE::C_Dot},
      {'=', CHCODE::C_Hyphen},
  };

  uint64_t r = 0, l = 0;
  int      bit = 64;

  for (int i = 0; str[i] != '\0'; i++)
  {
    auto c    = str[i];
    auto code = encode_map[c];
    auto b    = code >= CHCODE::C_LONG ? 6 : 5;
    if (bit - b >= 4)
    {
      bit -= b;
      r |= static_cast<uint64_t>(code) << bit;
      l++;
    }
  }
  if (l > 15)
  {
    l = 15;
  }
  return r | l;
}

//
// Decode:
// auto s = decode(0xe15969ce95500008ULL);
// std::cout << "S=" << s;
// ---
// S=y-suzuki
//
std::string
decode(uint64_t code)
{
  static constexpr char decode_map[] = "0123456789-+/():abcdefghijklmnopqrstuvwxyz?.@#$&";

  auto l   = code & 0xf;
  int  bit = 63;

  std::string r;
  r.reserve(l);

  for (; r.length() < l && bit > 4;)
  {
    uint64_t c;
    if (code & (1ULL << bit))
    {
      bit -= 6;
      c = (code >> (bit + 1)) & 0x1f; // long
      c += 0x10;
    }
    else
    {
      bit -= 5;
      c = (code >> (bit + 1)) & 0xf; // short
    }
    r += decode_map[c];
  }
  return r;
}
} // namespace SimpleStringEncode
