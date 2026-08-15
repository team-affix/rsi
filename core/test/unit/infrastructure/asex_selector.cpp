#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_selector.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/population.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockEvaluate {
    MOCK_METHOD(double, evaluate, (const policy&));
};

using test_selector_t = asex_selector<MockEvaluate>;

struct AsexSelectorTest : public ::testing::Test {
    NiceMock<MockEvaluate> evaluate;
    test_selector_t selector{evaluate, 2};
    std::shared_ptr<expr> t0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> t2 = std::make_shared<expr>(expr{expr::var{2}});
    asex_agent a0{recursor{t0}, policy{t0}};
    asex_agent a1{recursor{t1}, policy{t1}};
    asex_agent a2{recursor{t2}, policy{t2}};
    population<asex_agent> pop{3};
};

TEST_F(AsexSelectorTest, EmitsOneContextPerTopG) {
    pop.agents.push_back(a0);
    pop.agents.push_back(a1);
    pop.agents.push_back(a2);
    EXPECT_CALL(evaluate, evaluate(a0.pol)).WillOnce(Return(1.0));
    EXPECT_CALL(evaluate, evaluate(a1.pol)).WillOnce(Return(3.0));
    EXPECT_CALL(evaluate, evaluate(a2.pol)).WillOnce(Return(2.0));
    std::vector<asex_reproduction_context> ctxs = selector.select(pop);
    ASSERT_EQ(ctxs.size(), 2u);
    EXPECT_EQ(ctxs[0].agent.pol.term, t1);
    EXPECT_EQ(ctxs[1].agent.pol.term, t2);
}
