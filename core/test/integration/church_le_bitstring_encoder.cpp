#include <memory>
#include <gtest/gtest.h>
#include "exprs_eq.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/rc_pool.hpp"
#include "value_objects/expr.hpp"

struct ChurchLeBitstringEncoderIntegrationTest : public ::testing::Test {
    using nodes_t = rc_pool<expr>;
    using factory_t = expr_factory<nodes_t>;
    using church_t = church<factory_t, factory_t>;
    using encoder_t = le_bitstring_encoder<church_t, church_t, factory_t, factory_t, factory_t>;
    ChurchLeBitstringEncoderIntegrationTest()
        : nodes()
        , factory(nodes)
        , ch(factory, factory)
        , encoder(ch, ch, factory, factory, factory) {
    }
    std::shared_ptr<expr> church_true() { return factory.make_abs(factory.make_abs(factory.make_var(1))); }
    std::shared_ptr<expr> church_false() {
        return factory.make_abs(factory.make_abs(factory.make_var(0)));
    }
    std::shared_ptr<expr> nil() { return factory.make_abs(factory.make_abs(factory.make_var(0))); }
    std::shared_ptr<expr> cons(std::shared_ptr<expr> h, std::shared_ptr<expr> t) {
        std::shared_ptr<expr> applied =
            factory.make_app(factory.make_app(factory.make_var(1), h), t);
        return factory.make_abs(factory.make_abs(applied));
    }
    nodes_t nodes;
    factory_t factory;
    church_t ch;
    encoder_t encoder;
};

TEST_F(ChurchLeBitstringEncoderIntegrationTest, ZeroIsNil) {
    EXPECT_TRUE(exprs_eq(encoder.encode_uint(0), nil()));
}

TEST_F(ChurchLeBitstringEncoderIntegrationTest, FiveIsLittleEndianScottList) {
    std::shared_ptr<expr> expected = cons(church_true(), cons(church_false(), cons(church_true(), nil())));
    EXPECT_TRUE(exprs_eq(encoder.encode_uint(5), expected));
}

TEST_F(ChurchLeBitstringEncoderIntegrationTest, BitsAreChurchBools) {
    EXPECT_TRUE(exprs_eq(ch.make_true(), church_true()));
    EXPECT_TRUE(exprs_eq(ch.make_false(), church_false()));
}
