#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_reproducer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/asex_seed.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

using test_reproducer_t = asex_reproducer<MockSampleUniform>;

struct AsexReproducerTest : public ::testing::Test {
    NiceMock<MockSampleUniform> sample_uniform;
    test_reproducer_t reproducer{sample_uniform, 2};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_progenitor progenitor{asex_agent{recursor{t}, policy{t}}};
};

TEST_F(AsexReproducerTest, ReturnsExactlyRSeedsWithSampledRands) {
    EXPECT_CALL(sample_uniform, sample_uniform(256)).WillOnce(Return(1)).WillOnce(Return(2));
    std::vector<asex_seed> seeds = reproducer.reproduce(progenitor);
    ASSERT_EQ(seeds.size(), 2u);
    EXPECT_EQ(seeds[0].progenitor.parent.pol.term, t);
    EXPECT_EQ(seeds[0].index, 0u);
    EXPECT_EQ(seeds[0].rand, 1u);
    EXPECT_EQ(seeds[1].progenitor.parent.pol.term, t);
    EXPECT_EQ(seeds[1].index, 1u);
    EXPECT_EQ(seeds[1].rand, 2u);
}
