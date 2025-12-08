#include "bear.hpp"
#include "elf.hpp"
#include "robber.hpp"

namespace lab6 {

Bear::Bear(const std::string& name, int x, int y)
    : NPC(NpcType::Bear, name, x, y) {}

bool Bear::Accept(std::shared_ptr<FightVisitor> visitor) {
  return visitor->Visit(std::dynamic_pointer_cast<Bear>(shared_from_this()));
}

// Visitor implementation for Bear - contains fight logic
// Bear kills Elves
bool Bear::Visit(std::shared_ptr<Bear> /*defender*/) {
  return false;  // Bear cannot kill Bear
}

bool Bear::Visit(std::shared_ptr<Elf> /*defender*/) {
  return true;  // Bear kills Elf
}

bool Bear::Visit(std::shared_ptr<Robber> /*defender*/) {
  return false;  // Bear cannot kill Robber
}

}  // namespace lab6

