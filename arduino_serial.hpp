//
// arduino serial communication sample
// wave.suzuki.z@gmail.com
// 2021
//
#pragma once

#include <chrono>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <poll.h>
#include <queue>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

namespace Arduino
{

using RecvStatus = std::function<void(std::string&, std::vector<std::string>&)>;

static constexpr uint16_t
RGB(uint8_t r, uint8_t g, uint8_t b)
{
  r >>= 3;
  g >>= 2;
  b >>= 3;
  return (uint16_t)r << 11 | (uint16_t)g << 5 | (uint16_t)b;
}

///
class SerialCommunicator
{
  using time_point = std::chrono::steady_clock::time_point;
  int FD;

  std::vector<char>        readBuffer;
  bool                     commandIn;
  bool                     argumentIn;
  std::string              readString;
  std::string              command;
  std::vector<std::string> arguments;
  time_point               lastSendTime;
  std::queue<std::string>  sendQueue;
  RecvStatus               recvFunction;
  bool                     sendEnable;

  void sendCommand()
  {
    if (!sendEnable)
      return;

    using namespace std::chrono;
    auto nowtime = steady_clock::now();
    auto diff    = duration_cast<microseconds>(nowtime - lastSendTime).count();
    if (diff <= 1000 * 1000 / 115200)
      return;
    if (sendQueue.empty())
      return;

    auto& f = sendQueue.front();
    write(FD, f.data(), f.size());
    // std::cout << "Send:" << f << std::endl;
    sendQueue.pop();
    lastSendTime = nowtime;
    sendEnable   = false;
  }

  bool readStatus()
  {
    struct pollfd fds[1];
    fds[0].fd     = FD;
    fds[0].events = POLL_IN;
    auto ret      = poll(fds, 1, 0);
    if (ret < 0)
      return false;
    // timeout
    if (ret == 0)
      return true;

    int rsz = read(FD, readBuffer.data(), readBuffer.size());
    for (int i = 0; i < rsz; i++)
    {
      auto ch = readBuffer[i];
      if (ch == '#')
      {
        if (commandIn)
        {
          if (readString.empty() == false)
          {
            if (argumentIn)
              arguments.push_back(readString);
            else
              command = readString;
          }
          readString.clear();
          if (command == "Complete")
          {
            sendEnable = true;
          }
          else if (recvFunction)
          {
            recvFunction(command, arguments);
          }
          commandIn  = false;
          argumentIn = false;
        }
        else
        {
          commandIn = true;
          command.clear();
        }
      }
      else if (commandIn)
      {
        if (argumentIn)
        {
          if (ch == ',')
          {
            arguments.push_back(readString);
            readString.clear();
          }
          else
            readString += ch;
        }
        else if (ch == ':')
        {
          argumentIn = true;
          command    = readString;
          readString.clear();
        }
        else
        {
          readString += ch;
        }
      }
    }
    return true;
  }

public:
  SerialCommunicator()  = default;
  ~SerialCommunicator() = default;

  bool initialize(const char* fname, RecvStatus recvf)
  {
    FD = open(fname, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (FD < 0)
    {
      std::cerr << "Error: device open failed[" << fname << "]" << std::endl;
      return false;
    }
    if (auto r = tcflush(FD, TCIOFLUSH))
    {
      std::cerr << "Error: flush " << r << std::endl;
      return false;
    }
    struct termios opt;
    if (auto r = tcgetattr(FD, &opt))
    {
      std::cerr << "Error: getattr " << r << std::endl;
      return false;
    }
    // opt.c_cflag = CS8 | CLOCAL | CREAD;
    opt.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    opt.c_oflag &= ~(ONLCR | OCRNL);
    opt.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    opt.c_cc[VTIME] = 1;
    opt.c_cc[VMIN]  = 0;

    cfsetospeed(&opt, B115200);
    cfsetispeed(&opt, cfgetospeed(&opt));
    tcsetattr(FD, TCSANOW, &opt);

    readBuffer.resize(1024);
    commandIn  = false;
    argumentIn = false;
    readString.reserve(128);
    readString.clear();
    command.clear();
    arguments.clear();
    lastSendTime = std::chrono::steady_clock::now();
    recvFunction = recvf;
    sendEnable   = true;

    return true;
  };

  void print(const char* msg, int x, int y, int color = 0xffff, int bg = 0)
  {
    char   buff[64];
    size_t sz = snprintf(buff, sizeof(buff), "#%03d%03d%04x%04x%s\n", x, y, color, bg, msg);
    sendQueue.push({buff, sz});
  }

  void rect(int x, int y, int w, int h, int color = 0xffff)
  {
    char   buff[64];
    size_t sz = snprintf(buff, sizeof(buff), "!%03d%03d%03d%03d%04x\n", x, y, w, h, color);
    sendQueue.push({buff, sz});
  }

  void fill(int x, int y, int w, int h, int color = 0xffff)
  {
    char   buff[64];
    size_t sz = snprintf(buff, sizeof(buff), "$%03d%03d%03d%03d%04x\n", x, y, w, h, color);
    sendQueue.push({buff, sz});
  }

  void hline(int x, int y, int w, int color = 0xffff)
  {
    char   buff[64];
    size_t sz = snprintf(buff, sizeof(buff), "-%03d%03d%03d%04x\n", x, y, w, color);
    sendQueue.push({buff, sz});
  }

  void vline(int x, int y, int h, int color = 0xffff)
  {
    char   buff[64];
    size_t sz = snprintf(buff, sizeof(buff), "|%03d%03d%03d%04x\n", x, y, h, color);
    sendQueue.push({buff, sz});
  }

  bool update()
  {
    if (readStatus())
    {
      sendCommand();
      return true;
    }
    return false;
  }
};
} // namespace Arduino
