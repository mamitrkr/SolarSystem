/**
 * @file unit_tests.cpp
 * @brief Unit tests for the SolarSystem<T> data structure.
 *
 * Tests pushToPlanet, removePlanet (Black Hole), gravityPull, and
 * validates that star.element_count always matches the actual sum
 * of elements across all planets after every operation.
 *
 * Uses Google Test (GTest) framework.
 */
#include "SolarSystem.hpp"
#include <gtest/gtest.h>


// ═══════════════════════════════════════════════════════════════
//  Helper: Validates element_count consistency after every operation
// ═══════════════════════════════════════════════════════════════

/**
 * @brief Asserts that the tracked element count matches the actual sum.
 *
 * This is the core validation specified in the requirements:
 * star.element_count must always equal the sum of all planet sizes.
 */
#define ASSERT_ELEMENT_COUNT_CONSISTENT(sys)                                   \
  ASSERT_EQ((sys).getElementCount(), (sys).getActualElementCount())            \
      << "CRITICAL: star.element_count is out of sync with actual planet "     \
         "elements!"

// ═══════════════════════════════════════════════════════════════
//  Test Fixture
// ═══════════════════════════════════════════════════════════════

class SolarSystemTest : public ::testing::Test {
protected:
  SolarSystem<int> system;

  void SetUp() override {
    // Start each test with 3 planets (one of each type)
    system.addPlanet(PlanetType::Vector);
    system.addPlanet(PlanetType::Stack);
    system.addPlanet(PlanetType::Queue);
  }
};

// ═══════════════════════════════════════════════════════════════
//  pushToPlanet Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(SolarSystemTest, PushToVectorPlanet) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(0, 20);
  system.pushToPlanet(0, 30);

  ASSERT_EQ(system.getElementCount(), 3);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, PushToStackPlanet) {
  system.pushToPlanet(1, 100);
  system.pushToPlanet(1, 200);

  ASSERT_EQ(system.getElementCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, PushToQueuePlanet) {
  system.pushToPlanet(2, 50);

  ASSERT_EQ(system.getElementCount(), 1);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, PushToAllPlanets) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(1, 20);
  system.pushToPlanet(2, 30);

  ASSERT_EQ(system.getElementCount(), 3);
  ASSERT_EQ(system.getPlanetCount(), 3);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, PushToInvalidIndexThrows) {
  ASSERT_THROW(system.pushToPlanet(-1, 10), std::out_of_range);
  ASSERT_THROW(system.pushToPlanet(3, 10), std::out_of_range);
  ASSERT_THROW(system.pushToPlanet(99, 10), std::out_of_range);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, PushMultipleElementsAccumulatesCount) {
  for (int i = 0; i < 100; i++)
    system.pushToPlanet(0, i);

  ASSERT_EQ(system.getElementCount(), 100);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

// ═══════════════════════════════════════════════════════════════
//  removePlanet (Black Hole) Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(SolarSystemTest, RemovePlanetDecrementsPlanetCount) {
  ASSERT_EQ(system.getPlanetCount(), 3);

  system.removePlanet(0);
  ASSERT_EQ(system.getPlanetCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, RemovePlanetWithElementsDecrementsElementCount) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(0, 20);
  system.pushToPlanet(0, 30);
  system.pushToPlanet(1, 40);
  ASSERT_EQ(system.getElementCount(), 4);

  // Remove planet 0 (has 3 elements)
  system.removePlanet(0);
  ASSERT_EQ(system.getElementCount(), 1);
  ASSERT_EQ(system.getPlanetCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, RemoveEmptyPlanetKeepsElementCount) {
  system.pushToPlanet(1, 99);
  ASSERT_EQ(system.getElementCount(), 1);

  // Remove planet 0 which has 0 elements
  system.removePlanet(0);
  ASSERT_EQ(system.getElementCount(), 1);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, RemoveAllPlanetsOneByOne) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(1, 20);
  system.pushToPlanet(2, 30);

  system.removePlanet(2);
  ASSERT_EQ(system.getPlanetCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  system.removePlanet(1);
  ASSERT_EQ(system.getPlanetCount(), 1);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  system.removePlanet(0);
  ASSERT_EQ(system.getPlanetCount(), 0);
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, RemovePlanetInvalidIndexThrows) {
  ASSERT_THROW(system.removePlanet(-1), std::out_of_range);
  ASSERT_THROW(system.removePlanet(3), std::out_of_range);
  ASSERT_THROW(system.removePlanet(99), std::out_of_range);
}

TEST_F(SolarSystemTest, BlackHoleAliasWorks) {
  system.pushToPlanet(0, 10);
  system.blackHole(0); // Should behave identically to removePlanet
  ASSERT_EQ(system.getPlanetCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

// ═══════════════════════════════════════════════════════════════
//  gravityPull Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(SolarSystemTest, GravityPullRemovesBelowThreshold) {
  system.pushToPlanet(0, 5);
  system.pushToPlanet(0, 15);
  system.pushToPlanet(0, 25);
  ASSERT_EQ(system.getElementCount(), 3);

  system.gravityPull(10); // Should remove 5, keep 15 and 25
  ASSERT_EQ(system.getElementCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, GravityPullAcrossAllPlanetTypes) {
  // Vector planet
  system.pushToPlanet(0, 3);
  system.pushToPlanet(0, 20);

  // Stack planet
  system.pushToPlanet(1, 7);
  system.pushToPlanet(1, 30);

  // Queue planet
  system.pushToPlanet(2, 1);
  system.pushToPlanet(2, 50);

  ASSERT_EQ(system.getElementCount(), 6);

  system.gravityPull(10); // Remove 3, 7, 1 — keep 20, 30, 50
  ASSERT_EQ(system.getElementCount(), 3);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, GravityPullRemovesNothing) {
  system.pushToPlanet(0, 100);
  system.pushToPlanet(1, 200);
  system.pushToPlanet(2, 300);

  system.gravityPull(10); // All above threshold
  ASSERT_EQ(system.getElementCount(), 3);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, GravityPullRemovesEverything) {
  system.pushToPlanet(0, 1);
  system.pushToPlanet(1, 2);
  system.pushToPlanet(2, 3);

  system.gravityPull(100); // All below threshold
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, GravityPullOnEmptyPlanets) {
  // No elements pushed — should not crash
  system.gravityPull(10);
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, GravityPullThresholdBoundary) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(0, 10);
  system.pushToPlanet(0, 9);

  system.gravityPull(10); // Should remove 9, keep both 10s (>= threshold)
  ASSERT_EQ(system.getElementCount(), 2);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

// ═══════════════════════════════════════════════════════════════
//  resetSystem (Supernova) Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(SolarSystemTest, ResetSystemClearsEverything) {
  system.pushToPlanet(0, 10);
  system.pushToPlanet(1, 20);
  system.pushToPlanet(2, 30);

  system.resetSystem();
  ASSERT_EQ(system.getPlanetCount(), 0);
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

TEST_F(SolarSystemTest, SupernovaAliasWorks) {
  system.pushToPlanet(0, 10);
  system.supernova();
  ASSERT_EQ(system.getPlanetCount(), 0);
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

// ═══════════════════════════════════════════════════════════════
//  Element Count Consistency — Compound Operations
// ═══════════════════════════════════════════════════════════════

TEST_F(SolarSystemTest, ElementCountConsistentAfterMixedOperations) {
  // Push elements
  system.pushToPlanet(0, 5);
  system.pushToPlanet(0, 15);
  system.pushToPlanet(0, 25);
  system.pushToPlanet(1, 10);
  system.pushToPlanet(1, 20);
  system.pushToPlanet(2, 30);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  // Delete one element
  system.deleteElement(0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  // Gravity pull
  system.gravityPull(12);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  // Remove a planet
  system.removePlanet(0);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  // Add new planet and push
  system.addPlanet(PlanetType::Vector);
  system.pushToPlanet(system.getPlanetCount() - 1, 99);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);

  // Final reset
  system.resetSystem();
  ASSERT_EQ(system.getElementCount(), 0);
  ASSERT_EQ(system.getActualElementCount(), 0);
}

TEST_F(SolarSystemTest, DeleteFromEmptyPlanetThrows) {
  ASSERT_THROW(system.deleteElement(0), std::underflow_error);
  ASSERT_ELEMENT_COUNT_CONSISTENT(system);
}

// ═══════════════════════════════════════════════════════════════
//  Entry Point
// ═══════════════════════════════════════════════════════════════

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
