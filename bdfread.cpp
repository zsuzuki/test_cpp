//
// copyright wave.suzuki.z@gmail.com
// since 2021
//

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

///
std::string
convert(uint32_t c)
{
  char buff[5]{};
  int  len = 1;
  // 0000 0000-0000 007F | 0xxxxxxx
  // 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
  // 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
  // 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  if (c < 0x7f)
  {
    buff[0] = c;
  }
  else if (c < 0x7ff)
  {
    buff[0] = 0xc0 | (c >> 6);
    buff[1] = 0x80 | (c & 0x3f);
    len     = 2;
  }
  else if (c < 0xffff)
  {
    buff[0] = 0xe0 | (c >> 12);
    buff[1] = 0x80 | ((c >> 6) & 0x3f);
    buff[2] = 0x80 | ((c & 0x3f));
    len     = 3;
  }
  return buff;
}

/// 文字列nを文字sで区切って配列にして返す
/// 適当に切り貼りするので長い文字列には使用しない(遅い)
std::vector<std::string>
split(const std::string& n, const char s = ' ')
{
  std::vector<std::string> l;

  size_t pos = 0;
  do
  {
    auto p = n.find(s, pos);
    if (p == std::string::npos)
      break;
    l.emplace_back(n.substr(pos, p - pos));
    pos = p + 1;
  } while (true);

  return l;
}

/// 文字列nがsで始まるか調べて、その通りなら残りの文字列をargに入れて返す
bool
cmp(const std::string& n, const char* s, std::string& arg)
{
  int l = std::strlen(s);
  if (n.length() < l)
  {
    arg = "";
    return false;
  }

  for (int i = 0; i < l; i++)
  {
    if (n[i] != s[i])
    {
      arg = "";
      return false;
    }
  }
  arg = n.substr(l);
  return true;
};

/// 一行切り抜き
size_t
cut(const std::string& n, size_t pos, std::string& ret)
{
  auto p = n.find('\n', pos);
  ret    = p != std::string::npos ? n.substr(pos, p - pos) : "";
  return p;
}

} // namespace

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << argv[0] << ": need bdf-file" << std::endl;
    return -1;
  }
  std::filesystem::path fname{argv[1]};
  if (fname.extension() != ".bdf")
  {
    std::cerr << argv[0] << ": not bdf" << std::endl;
    return -1;
  }
  std::ifstream infile{fname};
  infile.seekg(0, std::ios_base::end);
  int filesize = infile.tellg();
  infile.seekg(0, std::ios_base::beg);
  std::cout << "filesize: " << filesize << std::endl;
  std::string buffer;
  buffer.resize(filesize);
  infile.read(buffer.data(), buffer.size());

  //
  size_t      fontSize = 16;
  size_t      count    = 0;
  std::string encode   = "ascii";

  // プロパティを読む
  size_t pos = 0;
  do
  {
    std::string line, arg;
    if (cut(buffer, pos, line) == std::string::npos)
    {
      std::cerr << "invalid format!" << std::endl;
      return -1;
    }
    pos += line.length() + 1;
    if (cmp(line, "PIXEL_SIZE ", arg))
      fontSize = std::stoi(arg);
    else if (cmp(line, "CHARSET_REGISTRY ", arg))
      encode = arg;
    else if (cmp(line, "ENDPROPERTIES", arg))
      break;
  } while (true);
  {
    // 文字数取得
    std::string line, arg;
    if (cut(buffer, pos, line) != std::string::npos)
    {
      if (cmp(line, "CHARS ", arg))
        count = std::stoi(arg);
    }
    pos += line.length() + 1;
  }
  // info
  std::cout << "文字コード: " << encode << std::endl;
  std::cout << "フォントサイズ: " << fontSize << std::endl;
  std::cout << "収録文字数: " << count << std::endl;

  struct font
  {
    uint32_t              code;
    std::string           utf8;
    std::vector<uint32_t> bitmap;

    bool operator==(const std::string& str) const { return utf8 == str; }
  };
  std::vector<font> fontList;
  fontList.reserve(count);

  font current;
  current.bitmap.resize(fontSize);
  //
  int cnt      = 0;
  int onBitmap = -1;
  while (true)
  {
    std::string line, arg;
    if (cut(buffer, pos, line) == std::string::npos)
      break;

    if (cmp(line, "STARTCHAR ", arg))
    {
      cnt++;
    }
    else if (cmp(line, "ENCODING ", arg))
    {
      current.code = std::stoi(arg);
      current.utf8 = convert(current.code);
    }
    else if (cmp(line, "BITMAP", arg))
    {
      onBitmap = 0;
    }
    else if (cmp(line, "ENDCHAR", arg))
    {
      onBitmap = -1;
      fontList.push_back(current);
    }
    else if (onBitmap >= 0)
    {
      current.bitmap[onBitmap++] = std::stoi(line, nullptr, 16);
    }

    pos += line.length() + 1;
  }
  std::cout << "find: " << fontList.size() << std::endl;

  // 検索
  if (argc >= 3)
  {
    std::string n  = argv[2];
    auto        it = std::find(fontList.begin(), fontList.end(), n);
    if (it != fontList.end())
    {
      auto& font = *it;
      std::cout << "found: " << n << std::endl;
      std::cout << "  code: " << font.code << std::endl;
      std::cout << "  index: " << it - fontList.begin() << std::endl;
      for (int y = 0; y < fontSize; y++)
      {
        uint32_t l = font.bitmap[y];
        char     xl[33]{};
        for (int x = 0; x < 32; x++)
        {
          xl[x] = l & (1 << (31 - x)) ? '#' : ' ';
        }
        std::cout << xl << std::endl;
      }
    }
    else
    {
      std::cout << "not fount: " << n << std::endl;
    }
  }

  return 0;
}
