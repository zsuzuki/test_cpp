//
#include "game_struct.hpp"
#include "worker.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>

namespace
{
std::random_device         seed_gen;
std::mt19937               l_rand{seed_gen()};
std::normal_distribution<> dist(1.0, 0.4);

constexpr size_t NB_PLAYERS = 10;

using NameList     = std::array<const char*, NB_PLAYERS>;
NameList name_list = {"Suzuki", "Satou",  "Watanabe", "Takahashi", "Itou",
                      "Tanaka", "Yamada", "Yamamoto", "Nakamura",  "Kobayashi"};

constexpr size_t MAX_LEVEL = 50;
std::vector<int> exp_list;
// 経験値リストの作成
void
build_exp_list()
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

// 適当なステータス配分
void
status_alloc(Game::Player& player, int num)
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
  std::cout << player.getName() << ":\n  Life:" << player.getHp() << "/" << player.getLife() << ", Attack: " << player.getAttack()
            << ", Defence: " << player.getDefence() << ", Agility: " << player.getAgility() << std::endl;
}

// 回復
void
healing(Game::Player& p)
{
  int nb_heal = p.getHealing();
  if (nb_heal > 0)
  {
    int hp     = rand() % 25 + 50 + p.getHp();
    int max_hp = p.getLife();
    p.setHp(max_hp < hp ? max_hp : hp);
    --nb_heal;
    std::cout << "healing: " << p.getName() << " -> " << p.getHp() << "(" << nb_heal << ")" << std::endl;
    p.setHealing(nb_heal);
  }
}

// レベルアップチェック
void
check_levelup(Game::Player& p)
{
  auto lv       = p.getLevel();
  auto need_exp = exp_list[lv - 1];
  if (need_exp < p.getExp())
  {
    lv++;
    std::cout << "Level up! <" << p.getName() << ">: level " << lv << std::endl;
    p.setLevel(lv);
    p.setHealing(p.getHealing() + 1);
    status_alloc(p, 8);
  }
}

// 戦闘
void
battle(Game::Player& player1, Game::Player& player2)
{
  if (player1.getHp() == 0 || player2.getHp() == 0)
    return;

  auto hit = [](auto& p1, auto& p2) {
    double ag1 = p1.getAgility(), ag2 = p2.getAgility();
    double hr = ag1 * dist(l_rand) * 2.0 - ag2 * dist(l_rand);
    if (hr > 0.0)
    {
      double at = p1.getAttack();
      double df = p2.getDefence();
      at *= dist(l_rand) + sqrtf(hr) * 0.3;
      df *= dist(l_rand);
      auto           life   = p2.getHp();
      decltype(life) damage = (at - df) * 0.4;
      if (life <= damage)
      {
        damage = life;
        std::cout << "dead: " << p2.getName() << std::endl;
      }
      auto new_hp = life - damage;
      p2.setHp(new_hp);
      auto ldiff = p2.getLife() - new_hp;
      if (new_hp > 0 && (new_hp < 60 || ldiff > 75))
        healing(p2);
      p1.setExp(p1.getExp() + damage / 2);
      check_levelup(p1);
      return new_hp > 0;
    }
    return true;
  };

  double a1 = player1.getAgility();
  double a2 = player2.getAgility();
  if (a1 * dist(l_rand) > a2 * dist(l_rand))
  {
    if (hit(player1, player2))
      hit(player2, player1);
  }
  else
  {
    if (hit(player2, player1))
      hit(player1, player2);
  }
  player1.setNbBattle(player1.getNbBattle() + 1);
  player2.setNbBattle(player2.getNbBattle() + 1);
}

} // namespace

//
//
//
int
main(int argc, char** argv)
{
  WorkerThread wt;
  // ワーカーがすべて終了するまで待つ
  auto wait = [&wt]() {
    using namespace std::chrono;
    auto s = steady_clock::now();
    while (wt.checkComplete() == false)
    {
      std::this_thread::sleep_for(microseconds(0));
      auto e  = steady_clock::now();
      auto et = duration_cast<seconds>(e - s);
      if (et.count() > 1)
      {
        std::cout << "Time out!" << std::endl;
        return false;
      }
    }
    return true;
  };

  build_exp_list();

  // プレイヤー生成
  std::vector<Game::Player> pl_list;
  pl_list.resize(NB_PLAYERS);
  for (int i = 0; i < pl_list.size(); i++)
  {
    auto& pl = pl_list[i];
    pl.setId(i + 1);
    pl.setName(name_list[i % name_list.size()]);
    pl.setHp(pl.getLife());
    status_alloc(pl, 15);
  }

  std::vector<int> pl_ids;
  pl_ids.resize(pl_list.size());
  NQueue<int> pl_que{pl_list.size() * 2, -1};

  // 戦闘ループ
  std::iota(pl_ids.begin(), pl_ids.end(), 0);
  std::shuffle(pl_ids.begin(), pl_ids.end(), l_rand);
  for (int i = 0; i < 1000; i++)
  {
    std::atomic_int ecnt{(int)pl_ids.size() / 2};
    wt.clear();
    // 最後に実行するものを待たせておく
    wt.push(ecnt, [&]() {
      // 生き残りリストをシャッフルする
      auto sz = pl_ids.size();
      if (sz & 1)
        pl_que.push(pl_ids[sz - 1]);
      else if (pl_que.empty())
        return;
      pl_ids.resize(0);
      while (auto id = pl_que.pop())
        pl_ids.push_back(id.value);
      std::shuffle(pl_ids.begin(), pl_ids.end(), l_rand);
    });
    for (int pi = 0; pi < pl_ids.size() / 2; pi++)
    {
      wt.push(
          [&](int idx) {
            int   i1 = pl_ids[idx];
            int   i2 = pl_ids[idx + 1];
            auto& p1 = pl_list[i1];
            auto& p2 = pl_list[i2];
            battle(p1, p2);
            if (p1.getHp() > 0)
              pl_que.push(i1);
            if (p2.getHp() > 0)
              pl_que.push(i2);
            int ec, en;
            do
            {
              ec = ecnt;
              en = ec - 1;
            } while (ecnt.compare_exchange_weak(ec, en) == false);
          },
          pi * 2);
    }
    if (wait() == false)
    {
      char pm[128];
      snprintf(pm, sizeof(pm), "E:%d", ecnt.load());
      std::cout << pm << std::endl;
    }
    if (pl_ids.size() == 1)
    {
      // 一人になったので終了
      std::cout << "[" << i << "] survive only one." << std::endl;
      break;
    }
  }
  // 結果表示
  std::mutex m;
  for (auto& pl : pl_list)
  {
    wt.push(
        [&m](auto* player) {
          auto is_dead = [&]() -> const char* { return player->getHp() > 0 ? "LIVE" : "DEAD"; };
          m.lock();
          std::cout << player->getName() << ", Lv:" << player->getLevel() << ": " << is_dead()
                    << " battle: " << player->getNbBattle() << " exp: " << player->getExp() << std::endl;
          m.unlock();
        },
        &pl);
  }
  wait();

  return 0;
}
