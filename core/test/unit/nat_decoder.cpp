#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include "nat_decoder.hpp"
#include "value_objects/expr.hpp"

struct NatDecoderTest : public ::testing::Test {
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> abs_var = std::make_shared<expr>(expr{expr::abs{v}});
    std::shared_ptr<expr> abs_abs_var = std::make_shared<expr>(expr{expr::abs{abs_var}});
    std::shared_ptr<expr> app_v_v = std::make_shared<expr>(expr{expr::app{v, v}});
    std::shared_ptr<expr> abs_abs_abs_app =
        std::make_shared<expr>(expr{expr::abs{std::make_shared<expr>(
            expr{expr::abs{std::make_shared<expr>(expr{expr::abs{app_v_v}})}})}});
    std::shared_ptr<expr> abs_app_inner_abs =
        std::make_shared<expr>(expr{expr::app{abs_var, v}});
    std::shared_ptr<expr> abs_of_app_inner_abs =
        std::make_shared<expr>(expr{expr::abs{abs_app_inner_abs}});
};

TEST_F(NatDecoderTest, NullIsNotANat) {
    EXPECT_EQ(decode_nat(nullptr), std::nullopt);
}

TEST_F(NatDecoderTest, VarIsNotANat) {
    EXPECT_EQ(decode_nat(v.get()), std::nullopt);
}

TEST_F(NatDecoderTest, AppIsNotANat) {
    EXPECT_EQ(decode_nat(app_v_v.get()), std::nullopt);
}

TEST_F(NatDecoderTest, OneAbsIsZero) {
    EXPECT_EQ(decode_nat(abs_var.get()), 0u);
}

TEST_F(NatDecoderTest, TwoAbsIsOne) {
    EXPECT_EQ(decode_nat(abs_abs_var.get()), 1u);
}

TEST_F(NatDecoderTest, ThreeAbsThenAppIsTwo) {
    EXPECT_EQ(decode_nat(abs_abs_abs_app.get()), 2u);
}

TEST_F(NatDecoderTest, AbsOverAppStopsAtApp) {
    EXPECT_EQ(decode_nat(abs_of_app_inner_abs.get()), 0u);
}
