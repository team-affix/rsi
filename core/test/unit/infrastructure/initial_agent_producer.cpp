#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/initial_agent_producer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleNf {
    MOCK_METHOD(std::shared_ptr<expr>, sample, ());
};

using test_producer_t = initial_agent_producer<MockSampleNf>;

struct InitialAgentProducerTest : public ::testing::Test {
    NiceMock<MockSampleNf> sample_nf;
    test_producer_t producer{sample_nf};
    std::shared_ptr<expr> rec_term = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> pol_term = std::make_shared<expr>(expr{expr::var{1}});
};

TEST_F(InitialAgentProducerTest, TwoSamplesBecomeRecursorAndPolicy) {
    EXPECT_CALL(sample_nf, sample()).WillOnce(Return(rec_term)).WillOnce(Return(pol_term));
    asex_agent agent = producer.produce();
    EXPECT_EQ(agent.rec.term, rec_term);
    EXPECT_EQ(agent.pol.term, pol_term);
}
