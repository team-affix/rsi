#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_selector.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

struct MockGetSize {
    MOCK_METHOD(std::size_t, size, (), (const));
};

struct MockGetAgent {
    MOCK_METHOD(const asex_agent&, get, (std::size_t), (const));
};

struct MockEvaluate {
    MOCK_METHOD(double, evaluate, (const policy&));
};

using test_selector_t = asex_selector<MockGetSize, MockGetAgent, MockEvaluate>;

struct AsexSelectorTest : public ::testing::Test {
    NiceMock<MockGetSize> get_size;
    NiceMock<MockGetAgent> get_agent;
    NiceMock<MockEvaluate> evaluate;
    test_selector_t selector{get_size, get_agent, evaluate, 2};
    std::shared_ptr<expr> t0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> t2 = std::make_shared<expr>(expr{expr::var{2}});
    asex_agent a0{recursor{t0}, policy{t0}};
    asex_agent a1{recursor{t1}, policy{t1}};
    asex_agent a2{recursor{t2}, policy{t2}};
};

TEST_F(AsexSelectorTest, ReturnsExactlyGProgenitorsBestFirst) {
    EXPECT_CALL(get_size, size()).WillOnce(Return(3));
    EXPECT_CALL(get_agent, get(0)).WillRepeatedly(ReturnRef(a0));
    EXPECT_CALL(get_agent, get(1)).WillRepeatedly(ReturnRef(a1));
    EXPECT_CALL(get_agent, get(2)).WillRepeatedly(ReturnRef(a2));
    EXPECT_CALL(evaluate, evaluate(a0.pol)).WillOnce(Return(1.0));
    EXPECT_CALL(evaluate, evaluate(a1.pol)).WillOnce(Return(3.0));
    EXPECT_CALL(evaluate, evaluate(a2.pol)).WillOnce(Return(2.0));
    std::vector<asex_progenitor> progenitors = selector.select();
    ASSERT_EQ(progenitors.size(), 2u);
    EXPECT_EQ(progenitors[0].parent.pol.term, t1);
    EXPECT_EQ(progenitors[1].parent.pol.term, t2);
}
