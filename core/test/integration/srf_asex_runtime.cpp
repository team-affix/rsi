#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/srf_asex_manifest.hpp"
#include "infrastructure/srf_asex_runtime.hpp"
#include "value_objects/policy.hpp"
#include "value_objects/step_result.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockEvaluate {
    MOCK_METHOD(double, evaluate, (const policy&));
};

struct SrfAsexRuntimeIntegrationTest : public ::testing::Test {
    NiceMock<MockEvaluate> evaluate;
};

TEST_F(SrfAsexRuntimeIntegrationTest, StartsEmpty) {
    srf_asex_manifest<MockEvaluate> manifest{4, 2, 8, 100000, 2, 1, evaluate};
    EXPECT_EQ(manifest.population_.size(), 0u);
    EXPECT_EQ(manifest.population_.capacity(), 4u);
}

TEST_F(SrfAsexRuntimeIntegrationTest, InitializeThenStepsKeepN) {
    ON_CALL(evaluate, evaluate).WillByDefault(Return(0.0));
    srf_asex_manifest<MockEvaluate> manifest{4, 2, 8, 100000, 2, 1, evaluate};
    manifest.initializer_.initialize();
    EXPECT_EQ(manifest.population_.size(), 4u);
    manifest.stepper_.step();
    EXPECT_EQ(manifest.population_.size(), 4u);
    manifest.stepper_.step();
    EXPECT_EQ(manifest.population_.size(), 4u);
}

TEST_F(SrfAsexRuntimeIntegrationTest, StepReturnsDetailsAndTracksBest) {
    ON_CALL(evaluate, evaluate).WillByDefault(Return(1.0));
    srf_asex_runtime<MockEvaluate> runtime{4, 2, 8, 100000, 2, 1, evaluate};
    runtime.initialize();
    step_result first = runtime.step();
    EXPECT_EQ(first.best_reward, 1.0);
    EXPECT_EQ(runtime.best_reward(), 1.0);
    EXPECT_EQ(runtime.best_policy().term, first.best_model.term);
    ON_CALL(evaluate, evaluate).WillByDefault(Return(3.0));
    step_result second = runtime.step();
    EXPECT_EQ(second.best_reward, 3.0);
    EXPECT_EQ(runtime.best_reward(), 3.0);
    EXPECT_EQ(runtime.best_policy().term, second.best_model.term);
    ON_CALL(evaluate, evaluate).WillByDefault(Return(0.0));
    step_result third = runtime.step();
    EXPECT_EQ(third.best_reward, 0.0);
    EXPECT_EQ(runtime.best_reward(), 3.0);
    EXPECT_EQ(runtime.best_policy().term, second.best_model.term);
}
