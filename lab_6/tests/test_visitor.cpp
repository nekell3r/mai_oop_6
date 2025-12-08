#include <gtest/gtest.h>

#include "bear.hpp"
#include "elf.hpp"
#include "robber.hpp"

using lab6::Bear;
using lab6::Elf;
using lab6::FightVisitor;
using lab6::Robber;

TEST(VisitorTest, BearVisitsElf) {
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(bear);
  ASSERT_NE(visitor, nullptr);
  
  bool result = elf->Accept(visitor);
  EXPECT_TRUE(result);
}

TEST(VisitorTest, BearVisitsBear) {
  auto bear1 = std::make_shared<Bear>("Bear1", 0, 0);
  auto bear2 = std::make_shared<Bear>("Bear2", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(bear1);
  ASSERT_NE(visitor, nullptr);
  
  bool result = bear2->Accept(visitor);
  EXPECT_FALSE(result);
}

TEST(VisitorTest, BearVisitsRobber) {
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(bear);
  ASSERT_NE(visitor, nullptr);
  
  bool result = robber->Accept(visitor);
  EXPECT_FALSE(result);
}

TEST(VisitorTest, ElfVisitsRobber) {
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(elf);
  ASSERT_NE(visitor, nullptr);
  
  bool result = robber->Accept(visitor);
  EXPECT_TRUE(result);
}

TEST(VisitorTest, ElfVisitsBear) {
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(elf);
  ASSERT_NE(visitor, nullptr);
  
  bool result = bear->Accept(visitor);
  EXPECT_FALSE(result);
}

TEST(VisitorTest, ElfVisitsElf) {
  auto elf1 = std::make_shared<Elf>("Elf1", 0, 0);
  auto elf2 = std::make_shared<Elf>("Elf2", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(elf1);
  ASSERT_NE(visitor, nullptr);
  
  bool result = elf2->Accept(visitor);
  EXPECT_FALSE(result);
}

TEST(VisitorTest, RobberVisitsRobber) {
  auto robber1 = std::make_shared<Robber>("Robber1", 0, 0);
  auto robber2 = std::make_shared<Robber>("Robber2", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(robber1);
  ASSERT_NE(visitor, nullptr);
  
  bool result = robber2->Accept(visitor);
  EXPECT_TRUE(result);
}

TEST(VisitorTest, RobberVisitsBear) {
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  auto bear = std::make_shared<Bear>("Bear1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(robber);
  ASSERT_NE(visitor, nullptr);
  
  bool result = bear->Accept(visitor);
  EXPECT_FALSE(result);
}

TEST(VisitorTest, RobberVisitsElf) {
  auto robber = std::make_shared<Robber>("Robber1", 0, 0);
  auto elf = std::make_shared<Elf>("Elf1", 0, 0);
  
  auto visitor = std::dynamic_pointer_cast<FightVisitor>(robber);
  ASSERT_NE(visitor, nullptr);
  
  bool result = elf->Accept(visitor);
  EXPECT_FALSE(result);
}







