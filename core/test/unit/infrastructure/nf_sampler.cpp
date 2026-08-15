#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/nf_sampler.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

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
    NiceMock<MockSampleUniform> sample_uniform;
    NiceMock<MockMakeAbs> make_abs;
    NiceMock<MockMakeApp> make_app;
    NiceMock<MockMakeVar> make_var;
    test_sampler_t sampler{sample_uniform, make_abs, make_app, make_var, 2};
    std::shared_ptr<expr> v = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::abs{v}});
};

TEST_F(NfSamplerTest, TwoNodesIsAbsVar) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillOnce(Return(0));
    EXPECT_CALL(make_var, make_var(0)).WillOnce(Return(v));
    EXPECT_CALL(make_abs, make_abs(v)).WillOnce(Return(result));
    EXPECT_EQ(sampler.sample(), result);
}
