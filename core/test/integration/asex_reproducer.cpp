#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_child_generator.hpp"
#include "infrastructure/asex_reproducer.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/y_combinator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

struct AsexReproducerIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using church_t = church<factory_t, factory_t>;
    using y_t = y_combinator<factory_t, factory_t, factory_t>;
    using encoder_t = le_bitstring_encoder<church_t, church_t, factory_t, factory_t, factory_t>;
    using applicator_t = recursor_applicator<factory_t>;
    using normalizer_t = bounded_normalizer<lc_runtime_maker>;
    using gen_t =
        asex_child_generator<MockSampleUniform, encoder_t, church_t, church_t, y_t, applicator_t,
                             normalizer_t>;
    using reproducer_t = asex_reproducer<gen_t>;
    AsexReproducerIntegrationTest()
        : expr_nodes()
        , out_nodes()
        , factory(expr_nodes)
        , ch(factory, factory)
        , y(factory, factory, factory)
        , encoder(ch, ch, factory, factory, factory)
        , applicator(factory)
        , runtime_maker(out_nodes)
        , normalizer(runtime_maker, 64, 100000)
        , gen(sample_uniform, encoder, ch, ch, y, applicator, normalizer, 3)
        , reproducer(gen, 2, 3)
        , flag_recursor{factory.make_abs(
              factory.make_abs(factory.make_abs(factory.make_var(2))))} {
        std::shared_ptr<expr> xx =
            factory.make_abs(factory.make_app(factory.make_var(0), factory.make_var(0)));
        omega_recursor = recursor{factory.make_app(xx, xx)};
        flag_agent = asex_agent{flag_recursor, policy{ch.make_false()}};
        omega_agent = asex_agent{omega_recursor, policy{ch.make_false()}};
    }
    NiceMock<MockSampleUniform> sample_uniform;
    nodes_t expr_nodes;
    nodes_t out_nodes;
    factory_t factory;
    church_t ch;
    y_t y;
    encoder_t encoder;
    applicator_t applicator;
    lc_runtime_maker runtime_maker;
    normalizer_t normalizer;
    gen_t gen;
    reproducer_t reproducer;
    recursor flag_recursor;
    recursor omega_recursor;
    asex_agent flag_agent;
    asex_agent omega_agent;
};

TEST_F(AsexReproducerIntegrationTest, FertileParentYieldsRChildren) {
    EXPECT_CALL(sample_uniform, sample_uniform(8)).WillRepeatedly(Return(5));
    std::vector<asex_agent> kids = reproducer.reproduce(asex_reproduction_context{flag_agent});
    EXPECT_EQ(kids.size(), 2u);
}

TEST_F(AsexReproducerIntegrationTest, OmegaBurnsRetriesKeepsNoKids) {
    EXPECT_CALL(sample_uniform, sample_uniform(8)).WillRepeatedly(Return(0));
    std::vector<asex_agent> kids = reproducer.reproduce(asex_reproduction_context{omega_agent});
    EXPECT_EQ(kids.size(), 0u);
}
