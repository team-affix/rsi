#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_germinator.hpp"
#include "infrastructure/asex_reproducer.hpp"
#include "infrastructure/asex_selector.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/initial_agent_producer.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/next_generation_buffer.hpp"
#include "infrastructure/nf_sampler.hpp"
#include "infrastructure/population.hpp"
#include "infrastructure/population_initializer.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/srf_stepper.hpp"
#include "infrastructure/y_combinator.hpp"
#include "exprs_eq.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/policy.hpp"
#include "value_objects/recursor.hpp"
#include "value_objects/step_result.hpp"

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
    using population_t = population<asex_agent>;
    using gen_t =
        asex_germinator<encoder_t, church_t, church_t, y_t, applicator_t, normalizer_t>;
    using selector_t = asex_selector<population_t, population_t, MockEvaluate>;
    using reproducer_t = asex_reproducer<MockSampleUniform>;
    using initializer_t =
        population_initializer<population_t, population_t, population_t, producer_t>;
    using buffer_t = next_generation_buffer<asex_agent, population_t, population_t>;
    using stepper_t =
        srf_stepper<selector_t, reproducer_t, gen_t, producer_t, buffer_t, buffer_t, buffer_t>;
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
        , normalizer(runtime_maker, 64, 100000)
        , producer(sampler)
        , pop(4)
        , gen(encoder, ch, ch, y, applicator, normalizer)
        , selector(pop, pop, evaluate, 2)
        , reproducer(sample_uniform, 2)
        , initializer(pop, pop, pop, producer)
        , buffer(pop, pop, 4)
        , stepper(selector, reproducer, gen, producer, buffer, buffer, buffer) {
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
    population_t pop;
    gen_t gen;
    selector_t selector;
    reproducer_t reproducer;
    initializer_t initializer;
    buffer_t buffer;
    stepper_t stepper;
};

TEST_F(SrfAsexStepIntegrationTest, InitializeThenStepKeepsN) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(256)).WillRepeatedly(Return(0));
    ON_CALL(evaluate, evaluate).WillByDefault(Return(0.0));
    EXPECT_EQ(pop.size(), 0u);
    initializer.initialize();
    EXPECT_EQ(pop.size(), 4u);
    stepper.step();
    EXPECT_EQ(pop.size(), 4u);
}

TEST_F(SrfAsexStepIntegrationTest, OmegaLeavesHolesThenHoleFillRestoresN) {
    EXPECT_CALL(sample_uniform, sample_uniform(1)).WillRepeatedly(Return(0));
    EXPECT_CALL(sample_uniform, sample_uniform(256)).WillRepeatedly(Return(0));
    ON_CALL(evaluate, evaluate).WillByDefault(Return(1.0));
    std::shared_ptr<expr> xx =
        factory.make_abs(factory.make_app(factory.make_var(0), factory.make_var(0)));
    recursor omega{factory.make_app(xx, xx)};
    asex_agent omega_agent{omega, policy{ch.make_false()}};
    pop.add(omega_agent);
    pop.add(omega_agent);
    pop.add(omega_agent);
    pop.add(omega_agent);
    step_result result = stepper.step();
    EXPECT_EQ(pop.size(), 4u);
    EXPECT_EQ(result.best_reward, 1.0);
    EXPECT_TRUE(exprs_eq(result.best_policy.term, ch.make_false()));
    EXPECT_EQ(result.viable_seed_count, 0u);
}

TEST_F(SrfAsexStepIntegrationTest, FlagRecursorChildrenReplaceParents) {
    EXPECT_CALL(sample_uniform, sample_uniform(256)).WillRepeatedly(Return(0));
    ON_CALL(evaluate, evaluate).WillByDefault(Return(2.0));
    recursor flag_recursor{factory.make_abs(factory.make_abs(
        factory.make_abs(factory.make_abs(factory.make_var(3)))))};
    asex_agent parent{flag_recursor, policy{ch.make_false()}};
    pop.add(parent);
    pop.add(parent);
    pop.add(parent);
    pop.add(parent);
    step_result result = stepper.step();
    EXPECT_EQ(pop.size(), 4u);
    EXPECT_EQ(result.best_reward, 2.0);
    EXPECT_TRUE(exprs_eq(result.best_policy.term, ch.make_false()));
    EXPECT_EQ(result.viable_seed_count, 4u);
    EXPECT_FALSE(exprs_eq(pop.get(0).rec.term, flag_recursor.term));
    EXPECT_TRUE(exprs_eq(pop.get(0).rec.term, ch.make_true()));
    EXPECT_TRUE(exprs_eq(pop.get(0).pol.term, ch.make_false()));
    EXPECT_TRUE(exprs_eq(pop.get(1).rec.term, ch.make_true()));
    EXPECT_TRUE(exprs_eq(pop.get(2).rec.term, ch.make_true()));
    EXPECT_TRUE(exprs_eq(pop.get(3).rec.term, ch.make_true()));
}
