#include "arduino_serial.hpp"
#include <iostream>

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << argv[0] << ": <serial device>" << std::endl;
    return 1;
  }

  using AR = Arduino::SerialCommunicator;
  AR sc;

  sc.initialize(argv[1], [&](auto& cmd, auto& args) {
    if (cmd == "Screen")
    {
      std::cout << "screen size:" << args[0] << "," << args[1] << std::endl;
      sc.print("From PC", 200, 50);
      sc.print("Send Message", 150, 70);
      sc.print("Sample Program", 80, 240, Arduino::RGB(255, 255, 0));
      sc.rect(140, 150, 140, 40, Arduino::RGB(0, 255, 255));
      sc.rect(140, 90, 140, 50, Arduino::RGB(0, 255, 0));
      sc.fill(200, 160, 60, 20, Arduino::RGB(128, 128, 128));
      sc.fill(140, 200, 140, 20, Arduino::RGB(180, 180, 0));
      sc.print("My Computer", 160, 206, Arduino::RGB(50, 0, 0), Arduino::RGB(180, 180, 0));
      sc.hline(50, 20, 390);
      sc.vline(50, 20, 100);
      sc.vline(440, 20, 100);
      sc.print("TEST", 300, 50, Arduino::RGB(255, 0, 0));
    }
  });

  while (true)
  {
    sc.update();
  }

  return 0;
}
