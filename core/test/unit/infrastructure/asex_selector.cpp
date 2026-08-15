#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_selector.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_selection.hpp"
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
    AsexSelectorTest();
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

AsexSelectorTest::AsexSelectorTest() {
    ON_CALL(get_size, size()).WillByDefault(Return(3));
    ON_CALL(get_agent, get(0)).WillByDefault(ReturnRef(a0));
    ON_CALL(get_agent, get(1)).WillByDefault(ReturnRef(a1));
    ON_CALL(get_agent, get(2)).WillByDefault(ReturnRef(a2));
    ON_CALL(evaluate, evaluate(a0.pol)).WillByDefault(Return(1.0));
    ON_CALL(evaluate, evaluate(a1.pol)).WillByDefault(Return(3.0));
    ON_CALL(evaluate, evaluate(a2.pol)).WillByDefault(Return(2.0));
}

TEST_F(AsexSelectorTest, ReturnsExactlyGProgenitorsBestFirst) {
    asex_selection selection = selector.select();
    ASSERT_EQ(selection.progenitors.size(), 2u);
    EXPECT_EQ(selection.progenitors[0].parent.pol.term, t1);
    EXPECT_EQ(selection.progenitors[1].parent.pol.term, t2);
    EXPECT_EQ(selection.best_reward, 3.0);
    EXPECT_EQ(selection.best_model.term, t1);
}
