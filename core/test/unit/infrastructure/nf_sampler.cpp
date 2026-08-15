#include <cstdint>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/nf_sampler.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
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

using test_sampler_t = nf_sampler<MockSampleUniform, MockMakeAbs, MockMakeApp, MockMakeVar>;

struct NfSamplerTest : public ::testing::Test {
    test_sampler_t make_sampler(uint64_t sample_nodes);
    NiceMock<MockSampleUniform> sample_uniform;
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeApp> make_app;
    NiceMock<MockMakeVar> make_var;
    test_sampler_t sampler{sample_uniform, make_abs, make_app, make_var, 2};
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> inner = std::make_shared<expr>(expr{expr::abs{v}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::abs{v}});
    std::shared_ptr<expr> applied = std::make_shared<expr>(expr{expr::app{v, v}});
};

test_sampler_t NfSamplerTest::make_sampler(uint64_t sample_nodes) {
    return test_sampler_t{sample_uniform, make_abs, make_app, make_var, sample_nodes};
}

TEST_F(NfSamplerTest, TwoNodesIsAbsVar) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(result));
    EXPECT_EQ(sampler.sample(), result);
}

TEST_F(NfSamplerTest, BudgetTwoPicksVar) {
    test_sampler_t s = make_sampler(3);
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillOnce(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, BudgetTwoPicksAbs) {
    test_sampler_t s = make_sampler(3);
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillOnce(Return(1)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(inner));
    EXPECT_CALL(make_abs, make_abs(inner)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksVar) {
    test_sampler_t s = make_sampler(4);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksAbs) {
    test_sampler_t s = make_sampler(4);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(1));
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(inner));
    EXPECT_CALL(make_abs, make_abs(inner)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksApp) {
    test_sampler_t s = make_sampler(4);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(2));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(make_var, make_var(_)).WillRepeatedly(Return(v));
    EXPECT_CALL(make_app, make_app(_, _)).WillOnce(Return(applied));
    EXPECT_CALL(make_abs, make_abs(applied)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleAppSplitsBudget) {
    test_sampler_t s = make_sampler(5);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(2));
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillOnce(Return(1));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(make_var, make_var(_)).WillRepeatedly(Return(v));
    EXPECT_CALL(make_app, make_app(_, _)).WillOnce(Return(applied));
    EXPECT_CALL(make_abs, make_abs(applied)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleNeCoinFlipPicksVar) {
    test_sampler_t s = make_sampler(7);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(2));
    EXPECT_CALL(sample_uniform, sample_uniform(4)).WillOnce(Return(2));
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillRepeatedly(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(make_var, make_var(_)).WillRepeatedly(Return(v));
    EXPECT_CALL(make_app, make_app(_, _)).Times(1).WillOnce(Return(applied));
    EXPECT_CALL(make_abs, make_abs(applied)).WillOnce(Return(result));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleNeCoinFlipPicksApp) {
    test_sampler_t s = make_sampler(7);
    EXPECT_CALL(sample_uniform, sample_uniform(3)).WillOnce(Return(2)).WillRepeatedly(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(4)).WillOnce(Return(2));
    EXPECT_CALL(sample_uniform, sample_uniform(2)).WillRepeatedly(Return(1));
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(make_var, make_var(_)).WillRepeatedly(Return(v));
    EXPECT_CALL(make_app, make_app(_, _)).Times(2).WillRepeatedly(Return(applied));
    EXPECT_CALL(make_abs, make_abs(_)).WillRepeatedly(Return(result));
    EXPECT_EQ(s.sample(), result);
}