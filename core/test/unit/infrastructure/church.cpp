#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/church.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockMakeAbs {
    MOCK_METHOD(std::shared_ptr<expr>, make_abs, (std::shared_ptr<expr>));
};

struct MockMakeVar {
    MOCK_METHOD(std::shared_ptr<expr>, make_var, (uint32_t));
};

using test_church_t = church<MockMakeAbs, MockMakeVar>;

struct ChurchTest : public ::testing::Test {
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeVar> make_var;
    test_church_t ch{make_abs, make_var};
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> inner = std::make_shared<expr>(expr{expr::abs{v}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::abs{inner}});
};

TEST_F(ChurchTest, MakeTrueIsAbsAbsVar1) {
    EXPECT_CALL(make_var, make_var(1)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(inner));
    EXPECT_CALL(make_abs, make_abs(inner)).WillOnce(Return(result));
    EXPECT_EQ(ch.make_true(), result);
}

TEST_F(ChurchTest, MakeFalseIsAbsAbsVar0) {
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(inner));
    EXPECT_CALL(make_abs, make_abs(inner)).WillOnce(Return(result));
    EXPECT_EQ(ch.make_false(), result);
}
