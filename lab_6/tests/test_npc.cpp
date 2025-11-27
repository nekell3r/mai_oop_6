#include <gtest/gtest.h>

#include "bear.hpp"
#include "elf.hpp"
#include "robber.hpp"

TEST(NPCTest, BearCreation) {
  Bear bear("Winnie", 100, 100);
  EXPECT_EQ(bear.GetName(), "Winnie");
  EXPECT_EQ(bear.GetX(), 100);
  EXPECT_EQ(bear.GetY(), 100);
  EXPECT_EQ(bear.GetType(), NpcType::Bear);
}

TEST(NPCTest, ElfCreation) {
  Elf elf("Legolas", 200, 200);
  EXPECT_EQ(elf.GetName(), "Legolas");
  EXPECT_EQ(elf.GetX(), 200);
  EXPECT_EQ(elf.GetY(), 200);
  EXPECT_EQ(elf.GetType(), NpcType::Elf);
}

TEST(NPCTest, RobberCreation) {
  Robber robber("Jack", 300, 300);
  EXPECT_EQ(robber.GetName(), "Jack");
  EXPECT_EQ(robber.GetX(), 300);
  EXPECT_EQ(robber.GetY(), 300);
  EXPECT_EQ(robber.GetType(), NpcType::Robber);
}

TEST(NPCTest, IsCloseTest) {
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  auto elf = std::make_shared<Elf>("Elf1", 3, 4);  // Distance = 5
  auto robber = std::make_shared<Robber>("Robber1", 100, 100);

  EXPECT_TRUE(bear->IsClose(elf, 10));
  EXPECT_TRUE(bear->IsClose(elf, 5));
  EXPECT_FALSE(bear->IsClose(elf, 4));
  EXPECT_FALSE(bear->IsClose(robber, 50));
}

TEST(NPCTest, FightRulesBearKillsElf) {
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);

  EXPECT_TRUE(bear->Fight(elf));
}

TEST(NPCTest, FightRulesBearDoesNotKillBear) {
  auto bear1 = std::make_shared<Bear>("Bear1", 0, 0);
  auto bear2 = std::make_shared<Bear>("Bear2", 0, 0);

  EXPECT_FALSE(bear1->Fight(bear2));
}

TEST(NPCTest, FightRulesBearDoesNotKillRobber) {
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);

  EXPECT_FALSE(bear->Fight(robber));
}

TEST(NPCTest, FightRulesElfKillsRobber) {
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);

  EXPECT_TRUE(elf->Fight(robber));
}

TEST(NPCTest, FightRulesElfDoesNotKillBear) {
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);

  EXPECT_FALSE(elf->Fight(bear));
}

TEST(NPCTest, FightRulesElfDoesNotKillElf) {
  auto elf1 = std::make_shared<Elf>("Elf1", 0, 0);
  auto elf2 = std::make_shared<Elf>("Elf2", 0, 0);

  EXPECT_FALSE(elf1->Fight(elf2));
}

TEST(NPCTest, FightRulesRobberKillsRobber) {
  auto robber1 = std::make_shared<Robber>("Robber1", 0, 0);
  auto robber2 = std::make_shared<Robber>("Robber2", 0, 0);

  EXPECT_TRUE(robber1->Fight(robber2));
}

TEST(NPCTest, FightRulesRobberDoesNotKillBear) {
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);

  EXPECT_FALSE(robber->Fight(bear));
}

TEST(NPCTest, FightRulesRobberDoesNotKillElf) {
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);

  EXPECT_FALSE(robber->Fight(elf));
}

