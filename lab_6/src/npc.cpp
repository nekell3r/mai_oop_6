#include "npc.hpp"
#include "observer.hpp"

NPC::NPC(NpcType type, const std::string& name, int x, int y)
    : name_(name), x_(x), y_(y), type_(type) {}

void NPC::Subscribe(std::shared_ptr<IFightObserver> observer) {
  observers_.push_back(observer);
}

void NPC::FightNotify(const std::shared_ptr<NPC>& attacker,
                      const std::shared_ptr<NPC>& defender,
                      bool win) {
  for (const auto& observer : observers_) {
    observer->OnFight(attacker, defender, win);
  }
}

bool NPC::IsClose(const std::shared_ptr<NPC>& other, size_t distance) const {
  auto dx = x_ - other->x_;
  auto dy = y_ - other->y_;
  return (dx * dx + dy * dy) <= static_cast<int>(distance * distance);
}

void NPC::Print(std::ostream& os) const {
  os << "NPC{name=\"" << name_ << "\", type=" << static_cast<int>(type_)
     << ", x=" << x_ << ", y=" << y_ << "}";
}

void NPC::Save(std::ostream& os) const {
  os << static_cast<int>(type_) << " " << name_ << " " << x_ << " " << y_;
}

std::ostream& operator<<(std::ostream& os, const NPC& npc) {
  npc.Print(os);
  return os;
}

