#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/gap_fill.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/population.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockProduceInitialAgent {
    MOCK_METHOD(asex_agent, produce, ());
};

using test_gap_fill_t = gap_fill<asex_agent, MockProduceInitialAgent>;

struct GapFillTest : public ::testing::Test {
    NiceMock<MockProduceInitialAgent> produce;
    test_gap_fill_t fill{produce};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_agent agent{recursor{t}, policy{t}};
};

TEST_F(GapFillTest, EmptyFillsToCapacity) {
    population<asex_agent> pop{2};
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent)).WillOnce(Return(agent));
    fill.fill(pop);
    EXPECT_EQ(pop.agents.size(), 2u);
}

TEST_F(GapFillTest, ShortfallOnly) {
    population<asex_agent> pop{2};
    pop.agents.push_back(agent);
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent));
    fill.fill(pop);
    EXPECT_EQ(pop.agents.size(), 2u);
}

TEST_F(GapFillTest, AlreadyFullIsNoOp) {
    population<asex_agent> pop{1};
    pop.agents.push_back(agent);
    EXPECT_CALL(produce, produce()).Times(0);
    fill.fill(pop);
    EXPECT_EQ(pop.agents.size(), 1u);
}
