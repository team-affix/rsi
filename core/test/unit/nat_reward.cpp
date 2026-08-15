#include <cstdint>
#include <memory>
#include <gtest/gtest.h>
#include "nat_reward.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/policy.hpp"

struct NatRewardTest : public ::testing::Test {
    std::shared_ptr<expr> nat_term(uint64_t n);
    nat_reward reward{4, 13};
};

std::shared_ptr<expr> NatRewardTest::nat_term(uint64_t n) {
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    uint64_t leading_abs = n + 1;
    for(uint64_t i = 0; i < leading_abs; ++i)
        t = std::make_shared<expr>(expr{expr::abs{t}});
    return t;
}

TEST_F(NatRewardTest, InvalidTermScoresZero) {
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    EXPECT_EQ(reward.evaluate(policy{v}), 0.0);
}

TEST_F(NatRewardTest, PeakBeatsNeighbors) {
    double at_peak = reward.evaluate(policy{nat_term(4)});
    double below = reward.evaluate(policy{nat_term(3)});
    double above = reward.evaluate(policy{nat_term(5)});
    EXPECT_EQ(at_peak, 16.0);
    EXPECT_GT(at_peak, below);
    EXPECT_GT(at_peak, above);
}

TEST_F(NatRewardTest, JackpotBeatsPeak) {
    double at_peak = reward.evaluate(policy{nat_term(4)});
    double at_jackpot = reward.evaluate(policy{nat_term(13)});
    EXPECT_EQ(at_jackpot, 26.0);
    EXPECT_GT(at_jackpot, at_peak);
}

TEST_F(NatRewardTest, OvershootScoresBelowPeak) {
    double at_peak = reward.evaluate(policy{nat_term(4)});
    double far = reward.evaluate(policy{nat_term(20)});
    EXPECT_LT(far, at_peak);
}
