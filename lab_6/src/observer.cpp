#include "observer.hpp"

#include <iostream>
#include <string_view>

#include "npc.hpp"

namespace lab6 {
namespace {

constexpr std::string_view kMurderPrefix = "MURDER: ";

const char* NpcTypeToString(NpcType type) {
  switch (type) {
    case NpcType::Bear:
      return "Bear";
    case NpcType::Elf:
      return "Elf";
    case NpcType::Robber:
      return "Robber";
    case NpcType::Unknown:
      break;
  }
  return "Unknown";
}

}  // namespace

void ConsoleObserver::OnFight(const std::shared_ptr<NPC>& attacker,
                              const std::shared_ptr<NPC>& defender,
                              bool win) {
  if (win) {
    std::cout << kMurderPrefix << NpcTypeToString(attacker->GetType())
              << " \"" << attacker->GetName() << "\" killed "
              << NpcTypeToString(defender->GetType())
              << " \"" << defender->GetName() << "\"" << std::endl;
  }
}

FileObserver::FileObserver(const std::string& filename)
    : log_file_(filename, std::ios::app) {}

FileObserver::~FileObserver() {
  if (log_file_.is_open()) {
    log_file_.close();
  }
}

void FileObserver::OnFight(const std::shared_ptr<NPC>& attacker,
                           const std::shared_ptr<NPC>& defender,
                           bool win) {
  if (win && log_file_.is_open()) {
    log_file_ << kMurderPrefix << NpcTypeToString(attacker->GetType())
              << " \"" << attacker->GetName() << "\" killed "
              << NpcTypeToString(defender->GetType())
              << " \"" << defender->GetName() << "\"" << std::endl;
  }
}

}  // namespace lab6







