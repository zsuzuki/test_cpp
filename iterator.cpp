#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <random>

template <size_t SIZE>
class FlexArray
{
public:
  class iterator : public std::iterator<std::input_iterator_tag, int, int, const int*, int&>
  {
    size_t num  = 0;
    int*   list = nullptr;

  public:
    explicit iterator(int* l = nullptr, size_t n = 0) : list(l), num(n) {}
    iterator& operator++()
    {
      num++;
      return *this;
    }
    iterator operator++(int)
    {
      iterator r = *this;
      ++num;
      return r;
    }
    iterator operator--()
    {
      num = num > 0 ? num - 1 : 0;
      return *this;
    }
    iterator operator+(difference_type n) { return iterator(list, num + n); }
    iterator operator+=(difference_type n)
    {
      num += n;
      return *this;
    }
    iterator operator=(const iterator other)
    {
      num = other.num;
      return *this;
    }
    bool            operator==(iterator other) const { return num == other.num; }
    bool            operator!=(iterator other) const { return !(*this == other); }
    bool            operator<(iterator other) const { return num < other.num; }
    bool            operator>(iterator other) const { return num > other.num; }
    bool            operator>=(iterator other) const { return num >= other.num; }
    difference_type operator+(iterator other) const { return num + other.num; }
    difference_type operator-(iterator other) const { return num - other.num; }
    reference       operator[](difference_type n) const { return list[n]; }
    reference       operator*() const { return list[num]; }
    size_t          get_index() const { return num; }
  };
  size_t                cutsize;
  std::array<int, SIZE> list;
  FlexArray(size_t sz = SIZE) : cutsize(sz) {}
  void       resize(size_t nsz) { cutsize = nsz < SIZE ? nsz : SIZE - 1; }
  size_t     size() const { return cutsize; }
  size_t     capacity() const { return SIZE; }
  iterator   begin() { return iterator(list.data(), 0); }
  iterator   end() { return iterator(list.data(), cutsize); }
  int&       operator[](int n) { return list[n]; }
  void       iota(int s = 0) { std::iota(begin(), end(), s); }
  FlexArray& operator=(const FlexArray& other)
  {
    if (cutsize == other.cutsize)
    {
      for (size_t i = 0; i < cutsize; i++)
        list[i] = other.list[i];
    }
    return *this;
  }
};

int
main()
{
  auto range = FlexArray<48>(25);
  range.iota();
  std::random_device seed_gen;
  std::mt19937       engine(seed_gen());
  std::shuffle(range.begin(), range.end(), engine);

  auto itr = std::find(range.begin(), range.end(), 18);
  std::cout << *itr << '(' << itr.get_index() << ')' << '\n'; // 18
  for (auto l : range)
    std::cout << l << ' ';
  std::cout << std::endl;
  std::sort(range.begin(), range.end(), [](int a, int b) { return a < b; });
  for (auto l : range)
    std::cout << l << ' ';
  std::cout << ": " << sizeof(range);
  std::cout << '\n';

  return 0;
}
