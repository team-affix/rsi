#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/le_bitstring_encoder.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockMakeTrue {
    MOCK_METHOD(std::shared_ptr<expr>, make_true, ());
};

struct MockMakeFalse {
    MOCK_METHOD(std::shared_ptr<expr>, make_false, ());
};

struct MockMakeAbs {
    MOCK_METHOD(std::shared_ptr<expr>, make_abs, (std::shared_ptr<expr>));
};

struct MockMakeApp {
    MOCK_METHOD(std::shared_ptr<expr>, make_app, (std::shared_ptr<expr>, std::shared_ptr<expr>));
};

struct MockMakeVar {
    MOCK_METHOD(std::shared_ptr<expr>, make_var, (uint32_t));
};

using test_encoder_t =
    le_bitstring_encoder<MockMakeTrue, MockMakeFalse, MockMakeAbs, MockMakeApp, MockMakeVar>;

struct LeBitstringEncoderTest : public ::testing::Test {
    NiceMock<MockMakeTrue> make_true;
    NiceMock<MockMakeFalse> make_false;
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeApp> make_app;
    NiceMock<MockMakeVar> make_var;
    test_encoder_t encoder{make_true, make_false, make_abs, make_app, make_var};
    std::shared_ptr<expr> v0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> inner_nil = std::make_shared<expr>(expr{expr::abs{v0}});
    std::shared_ptr<expr> nil = std::make_shared<expr>(expr{expr::abs{inner_nil}});
};

TEST_F(LeBitstringEncoderTest, ZeroIsNil) {
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v0));
    EXPECT_CALL(make_abs, make_abs(v0)).WillOnce(Return(inner_nil));
    EXPECT_CALL(make_abs, make_abs(inner_nil)).WillOnce(Return(nil));
    EXPECT_EQ(encoder.encode_uint(0), nil);
}

TEST_F(LeBitstringEncoderTest, FiveUsesTrueFalseTrueBits) {
    std::shared_ptr<expr> dummy = std::make_shared<expr>(expr{expr::var{9}});
    ON_CALL(make_var, make_var).WillByDefault(Return(dummy));
    ON_CALL(make_abs, make_abs).WillByDefault(Return(dummy));
    ON_CALL(make_app, make_app).WillByDefault(Return(dummy));
    ON_CALL(make_true, make_true()).WillByDefault(Return(dummy));
    ON_CALL(make_false, make_false()).WillByDefault(Return(dummy));
    EXPECT_CALL(make_true, make_true()).Times(2);
    EXPECT_CALL(make_false, make_false()).Times(1);
    encoder.encode_uint(5);
}
