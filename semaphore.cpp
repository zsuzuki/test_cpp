//
// Copyright 2024 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <chrono>
#include <future>
#include <iostream>
#include <semaphore>
#include <thread>

std::counting_semaphore sem{0};

void
semtest01()
{
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "semtest 01" << std::endl;
  sem.release();
}

void
semtest02()
{
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "semtest 02" << std::endl;
  sem.release();
}

int
main(int argc, char** argv)
{
  auto ft1 = std::async(std::launch::async, semtest01);
  auto ft2 = std::async(std::launch::async, semtest02);

  std::cout << "waiting..." << std::endl;
  sem.acquire();
  sem.acquire();
  std::cout << "release" << std::endl;

  ft1.wait();
  ft2.wait();

  return 0;
}
