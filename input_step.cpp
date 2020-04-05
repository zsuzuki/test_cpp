//
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

//
// sample step input
//
struct step
{
  int v;
  int c;
};

std::vector<step> step_list = {{1, 4}, {0, 2}, {1, 3}, {0, 5}, {1, 2}, {0, 1}, {1, 2}, {0, 1}};

namespace
{
int s_cnt = 0;
int c_cnt = 0;

std::pair<int, bool>
getInput()
{
  bool result = true;
  auto si     = step_list[s_cnt];
  if (c_cnt >= si.c - 1)
  {
    if (s_cnt < step_list.size() - 1)
    {
      s_cnt++;
      c_cnt = 0;
    }
    else
      result = false;

    si = step_list[s_cnt];
  }
  else
    c_cnt++;
  return std::make_pair(si.v, result);
}
} // namespace

//
// on/off switch update
//

struct OnOffSwitch
{
  enum class State : int
  {
    Off,
    Hold,
    On
  };
  bool  on    = false;
  State state = State::Off;
  int   count = 0;

  void On() { on = true; }
  void Off() { on = false; }
  void update()
  {
    bool hold = false;
    if (count)
    {
      hold = true;
      --count;
    }
    if (!count)
    {
      if (on)
      {
        count = 4;
        state = State::On;
      }
      else if (hold)
        state = State::Off;
    }
    else
      state = State::Hold;
  }
  const char* getStete() const
  {
    const char* result = "Unknown";
    switch (state)
    {
    case State::On:
      result = "On";
      break;
    case State::Off:
      result = "Off";
      break;
    case State::Hold:
      result = "Hold";
      break;
    }
    return result;
  }
};

//
// main
//
int
main()
{
  OnOffSwitch oos;
  for (int i = 0; i < 30; ++i)
  {
    auto n = getInput();
    if (n.first)
      oos.On();
    else
      oos.Off();
    oos.update();
    printf("input: %d %s\n", n.first, oos.getStete());
  }

  return 0;
}
//
//
//
