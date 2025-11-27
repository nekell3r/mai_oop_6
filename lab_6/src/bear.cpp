#include "bear.hpp"
#include "elf.hpp"
#include "robber.hpp"

Bear::Bear(const std::string& name, int x, int y)
    : NPC(NpcType::Bear, name, x, y) {}

bool Bear::Accept(std::shared_ptr<FightVisitor> visitor) {
  return visitor->Visit(std::dynamic_pointer_cast<Bear>(shared_from_this()));
}

// Bear kills Elves
bool Bear::Fight(std::shared_ptr<Bear>) {
  return false;
}

bool Bear::Fight(std::shared_ptr<Elf>) {
  return true;
}

bool Bear::Fight(std::shared_ptr<Robber>) {
  return false;
}

// Visitor implementation for Bear
bool Bear::Visit(std::shared_ptr<Bear> defender) {
  return Fight(defender);
}

bool Bear::Visit(std::shared_ptr<Elf> defender) {
  return Fight(defender);
}

bool Bear::Visit(std::shared_ptr<Robber> defender) {
  return Fight(defender);
}

