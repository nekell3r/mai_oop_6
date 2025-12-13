#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "bear.hpp"
#include "elf.hpp"
#include "npc_factory.hpp"
#include "robber.hpp"

using lab6::Bear;
using lab6::Elf;
using lab6::NPC;
using lab6::NpcFactory;
using lab6::NpcType;
using lab6::Robber;

TEST(FactoryTest, CreateBear) {
  auto npc = NpcFactory::CreateNPC(NpcType::Bear, "TestBear", 100, 100);
  
  ASSERT_NE(npc, nullptr);
  EXPECT_EQ(npc->GetType(), NpcType::Bear);
  EXPECT_EQ(npc->GetName(), "TestBear");
  EXPECT_EQ(npc->GetX(), 100);
  EXPECT_EQ(npc->GetY(), 100);
}

TEST(FactoryTest, CreateElf) {
  auto npc = NpcFactory::CreateNPC(NpcType::Elf, "TestElf", 200, 200);
  
  ASSERT_NE(npc, nullptr);
  EXPECT_EQ(npc->GetType(), NpcType::Elf);
  EXPECT_EQ(npc->GetName(), "TestElf");
  EXPECT_EQ(npc->GetX(), 200);
  EXPECT_EQ(npc->GetY(), 200);
}

TEST(FactoryTest, CreateRobber) {
  auto npc = NpcFactory::CreateNPC(NpcType::Robber, "TestRobber", 300, 300);
  
  ASSERT_NE(npc, nullptr);
  EXPECT_EQ(npc->GetType(), NpcType::Robber);
  EXPECT_EQ(npc->GetName(), "TestRobber");
  EXPECT_EQ(npc->GetX(), 300);
  EXPECT_EQ(npc->GetY(), 300);
}

TEST(FactoryTest, InvalidCoordinatesTooLarge) {
  EXPECT_THROW(
    NpcFactory::CreateNPC(NpcType::Bear, "InvalidBear", 600, 100),
    std::invalid_argument
  );
}

TEST(FactoryTest, InvalidCoordinatesNegative) {
  EXPECT_THROW(
    NpcFactory::CreateNPC(NpcType::Bear, "InvalidBear", -10, 100),
    std::invalid_argument
  );
}

TEST(FactoryTest, CreateFromStream) {
  std::stringstream ss;
  ss << "1 TestBear 100 150";
  
  auto npc = NpcFactory::CreateNPC(ss);
  
  ASSERT_NE(npc, nullptr);
  EXPECT_EQ(npc->GetType(), NpcType::Bear);
  EXPECT_EQ(npc->GetName(), "TestBear");
  EXPECT_EQ(npc->GetX(), 100);
  EXPECT_EQ(npc->GetY(), 150);
}

TEST(FactoryTest, SaveAndLoadFile) {
  std::vector<std::shared_ptr<NPC>> npcs;
  npcs.push_back(NpcFactory::CreateNPC(NpcType::Bear, "Bear1", 10, 20));
  npcs.push_back(NpcFactory::CreateNPC(NpcType::Elf, "Elf1", 30, 40));
  npcs.push_back(NpcFactory::CreateNPC(NpcType::Robber, "Robber1", 50, 60));

  const std::string filename = "test_npcs.txt";
  
  // Save
  EXPECT_NO_THROW(NpcFactory::SaveToFile(npcs, filename));
  
  // Load
  auto loaded_npcs = NpcFactory::LoadFromFile(filename);
  
  ASSERT_EQ(loaded_npcs.size(), 3);
  
  EXPECT_EQ(loaded_npcs[0]->GetType(), NpcType::Bear);
  EXPECT_EQ(loaded_npcs[0]->GetName(), "Bear1");
  EXPECT_EQ(loaded_npcs[0]->GetX(), 10);
  EXPECT_EQ(loaded_npcs[0]->GetY(), 20);
  
  EXPECT_EQ(loaded_npcs[1]->GetType(), NpcType::Elf);
  EXPECT_EQ(loaded_npcs[1]->GetName(), "Elf1");
  EXPECT_EQ(loaded_npcs[1]->GetX(), 30);
  EXPECT_EQ(loaded_npcs[1]->GetY(), 40);
  
  EXPECT_EQ(loaded_npcs[2]->GetType(), NpcType::Robber);
  EXPECT_EQ(loaded_npcs[2]->GetName(), "Robber1");
  EXPECT_EQ(loaded_npcs[2]->GetX(), 50);
  EXPECT_EQ(loaded_npcs[2]->GetY(), 60);
  
  // Cleanup
  std::remove(filename.c_str());
}

TEST(FactoryTest, LoadNonExistentFile) {
  EXPECT_THROW(
    NpcFactory::LoadFromFile("non_existent_file.txt"),
    std::runtime_error
  );
}

