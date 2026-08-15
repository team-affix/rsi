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
    std::shared_ptr<expr> bit_t = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> bit_f = std::make_shared<expr>(expr{expr::var{3}});
    std::shared_ptr<expr> v1 = std::make_shared<expr>(expr{expr::var{1}});
};

TEST_F(LeBitstringEncoderTest, ZeroIsNil) {
    EXPECT_CALL(make_true, make_true()).Times(0);
    EXPECT_CALL(make_false, make_false()).Times(0);
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v0));
    EXPECT_CALL(make_abs, make_abs(v0)).WillOnce(Return(inner_nil));
    EXPECT_CALL(make_abs, make_abs(inner_nil)).WillOnce(Return(nil));
    EXPECT_EQ(encoder.encode_uint(0), nil);
}

TEST_F(LeBitstringEncoderTest, OneIsConsTrueNil) {
    std::shared_ptr<expr> app_c_t = std::make_shared<expr>(expr{expr::app{v1, bit_t}});
    std::shared_ptr<expr> applied = std::make_shared<expr>(expr{expr::app{app_c_t, nil}});
    std::shared_ptr<expr> inner_cons = std::make_shared<expr>(expr{expr::abs{applied}});
    std::shared_ptr<expr> cons = std::make_shared<expr>(expr{expr::abs{inner_cons}});
    EXPECT_CALL(make_false, make_false()).Times(0);
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v0));
    EXPECT_CALL(make_abs, make_abs(v0)).WillOnce(Return(inner_nil));
    EXPECT_CALL(make_abs, make_abs(inner_nil)).WillOnce(Return(nil));
    EXPECT_CALL(make_true, make_true()).WillOnce(Return(bit_t));
    EXPECT_CALL(make_var, make_var(1)).WillOnce(Return(v1));
    EXPECT_CALL(make_app, make_app(v1, bit_t)).WillOnce(Return(app_c_t));
    EXPECT_CALL(make_app, make_app(app_c_t, nil)).WillOnce(Return(applied));
    EXPECT_CALL(make_abs, make_abs(applied)).WillOnce(Return(inner_cons));
    EXPECT_CALL(make_abs, make_abs(inner_cons)).WillOnce(Return(cons));
    EXPECT_EQ(encoder.encode_uint(1), cons);
}

TEST_F(LeBitstringEncoderTest, TwoIsConsFalseConsTrueNil) {
    std::shared_ptr<expr> app_c_t = std::make_shared<expr>(expr{expr::app{v1, bit_t}});
    std::shared_ptr<expr> applied_t = std::make_shared<expr>(expr{expr::app{app_c_t, nil}});
    std::shared_ptr<expr> inner_cons_t = std::make_shared<expr>(expr{expr::abs{applied_t}});
    std::shared_ptr<expr> cons_t = std::make_shared<expr>(expr{expr::abs{inner_cons_t}});
    std::shared_ptr<expr> app_c_f = std::make_shared<expr>(expr{expr::app{v1, bit_f}});
    std::shared_ptr<expr> applied_f = std::make_shared<expr>(expr{expr::app{app_c_f, cons_t}});
    std::shared_ptr<expr> inner_cons_f = std::make_shared<expr>(expr{expr::abs{applied_f}});
    std::shared_ptr<expr> cons_f = std::make_shared<expr>(expr{expr::abs{inner_cons_f}});
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v0));
    EXPECT_CALL(make_abs, make_abs(v0)).WillOnce(Return(inner_nil));
    EXPECT_CALL(make_abs, make_abs(inner_nil)).WillOnce(Return(nil));
    EXPECT_CALL(make_true, make_true()).WillOnce(Return(bit_t));
    EXPECT_CALL(make_var, make_var(1)).WillOnce(Return(v1)).WillOnce(Return(v1));
    EXPECT_CALL(make_app, make_app(v1, bit_t)).WillOnce(Return(app_c_t));
    EXPECT_CALL(make_app, make_app(app_c_t, nil)).WillOnce(Return(applied_t));
    EXPECT_CALL(make_abs, make_abs(applied_t)).WillOnce(Return(inner_cons_t));
    EXPECT_CALL(make_abs, make_abs(inner_cons_t)).WillOnce(Return(cons_t));
    EXPECT_CALL(make_false, make_false()).WillOnce(Return(bit_f));
    EXPECT_CALL(make_app, make_app(v1, bit_f)).WillOnce(Return(app_c_f));
    EXPECT_CALL(make_app, make_app(app_c_f, cons_t)).WillOnce(Return(applied_f));
    EXPECT_CALL(make_abs, make_abs(applied_f)).WillOnce(Return(inner_cons_f));
    EXPECT_CALL(make_abs, make_abs(inner_cons_f)).WillOnce(Return(cons_f));
    EXPECT_EQ(encoder.encode_uint(2), cons_f);
}
