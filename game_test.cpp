//
#include "game_struct.hpp"
#include "worker.h"

#include <iostream>
#include <mutex>
#include <random>

namespace {
std::random_device seed_gen;
std::mt19937 l_rand{seed_gen()};
std::normal_distribution<> dist(1.0, 0.4);

constexpr size_t NB_PLAYERS = 10;
std::array<const char*,NB_PLAYERS> name_list = {
    "Suzuki", "Satou", "Watanabe", "Takahashi", "Itou",
    "Tanaka", "Yamada", "Yamamoto", "Nakamura", "Kobayashi"
};

constexpr size_t MAX_LEVEL = 50;
std::vector<int> exp_list;
void build_exp_list()
{
    float need_exp = 10.0;
    for (size_t i = 0; i < MAX_LEVEL; i++)
    {
        exp_list.push_back(need_exp);
        auto n = need_exp * 1.2f;
        if (n > 1000)
            n = 1000 + need_exp * 0.1f;
        need_exp += n;
    }
}

void status_alloc(Game::Player& player, int num)
{
    while (num > 0)
    {
        int n = num <= 2 ? 1 : l_rand() % (num > 5 ? num / 2 : num);
        num -= n;
        switch (l_rand() & 3)
        {
        case 0:
            player.setAttack(player.getAttack() + n);
            break;
        case 1:
            player.setDefence(player.getDefence() + n);
            break;
        case 2:
            player.setAgility(player.getAgility() + n);
            break;
        case 3:
            player.setLife(player.getLife() + n * 2);
            break;
        }
    }
    std::cout << player.getName() << ":\n  Life:" << player.getLife() << ", Attack: " << player.getAttack()
                  << ", Defence: " << player.getDefence() << ", Agility: " << player.getAgility() << std::endl;
}

void check_levelup(Game::Player& p)
{
    auto lv = p.getLevel();
    auto need_exp = exp_list[lv - 1];
    if (need_exp < p.getExp())
    {
        lv++;
        std::cout << "Level up! <" << p.getName() << ">: level " << lv << std::endl;
        p.setLevel(lv);
        status_alloc(p, 8);
    }
}

void battle(Game::Player& player1, Game::Player& player2)
{
    if (player1.getLife() == 0 || player2.getLife() == 0)
        return;

    auto hit = [](auto& p1, auto& p2) {
        double ag1 = p1.getAgility(), ag2 = p2.getAgility();
        double hr = ag1 * dist(l_rand) * 2.0 - ag2 * dist(l_rand);
        if (hr > 0.0)
        {
            double at = p1.getAttack();
            double df = p2.getDefence();
            at *= dist(l_rand) + sqrtf(hr) * 0.5;
            df *= dist(l_rand);
            auto life = p2.getLife();
            decltype(life) damage = (at - df) * 0.5;
            if (life <= damage)
            {
                damage = life;
                std::cout << "dead: " << p2.getName() << std::endl;
            }
            p2.setLife(life - damage);
            p1.setExp(p1.getExp() + damage / 2);
            check_levelup(p1);
        }
    };
    // std::cout << "Battle: " << player1.getName() << " vs " << player2.getName() << std::endl;
    hit(player1, player2);
    hit(player2, player1);
}

}

int
main(int argc,char** argv)
{
    WorkerThread wt;
    build_exp_list();

    std::vector<Game::Player> pl_list;
    pl_list.resize(NB_PLAYERS);
    for (int i = 0; i < pl_list.size(); i++)
    {
        auto& pl = pl_list[i];
        pl.setId(i + 1);
        pl.setName(name_list[i % name_list.size()]);
        status_alloc(pl,15);
    }

    for (int i = 0; i < 1000; i++)
    {
        int i1, i2;
        do
        {
            i1 = l_rand() % NB_PLAYERS;
            i2 = l_rand() % NB_PLAYERS;
        } while (i1 == i2);
        auto& p1 = pl_list[i1];
        auto& p2 = pl_list[i2];
        battle(p1, p2);
    }
    std::mutex m;
    for (auto& pl : pl_list) 
    {
        wt.push([&m](auto* player) {
            m.lock();
            std::cout << player->getName() << "," << player->getId() << ": " << player->getLife()
                      << " exp: " << player->getExp() << std::endl;
            m.unlock();
        }, &pl);
    }
    while (wt.checkComplete() == false)
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    return 0;
}
