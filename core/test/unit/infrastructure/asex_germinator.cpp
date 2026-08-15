#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_germinator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/asex_seed.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockEncodeUint {
    MOCK_METHOD(std::shared_ptr<expr>, encode_uint, (uint64_t));
};

struct MockMakeTrue {
    MOCK_METHOD(std::shared_ptr<expr>, make_true, ());
};

struct MockMakeFalse {
    MOCK_METHOD(std::shared_ptr<expr>, make_false, ());
};

struct MockMakeCombinator {
    MOCK_METHOD(std::shared_ptr<expr>, make_combinator, ());
};

struct MockApplyRecursor {
    MOCK_METHOD(std::shared_ptr<expr>, apply,
                (const recursor&, std::shared_ptr<expr>, std::shared_ptr<expr>,
                 std::shared_ptr<expr>, std::shared_ptr<expr>));
};

struct MockNormalize {
    MOCK_METHOD((std::optional<std::shared_ptr<expr>>), normalize, (std::shared_ptr<expr>));
};

using test_gen_t = asex_germinator<MockEncodeUint, MockMakeTrue, MockMakeFalse, MockMakeCombinator,
                                   MockApplyRecursor, MockNormalize>;

struct AsexGerminatorTest : public ::testing::Test {
    AsexGerminatorTest();
    NiceMock<MockEncodeUint> encode_uint;
    NiceMock<MockMakeTrue> make_true;
    NiceMock<MockMakeFalse> make_false;
    NiceMock<MockMakeCombinator> make_combinator;
    NiceMock<MockApplyRecursor> apply_recursor;
    NiceMock<MockNormalize> normalize;
    std::shared_ptr<expr> rec_term;
    recursor rec;
    asex_seed seed;
    std::shared_ptr<expr> index;
    std::shared_ptr<expr> rand;
    std::shared_ptr<expr> y;
    std::shared_ptr<expr> flag_t;
    std::shared_ptr<expr> flag_f;
    std::shared_ptr<expr> rec_app;
    std::shared_ptr<expr> pol_app;
    std::shared_ptr<expr> rec_nf;
    std::shared_ptr<expr> pol_nf;
    std::optional<test_gen_t> gen;
};

AsexGerminatorTest::AsexGerminatorTest()
    : encode_uint()
    , make_true()
    , make_false()
    , make_combinator()
    , apply_recursor()
    , normalize()
    , rec_term(std::make_shared<expr>(expr{expr::var{0}}))
    , rec{rec_term}
    , seed{asex_progenitor{asex_agent{rec, policy{rec_term}}}, 2, 5}
    , index(std::make_shared<expr>(expr{expr::var{9}}))
    , rand(std::make_shared<expr>(expr{expr::var{1}}))
    , y(std::make_shared<expr>(expr{expr::var{2}}))
    , flag_t(std::make_shared<expr>(expr{expr::var{3}}))
    , flag_f(std::make_shared<expr>(expr{expr::var{4}}))
    , rec_app(std::make_shared<expr>(expr{expr::var{5}}))
    , pol_app(std::make_shared<expr>(expr{expr::var{6}}))
    , rec_nf(std::make_shared<expr>(expr{expr::var{7}}))
    , pol_nf(std::make_shared<expr>(expr{expr::var{8}}))
    , gen() {
    ON_CALL(make_true, make_true()).WillByDefault(Return(flag_t));
    ON_CALL(make_false, make_false()).WillByDefault(Return(flag_f));
    ON_CALL(make_combinator, make_combinator()).WillByDefault(Return(y));
    gen.emplace(encode_uint, make_true, make_false, make_combinator, apply_recursor, normalize);
}

TEST_F(AsexGerminatorTest, EncodesIndexAndRandForBothHalves) {
    EXPECT_CALL(make_true, make_true()).Times(0);
    EXPECT_CALL(make_false, make_false()).Times(0);
    EXPECT_CALL(make_combinator, make_combinator()).Times(0);
    EXPECT_CALL(encode_uint, encode_uint(2)).WillOnce(Return(index));
    EXPECT_CALL(encode_uint, encode_uint(5)).WillOnce(Return(rand));
    EXPECT_CALL(apply_recursor, apply(rec, flag_t, y, index, rand)).WillOnce(Return(rec_app));
    EXPECT_CALL(apply_recursor, apply(rec, flag_f, y, index, rand)).WillOnce(Return(pol_app));
    EXPECT_CALL(normalize, normalize(rec_app)).WillOnce(Return(rec_nf));
    EXPECT_CALL(normalize, normalize(pol_app)).WillOnce(Return(pol_nf));
    std::optional<asex_agent> child = gen->germinate(seed);
    ASSERT_TRUE(child.has_value());
    EXPECT_EQ(child->rec.term, rec_nf);
    EXPECT_EQ(child->pol.term, pol_nf);
}

TEST_F(AsexGerminatorTest, NulloptIfEitherNormalizeFails) {
    EXPECT_CALL(encode_uint, encode_uint(2)).WillOnce(Return(index));
    EXPECT_CALL(encode_uint, encode_uint(5)).WillOnce(Return(rand));
    EXPECT_CALL(apply_recursor, apply(rec, flag_t, y, index, rand)).WillOnce(Return(rec_app));
    EXPECT_CALL(apply_recursor, apply(rec, flag_f, y, index, rand)).WillOnce(Return(pol_app));
    EXPECT_CALL(normalize, normalize(rec_app)).WillOnce(Return(std::nullopt));
    std::optional<asex_agent> child = gen->germinate(seed);
    EXPECT_FALSE(child.has_value());
}
