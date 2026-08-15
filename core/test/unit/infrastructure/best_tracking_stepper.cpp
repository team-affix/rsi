#include <limits>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/best_tracking_stepper.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/policy.hpp"
#include "value_objects/step_result.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockStep {
    MOCK_METHOD(step_result, step, ());
};

using test_tracker_t = best_tracking_stepper<MockStep>;

struct BestTrackingStepperTest : public ::testing::Test {
    NiceMock<MockStep> inner;
    test_tracker_t tracker{inner};
    std::shared_ptr<expr> t0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> t2 = std::make_shared<expr>(expr{expr::var{2}});
    policy p0{t0};
    policy p1{t1};
    policy p2{t2};
};

TEST_F(BestTrackingStepperTest, ForwardsInnerResult) {
    EXPECT_CALL(inner, step()).WillOnce(Return(step_result{1.5, p0, 7}));
    step_result result = tracker.step();
    EXPECT_EQ(result.best_reward, 1.5);
    EXPECT_EQ(result.best_policy.term, t0);
    EXPECT_EQ(result.viable_seed_count, 7u);
}

TEST_F(BestTrackingStepperTest, FirstStepBecomesBest) {
    EXPECT_CALL(inner, step()).WillOnce(Return(step_result{1.0, p0, 1}));
    tracker.step();
    EXPECT_EQ(tracker.best_reward(), 1.0);
    EXPECT_EQ(tracker.best_policy().term, t0);
}

TEST_F(BestTrackingStepperTest, HigherRewardUpdatesBest) {
    EXPECT_CALL(inner, step())
        .WillOnce(Return(step_result{1.0, p0, 1}))
        .WillOnce(Return(step_result{3.0, p1, 2}));
    tracker.step();
    tracker.step();
    EXPECT_EQ(tracker.best_reward(), 3.0);
    EXPECT_EQ(tracker.best_policy().term, t1);
}

TEST_F(BestTrackingStepperTest, EqualRewardKeepsPriorBest) {
    EXPECT_CALL(inner, step())
        .WillOnce(Return(step_result{2.0, p0, 1}))
        .WillOnce(Return(step_result{2.0, p1, 1}));
    tracker.step();
    tracker.step();
    EXPECT_EQ(tracker.best_reward(), 2.0);
    EXPECT_EQ(tracker.best_policy().term, t0);
}

TEST_F(BestTrackingStepperTest, LowerRewardKeepsPriorBest) {
    EXPECT_CALL(inner, step())
        .WillOnce(Return(step_result{1.0, p0, 1}))
        .WillOnce(Return(step_result{4.0, p1, 1}))
        .WillOnce(Return(step_result{0.0, p2, 1}));
    tracker.step();
    tracker.step();
    step_result result = tracker.step();
    EXPECT_EQ(result.best_reward, 0.0);
    EXPECT_EQ(result.best_policy.term, t2);
    EXPECT_EQ(tracker.best_reward(), 4.0);
    EXPECT_EQ(tracker.best_policy().term, t1);
}

TEST_F(BestTrackingStepperTest, StartsAtNegativeInfinity) {
    EXPECT_EQ(tracker.best_reward(), -std::numeric_limits<double>::infinity());
    EXPECT_EQ(tracker.best_policy().term, nullptr);
}
