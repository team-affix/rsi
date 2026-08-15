#include <cstdint>
#include <gtest/gtest.h>
#include "infrastructure/uniform_rng.hpp"

struct UniformRngTest : public ::testing::Test {
};

TEST_F(UniformRngTest, SampleUniformInRange) {
    uniform_rng rng{1};
    for(int i = 0; i < 100; ++i) {
        EXPECT_LT(rng.sample_uniform(7), 7u);
        EXPECT_LT(rng.sample_uniform(1), 1u);
        EXPECT_LT(rng.sample_uniform(256), 256u);
    }
}

TEST_F(UniformRngTest, SameSeedSameSequence) {
    uniform_rng a{42};
    uniform_rng b{42};
    for(int i = 0; i < 20; ++i)
        EXPECT_EQ(a.sample_uniform(100), b.sample_uniform(100));
}