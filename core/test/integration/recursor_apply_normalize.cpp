#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include "exprs_eq.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/y_combinator.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

struct RecursorApplyNormalizeIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using church_t = church<factory_t, factory_t>;
    using y_t = y_combinator<factory_t, factory_t, factory_t>;
    using applicator_t = recursor_applicator<factory_t>;
    using normalizer_t = bounded_normalizer<lc_runtime_maker>;
    RecursorApplyNormalizeIntegrationTest()
        : expr_nodes()
        , out_nodes()
        , factory(expr_nodes)
        , ch(factory, factory)
        , y(factory, factory, factory)
        , applicator(factory)
        , runtime_maker(out_nodes)
        , normalizer(runtime_maker, 64, 100000)
        , flag_recursor{factory.make_abs(
              factory.make_abs(factory.make_abs(factory.make_var(2))))} {
    }
    nodes_t expr_nodes;
    nodes_t out_nodes;
    factory_t factory;
    church_t ch;
    y_t y;
    applicator_t applicator;
    lc_runtime_maker runtime_maker;
    normalizer_t normalizer;
    recursor flag_recursor;
};

TEST_F(RecursorApplyNormalizeIntegrationTest, FlagRecursorTrueReducesToTrue) {
    std::shared_ptr<expr> applied =
        applicator.apply(flag_recursor, ch.make_true(), y.make_combinator(), ch.make_false());
    std::optional<std::shared_ptr<expr>> nf = normalizer.normalize(applied);
    ASSERT_TRUE(nf.has_value());
    EXPECT_TRUE(exprs_eq(*nf, ch.make_true()));
}

TEST_F(RecursorApplyNormalizeIntegrationTest, FlagRecursorFalseReducesToFalse) {
    std::shared_ptr<expr> applied =
        applicator.apply(flag_recursor, ch.make_false(), y.make_combinator(), ch.make_true());
    std::optional<std::shared_ptr<expr>> nf = normalizer.normalize(applied);
    ASSERT_TRUE(nf.has_value());
    EXPECT_TRUE(exprs_eq(*nf, ch.make_false()));
}
