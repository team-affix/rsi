#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "exprs_eq.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/nf_sampler.hpp"
#include "infrastructure/rc_pool.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

struct NfSamplerNormalizerIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using sampler_t = nf_sampler<MockSampleUniform, factory_t, factory_t, factory_t>;
    using normalizer_t = bounded_normalizer<lc_runtime_maker>;
    NfSamplerNormalizerIntegrationTest()
        : expr_nodes()
        , out_nodes()
        , factory(expr_nodes)
        , sampler(sample_uniform, factory, factory, factory, 2)
        , runtime_maker(out_nodes)
        , normalizer(runtime_maker, 32, 100000) {
    }
    NiceMock<MockSampleUniform> sample_uniform;
    nodes_t expr_nodes;
    nodes_t out_nodes;
    factory_t factory;
    sampler_t sampler;
    lc_runtime_maker runtime_maker;
    normalizer_t normalizer;
};

TEST_F(NfSamplerNormalizerIntegrationTest, SampledAbsVarIsAlreadyNf) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillOnce(Return(0));
    std::shared_ptr<expr> sampled = sampler.sample();
    std::optional<std::shared_ptr<expr>> nf = normalizer.normalize(sampled);
    ASSERT_TRUE(nf.has_value());
    EXPECT_TRUE(exprs_eq(*nf, sampled));
}
