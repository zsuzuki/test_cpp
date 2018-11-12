//
#include "game_struct.hpp"
#include "worker.h"

#include <iostream>
#include <mutex>

int
main(int argc,char** argv)
{
    WorkerThread wt;
    std::vector<Game::Player> pl_list;
    pl_list.resize(10);
    for (int i = 0; i < pl_list.size(); i++)
    {
        static std::array<const char*,10> nl_u = {
            "Suzuki", "Satou", "Watanabe", "Takahashi", "Itou",
            "Tanaka", "Yamada", "Yamamoto", "Nakamura", "Kobayashi"
        };
        auto& pl = pl_list[i];
        pl.setId(i + 1);
        pl.setName(nl_u[i % nl_u.size()]);
    }
    std::mutex m;
    for (auto& pl : pl_list) 
    {
        wt.push([&m](auto* player) {
            m.lock();
            std::cout << player->getName() << "," << player->getId() << ": " << player->getLife() << std::endl;
            m.unlock();
        }, &pl);
    }
    while (wt.checkComplete() == false)
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    return 0;
}
