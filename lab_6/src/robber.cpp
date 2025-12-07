#include "robber.hpp"
#include "bear.hpp"
#include "elf.hpp"

Robber::Robber(const std::string& name, int x, int y)
    : NPC(NpcType::Robber, name, x, y) {}

bool Robber::Accept(std::shared_ptr<FightVisitor> visitor) {
  return visitor->Visit(std::dynamic_pointer_cast<Robber>(shared_from_this()));
}

// Visitor implementation for Robber - contains fight logic
// Robber kills Robbers
bool Robber::Visit(std::shared_ptr<Bear> /*defender*/) {
  return false;  // Robber cannot kill Bear
}

bool Robber::Visit(std::shared_ptr<Elf> /*defender*/) {
  return false;  // Robber cannot kill Elf
}

bool Robber::Visit(std::shared_ptr<Robber> /*defender*/) {
  return true;  // Robber kills Robber
}

