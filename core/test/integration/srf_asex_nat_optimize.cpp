#include <gtest/gtest.h>
#include "infrastructure/srf_asex_manifest.hpp"
#include "infrastructure/srf_asex_runtime.hpp"
#include "nat_reward.hpp"

struct SrfAsexNatOptimizeIntegrationTest : public ::testing::Test {
    nat_reward reward{4, 13};
};

TEST_F(SrfAsexNatOptimizeIntegrationTest, InitializeThenStepsKeepN) {
    srf_asex_manifest<nat_reward> manifest{100, 2, 64, 100000, 16, 1, reward};
    manifest.initializer_.initialize();
    EXPECT_EQ(manifest.population_.size(), 100u);
    manifest.stepper_.step();
    EXPECT_EQ(manifest.population_.size(), 100u);
    manifest.stepper_.step();
    EXPECT_EQ(manifest.population_.size(), 100u);
}

TEST_F(SrfAsexNatOptimizeIntegrationTest, BestRewardMatchesEvaluateOfBestPolicy) {
    srf_asex_runtime<nat_reward> runtime{100, 2, 64, 100000, 16, 1, reward};
    runtime.initialize();
    for(int i = 0; i < 8; ++i) {
        runtime.step();
        EXPECT_EQ(runtime.best_reward(), reward.evaluate(runtime.best_policy()));
    }
}
