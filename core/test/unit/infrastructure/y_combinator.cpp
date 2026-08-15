#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/y_combinator.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockMakeAbs {
    MOCK_METHOD(std::shared_ptr<expr>, make_abs, (std::shared_ptr<expr>));
};

struct MockMakeApp {
    MOCK_METHOD(std::shared_ptr<expr>, make_app, (std::shared_ptr<expr>, std::shared_ptr<expr>));
};

struct MockMakeVar {
    MOCK_METHOD(std::shared_ptr<expr>, make_var, (uint32_t));
};

using test_y_t = y_combinator<MockMakeAbs, MockMakeApp, MockMakeVar>;

struct YCombinatorTest : public ::testing::Test {
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeApp> make_app;
    NiceMock<MockMakeVar> make_var;
    test_y_t y{make_abs, make_app, make_var};
    std::shared_ptr<expr> x = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> xx = std::make_shared<expr>(expr{expr::app{x, x}});
    std::shared_ptr<expr> f = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> fxx = std::make_shared<expr>(expr{expr::app{f, xx}});
    std::shared_ptr<expr> inner = std::make_shared<expr>(expr{expr::abs{fxx}});
    std::shared_ptr<expr> applied = std::make_shared<expr>(expr{expr::app{inner, inner}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::abs{applied}});
};

TEST_F(YCombinatorTest, MakeCombinatorIsY) {
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(x));
    EXPECT_CALL(make_app, make_app(x, x)).WillOnce(Return(xx));
    EXPECT_CALL(make_var, make_var(1)).WillOnce(Return(f));
    EXPECT_CALL(make_app, make_app(f, xx)).WillOnce(Return(fxx));
    EXPECT_CALL(make_abs, make_abs(fxx)).WillOnce(Return(inner));
    EXPECT_CALL(make_app, make_app(inner, inner)).WillOnce(Return(applied));
    EXPECT_CALL(make_abs, make_abs(applied)).WillOnce(Return(result));
    EXPECT_EQ(y.make_combinator(), result);
}
