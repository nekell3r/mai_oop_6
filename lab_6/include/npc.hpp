#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

// Forward declarations
class Bear;
class Elf;
class Robber;
class FightVisitor;
class IFightObserver;

enum class NpcType {
  Unknown = 0,
  Bear = 1,
  Elf = 2,
  Robber = 3
};

class NPC : public std::enable_shared_from_this<NPC> {
 protected:
  std::string name_;
  int x_;
  int y_;
  NpcType type_;
  std::vector<std::shared_ptr<IFightObserver>> observers_;

 public:
  NPC(NpcType type, const std::string& name, int x, int y);
  virtual ~NPC() = default;

  void Subscribe(std::shared_ptr<IFightObserver> observer);
  void FightNotify(const std::shared_ptr<NPC>& attacker, 
                   const std::shared_ptr<NPC>& defender, 
                   bool win);

  bool IsClose(const std::shared_ptr<NPC>& other, size_t distance) const;

  virtual bool Accept(std::shared_ptr<FightVisitor> visitor) = 0;

  virtual bool Fight(std::shared_ptr<Bear> other) = 0;
  virtual bool Fight(std::shared_ptr<Elf> other) = 0;
  virtual bool Fight(std::shared_ptr<Robber> other) = 0;

  virtual void Print(std::ostream& os) const;
  virtual void Save(std::ostream& os) const;

  std::string GetName() const { return name_; }
  int GetX() const { return x_; }
  int GetY() const { return y_; }
  NpcType GetType() const { return type_; }

  friend std::ostream& operator<<(std::ostream& os, const NPC& npc);
};



