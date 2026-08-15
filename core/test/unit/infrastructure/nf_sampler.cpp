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
    NfSamplerTest();
    test_sampler_t make_sampler(uint64_t sample_nodes);
    NiceMock<MockSampleUniform> sample_uniform;
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeApp> make_app;
    NiceMock<MockMakeVar> make_var;
    test_sampler_t sampler{sample_uniform, make_abs, make_app, make_var, 2};
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::abs{v}});
    std::shared_ptr<expr> applied = std::make_shared<expr>(expr{expr::app{v, v}});
};

NfSamplerTest::NfSamplerTest() {
    ON_CALL(make_var, make_var(_)).WillByDefault(Return(v));
    ON_CALL(make_abs, make_abs(_)).WillByDefault(Return(result));
    ON_CALL(make_app, make_app(_, _)).WillByDefault(Return(applied));
    ON_CALL(sample_uniform, sample_uniform(_)).WillByDefault(Return(0));
}

test_sampler_t NfSamplerTest::make_sampler(uint64_t sample_nodes) {
    return test_sampler_t{sample_uniform, make_abs, make_app, make_var, sample_nodes};
}

TEST_F(NfSamplerTest, TwoNodesIsAbsVar) {
    EXPECT_EQ(sampler.sample(), result);
}

TEST_F(NfSamplerTest, BudgetTwoPicksVar) {
    test_sampler_t s = make_sampler(3);
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, BudgetTwoPicksAbs) {
    test_sampler_t s = make_sampler(3);
    ON_CALL(sample_uniform, sample_uniform(2)).WillByDefault(Return(1));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksVar) {
    test_sampler_t s = make_sampler(4);
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksAbs) {
    test_sampler_t s = make_sampler(4);
    ON_CALL(sample_uniform, sample_uniform(3)).WillByDefault(Return(1));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, CanAppPicksApp) {
    test_sampler_t s = make_sampler(4);
    ON_CALL(sample_uniform, sample_uniform(3)).WillByDefault(Return(2));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleAppSplitsBudget) {
    test_sampler_t s = make_sampler(5);
    ON_CALL(sample_uniform, sample_uniform(3)).WillByDefault(Return(2));
    ON_CALL(sample_uniform, sample_uniform(2)).WillByDefault(Return(1));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleNeCoinFlipPicksVar) {
    test_sampler_t s = make_sampler(7);
    ON_CALL(sample_uniform, sample_uniform(3)).WillByDefault(Return(2));
    ON_CALL(sample_uniform, sample_uniform(4)).WillByDefault(Return(2));
    EXPECT_EQ(s.sample(), result);
}

TEST_F(NfSamplerTest, SampleNeCoinFlipPicksApp) {
    test_sampler_t s = make_sampler(7);
    ON_CALL(sample_uniform, sample_uniform(3)).WillByDefault(Return(2));
    ON_CALL(sample_uniform, sample_uniform(4)).WillByDefault(Return(2));
    ON_CALL(sample_uniform, sample_uniform(2)).WillByDefault(Return(1));
    EXPECT_EQ(s.sample(), result);
}
