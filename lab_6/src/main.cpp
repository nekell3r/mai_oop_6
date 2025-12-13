#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <random>
#include <utility>

#include "bear.hpp"
#include "elf.hpp"
#include "fight_visitor.hpp"
#include "npc.hpp"
#include "npc_factory.hpp"
#include "observer.hpp"
#include "robber.hpp"

namespace {

std::vector<std::shared_ptr<lab6::NPC>> Fight(
    const std::vector<std::shared_ptr<lab6::NPC>>& npcs,
    size_t distance) {
  std::set<std::shared_ptr<lab6::NPC>> dead_list;
  std::set<std::pair<std::shared_ptr<lab6::NPC>, std::shared_ptr<lab6::NPC>>> processed_pairs;

  for (const auto& attacker : npcs) {
    for (const auto& defender : npcs) {
      if (attacker == defender) {
        continue;
      }

      auto pair1 = std::make_pair(attacker, defender);
      auto pair2 = std::make_pair(defender, attacker);
      if (processed_pairs.find(pair1) != processed_pairs.end() ||
          processed_pairs.find(pair2) != processed_pairs.end()) {
        continue;
      }

      if (attacker->IsClose(defender, distance)) {
        processed_pairs.insert(pair1);

        auto attacker_visitor = std::dynamic_pointer_cast<lab6::FightVisitor>(attacker);
        if (attacker_visitor && defender->Accept(attacker_visitor)) {
          attacker->FightNotify(attacker, defender, true);
          dead_list.insert(defender);
        }

        auto defender_visitor = std::dynamic_pointer_cast<lab6::FightVisitor>(defender);
        if (defender_visitor && attacker->Accept(defender_visitor)) {
          defender->FightNotify(defender, attacker, true);
          dead_list.insert(attacker);
        }
      }
    }
  }

  std::vector<std::shared_ptr<lab6::NPC>> result;
  for (const auto& npc : npcs) {
    if (dead_list.find(npc) == dead_list.end()) {
      result.push_back(npc);
    }
  }

  return result;
}

}  // namespace

int main() {
  auto console_obs = std::make_shared<lab6::ConsoleObserver>();
  auto file_obs = std::make_shared<lab6::FileObserver>("log.txt");

  std::vector<std::shared_ptr<lab6::NPC>> npcs;

  // создаем несколько NPC
  auto bear = lab6::NpcFactory::CreateNPC(lab6::NpcType::Bear, "Bear1", 100, 100);
  auto elf = lab6::NpcFactory::CreateNPC(lab6::NpcType::Elf, "Elf1", 110, 110);
  auto robber = lab6::NpcFactory::CreateNPC(lab6::NpcType::Robber, "Robber1", 120, 120);

  bear->Subscribe(console_obs);
  bear->Subscribe(file_obs);
  elf->Subscribe(console_obs);
  elf->Subscribe(file_obs);
  robber->Subscribe(console_obs);
  robber->Subscribe(file_obs);

  npcs.push_back(bear);
  npcs.push_back(elf);
  npcs.push_back(robber);

  std::cout << "Created " << npcs.size() << " NPCs\n";

  // сохраняем в файл
  lab6::NpcFactory::SaveToFile(npcs, "npcs.txt");
  std::cout << "Saved to file\n";

  // загружаем из файла
  auto loaded = lab6::NpcFactory::LoadFromFile("npcs.txt");
  for (auto& npc : loaded) {
    npc->Subscribe(console_obs);
    npc->Subscribe(file_obs);
  }
  std::cout << "Loaded " << loaded.size() << " NPCs from file\n\n";

  // бой
  std::cout << "Fighting...\n";
  auto survivors = Fight(loaded, 50);
  std::cout << "Survivors: " << survivors.size() << "\n\n";

  // генерим случайных NPC
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> type_dist(1, 3);
  std::uniform_int_distribution<> coord_dist(0, 500);

  std::vector<std::shared_ptr<lab6::NPC>> random_npcs;
  for (int i = 0; i < 50; ++i) {
    auto npc = lab6::NpcFactory::CreateNPC(
        static_cast<lab6::NpcType>(type_dist(gen)),
        "NPC" + std::to_string(i),
        coord_dist(gen),
        coord_dist(gen));
    npc->Subscribe(console_obs);
    npc->Subscribe(file_obs);
    random_npcs.push_back(npc);
  }

  std::cout << "Generated " << random_npcs.size() << " random NPCs\n";

  // несколько раундов боев
  for (size_t dist = 20; dist <= 100 && !random_npcs.empty(); dist += 20) {
    size_t before = random_npcs.size();
    random_npcs = Fight(random_npcs, dist);
    std::cout << "Distance " << dist << ": " << (before - random_npcs.size())
              << " killed, " << random_npcs.size() << " alive\n";
  }

  return 0;
}

