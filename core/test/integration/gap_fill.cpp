#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/gap_fill.hpp"
#include "infrastructure/initial_agent_producer.hpp"
#include "infrastructure/nf_sampler.hpp"
#include "infrastructure/rc_pool.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/population.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

struct GapFillIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using sampler_t = nf_sampler<MockSampleUniform, factory_t, factory_t, factory_t>;
    using producer_t = initial_agent_producer<sampler_t>;
    using fill_t = gap_fill<asex_agent, producer_t>;
    GapFillIntegrationTest()
        : nodes()
        , factory(nodes)
        , sampler(sample_uniform, factory, factory, factory, 2)
        , producer(sampler)
        , fill(producer)
        , pop(3) {
    }
    NiceMock<MockSampleUniform> sample_uniform;
    nodes_t nodes;
    factory_t factory;
    sampler_t sampler;
    producer_t producer;
    fill_t fill;
    population<asex_agent> pop;
};

TEST_F(GapFillIntegrationTest, EmptyBecomesNClosedAgents) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    fill.fill(pop);
    EXPECT_EQ(pop.agents.size(), 3u);
    for(const asex_agent& agent : pop.agents) {
        ASSERT_TRUE(agent.rec.term);
        ASSERT_TRUE(agent.pol.term);
    }
}
