//
// this file is auto generated
// by structbuilder<https://github.com/zsuzuki/structbuilder>
//
#pragma once

#include <string>
#include <cstdint>

namespace Game {

//
class Player {
public:
  // child class

//
struct Position {
  // members
  float x;
  float y;
  // constructor
  Position() {
    setX(0);
    setY(0);
  }
  //
  bool operator == (const Position& other) const {
    if (x != other.x) return false;
    if (y != other.y) return false;
    return true;
  }
  bool operator != (const Position& other) const {
    return !(*this == other);
  }
  //
  void copyFrom(const Position& other) {
    x = other.x;
    y = other.y;
  }
  Position& operator=(const Position& other) {
    copyFrom(other);
    return *this;
  }
  // interface
  //
  const float getX() const { return x; }
  void setX(float n) { x = n; }
  //
  const float getY() const { return y; }
  void setY(float n) { y = n; }
};

protected:
  struct BitField {
    unsigned life : 10;
    unsigned hp : 10;
    unsigned healing : 4;
    unsigned attack : 10;
    unsigned defence : 10;
    unsigned agility : 7;
  };
  BitField bit_field;
  // members
  std::string name;
  uint16_t id;
  uint16_t level;
  uint32_t exp;
  uint32_t nb_battle;
  Position pos;
public:
  // constructor
  Player() {
    setId(0);
    setLevel(1);
    setExp(0);
    setNbBattle(0);
    setLife(100);
    setHp(100);
    setHealing(9);
    setAttack(10);
    setDefence(7);
    setAgility(5);
  }
  //
  bool operator == (const Player& other) const {
    if (bit_field.life != other.bit_field.life) return false;
    if (bit_field.hp != other.bit_field.hp) return false;
    if (bit_field.healing != other.bit_field.healing) return false;
    if (bit_field.attack != other.bit_field.attack) return false;
    if (bit_field.defence != other.bit_field.defence) return false;
    if (bit_field.agility != other.bit_field.agility) return false;
    if (name != other.name) return false;
    if (id != other.id) return false;
    if (level != other.level) return false;
    if (exp != other.exp) return false;
    if (nb_battle != other.nb_battle) return false;
    if (pos != other.pos) return false;
    return true;
  }
  bool operator != (const Player& other) const {
    return !(*this == other);
  }
  //
  void copyFrom(const Player& other) {
    bit_field = other.bit_field;
    name = other.name;
    id = other.id;
    level = other.level;
    exp = other.exp;
    nb_battle = other.nb_battle;
    pos = other.pos;
  }
  Player& operator=(const Player& other) {
    copyFrom(other);
    return *this;
  }
  // interface
  //
  unsigned getLife() const { return bit_field.life * 1 + 0; }
  void setLife(unsigned n) { bit_field.life = (n - 0) / 1; }
  //
  unsigned getHp() const { return bit_field.hp * 1 + 0; }
  void setHp(unsigned n) { bit_field.hp = (n - 0) / 1; }
  //
  unsigned getHealing() const { return bit_field.healing * 1 + 0; }
  void setHealing(unsigned n) { bit_field.healing = (n - 0) / 1; }
  //
  unsigned getAttack() const { return bit_field.attack * 1 + 0; }
  void setAttack(unsigned n) { bit_field.attack = (n - 0) / 1; }
  //
  unsigned getDefence() const { return bit_field.defence * 1 + 0; }
  void setDefence(unsigned n) { bit_field.defence = (n - 0) / 1; }
  //
  unsigned getAgility() const { return bit_field.agility * 1 + 0; }
  void setAgility(unsigned n) { bit_field.agility = (n - 0) / 1; }
  //
  const std::string& getName() const { return name; }
  void setName(std::string n) { name = n; }
  //
  const uint16_t getId() const { return id; }
  void setId(uint16_t n) { id = n; }
  //
  const uint16_t getLevel() const { return level; }
  void setLevel(uint16_t n) { level = n; }
  //
  const uint32_t getExp() const { return exp; }
  void setExp(uint32_t n) { exp = n; }
  //
  const uint32_t getNbBattle() const { return nb_battle; }
  void setNbBattle(uint32_t n) { nb_battle = n; }
  //
  const Position& getPos() const { return pos; }
  void setPos(Position& n) { pos = n; }
};
} // namespace Game
