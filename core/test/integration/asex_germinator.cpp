#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include "exprs_eq.hpp"
#include "infrastructure/asex_germinator.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/y_combinator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/asex_seed.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

struct AsexGerminatorIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using church_t = church<factory_t, factory_t>;
    using y_t = y_combinator<factory_t, factory_t, factory_t>;
    using encoder_t = le_bitstring_encoder<church_t, church_t, factory_t, factory_t, factory_t>;
    using applicator_t = recursor_applicator<factory_t>;
    using normalizer_t = bounded_normalizer<lc_runtime_maker>;
    using gen_t =
        asex_germinator<encoder_t, church_t, church_t, y_t, applicator_t, normalizer_t>;
    AsexGerminatorIntegrationTest()
        : expr_nodes()
        , out_nodes()
        , factory(expr_nodes)
        , ch(factory, factory)
        , y(factory, factory, factory)
        , encoder(ch, ch, factory, factory, factory)
        , applicator(factory)
        , runtime_maker(out_nodes)
        , normalizer(runtime_maker, 64, 100000)
        , gen(encoder, ch, ch, y, applicator, normalizer)
        , flag_recursor{factory.make_abs(factory.make_abs(
              factory.make_abs(factory.make_abs(factory.make_var(3)))))} {
        std::shared_ptr<expr> xx = factory.make_abs(factory.make_app(factory.make_var(0), factory.make_var(0)));
        omega_recursor = recursor{factory.make_app(xx, xx)};
    }
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
    recursor flag_recursor;
    recursor omega_recursor;
};

TEST_F(AsexGerminatorIntegrationTest, SameRandYieldsTwoNfs) {
    std::optional<asex_agent> child =
        gen.germinate(
            asex_seed{asex_progenitor{asex_agent{flag_recursor, policy{ch.make_false()}}}, 0, 5});
    ASSERT_TRUE(child.has_value());
    EXPECT_TRUE(exprs_eq(child->rec.term, ch.make_true()));
    EXPECT_TRUE(exprs_eq(child->pol.term, ch.make_false()));
}

TEST_F(AsexGerminatorIntegrationTest, OmegaRecursorHitsBudget) {
    std::optional<asex_agent> child =
        gen.germinate(
            asex_seed{asex_progenitor{asex_agent{omega_recursor, policy{ch.make_false()}}}, 0, 0});
    EXPECT_FALSE(child.has_value());
}
