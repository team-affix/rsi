#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_child_generator.hpp"
#include "infrastructure/asex_reproducer.hpp"
#include "infrastructure/asex_selector.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/gap_fill.hpp"
#include "infrastructure/initial_agent_producer.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/nf_sampler.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/srf_stepper.hpp"
#include "infrastructure/y_combinator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/policy.hpp"
#include "value_objects/population.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockEvaluate {
    MOCK_METHOD(double, evaluate, (const policy&));
};

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

struct SrfAsexStepIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using church_t = church<factory_t, factory_t>;
    using y_t = y_combinator<factory_t, factory_t, factory_t>;
    using encoder_t = le_bitstring_encoder<church_t, church_t, factory_t, factory_t, factory_t>;
    using applicator_t = recursor_applicator<factory_t>;
    using sampler_t = nf_sampler<MockSampleUniform, factory_t, factory_t, factory_t>;
    using normalizer_t = bounded_normalizer<lc_runtime_maker>;
    using producer_t = initial_agent_producer<sampler_t>;
    using gen_t =
        asex_child_generator<MockSampleUniform, encoder_t, church_t, church_t, y_t, applicator_t,
                             normalizer_t>;
    using reproducer_t = asex_reproducer<gen_t>;
    using selector_t = asex_selector<MockEvaluate>;
    using stepper_t =
        srf_stepper<asex_agent, asex_reproduction_context, selector_t, reproducer_t, producer_t>;
    SrfAsexStepIntegrationTest()
        : expr_nodes()
        , out_nodes()
        , factory(expr_nodes)
        , ch(factory, factory)
        , y(factory, factory, factory)
        , encoder(ch, ch, factory, factory, factory)
        , applicator(factory)
        , sampler(sample_uniform, factory, factory, factory, 2)
        , runtime_maker(out_nodes)
        , normalizer(runtime_maker, 8, 100000)
        , producer(sampler)
        , gen(sample_uniform, encoder, ch, ch, y, applicator, normalizer, 3)
        , reproducer(gen, 2, 2)
        , selector(evaluate, 2)
        , stepper(selector, reproducer, producer)
        , pop(4) {
    }
    NiceMock<MockEvaluate> evaluate;
    NiceMock<MockSampleUniform> sample_uniform;
    nodes_t expr_nodes;
    nodes_t out_nodes;
    factory_t factory;
    church_t ch;
    y_t y;
    encoder_t encoder;
    applicator_t applicator;
    sampler_t sampler;
    lc_runtime_maker runtime_maker;
    normalizer_t normalizer;
    producer_t producer;
    gen_t gen;
    reproducer_t reproducer;
    selector_t selector;
    stepper_t stepper;
    population<asex_agent> pop;
};

TEST_F(SrfAsexStepIntegrationTest, EmptyInReachesN) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    ON_CALL(evaluate, evaluate).WillByDefault(Return(0.0));
    population<asex_agent> out = stepper.step(pop);
    EXPECT_EQ(out.agents.size(), 4u);
}

TEST_F(SrfAsexStepIntegrationTest, SecondStepDropsParentsAndRestoresN) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(8)).WillRepeatedly(Return(0));
    ON_CALL(evaluate, evaluate).WillByDefault(Return(1.0));
    population<asex_agent> first = stepper.step(pop);
    ASSERT_EQ(first.agents.size(), 4u);
    population<asex_agent> second = stepper.step(first);
    EXPECT_EQ(second.agents.size(), 4u);
}
