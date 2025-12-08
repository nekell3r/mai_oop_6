#include "elf.hpp"
#include "bear.hpp"
#include "robber.hpp"

namespace lab6 {

Elf::Elf(const std::string& name, int x, int y)
    : NPC(NpcType::Elf, name, x, y) {}

bool Elf::Accept(std::shared_ptr<FightVisitor> visitor) {
  return visitor->Visit(std::dynamic_pointer_cast<Elf>(shared_from_this()));
}

// Visitor implementation for Elf - contains fight logic
// Elf kills Robbers
bool Elf::Visit(std::shared_ptr<Bear> /*defender*/) {
  return false;  // Elf cannot kill Bear
}

bool Elf::Visit(std::shared_ptr<Elf> /*defender*/) {
  return false;  // Elf cannot kill Elf
}

bool Elf::Visit(std::shared_ptr<Robber> /*defender*/) {
  return true;  // Elf kills Robber
}

}  // namespace lab6

