#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_child_generator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSampleUniform {
    MOCK_METHOD(uint64_t, sample_uniform, (uint64_t));
};

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
                 std::shared_ptr<expr>));
};

struct MockNormalize {
    MOCK_METHOD((std::optional<std::shared_ptr<expr>>), normalize, (std::shared_ptr<expr>));
};

using test_gen_t = asex_child_generator<MockSampleUniform, MockEncodeUint, MockMakeTrue,
                                        MockMakeFalse, MockMakeCombinator, MockApplyRecursor,
                                        MockNormalize>;

struct AsexChildGeneratorTest : public ::testing::Test {
    NiceMock<MockSampleUniform> sample_uniform;
    NiceMock<MockEncodeUint> encode_uint;
    NiceMock<MockMakeTrue> make_true;
    NiceMock<MockMakeFalse> make_false;
    NiceMock<MockMakeCombinator> make_combinator;
    NiceMock<MockApplyRecursor> apply_recursor;
    NiceMock<MockNormalize> normalize;
    test_gen_t gen{sample_uniform, encode_uint, make_true, make_false, make_combinator,
                   apply_recursor, normalize, 3};
    std::shared_ptr<expr> rec_term = std::make_shared<expr>(expr{expr::var{0}});
    recursor rec{rec_term};
    std::shared_ptr<expr> rand = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> y = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> flag_t = std::make_shared<expr>(expr{expr::var{3}});
    std::shared_ptr<expr> flag_f = std::make_shared<expr>(expr{expr::var{4}});
    std::shared_ptr<expr> rec_app = std::make_shared<expr>(expr{expr::var{5}});
    std::shared_ptr<expr> pol_app = std::make_shared<expr>(expr{expr::var{6}});
    std::shared_ptr<expr> rec_nf = std::make_shared<expr>(expr{expr::var{7}});
    std::shared_ptr<expr> pol_nf = std::make_shared<expr>(expr{expr::var{8}});
};

TEST_F(AsexChildGeneratorTest, SameRandForBothHalves) {
    EXPECT_CALL(sample_uniform, sample_uniform(8)).WillOnce(Return(5));
    EXPECT_CALL(encode_uint, encode_uint(5)).WillOnce(Return(rand));
    EXPECT_CALL(make_combinator, make_combinator()).WillOnce(Return(y));
    EXPECT_CALL(make_true, make_true()).WillOnce(Return(flag_t));
    EXPECT_CALL(make_false, make_false()).WillOnce(Return(flag_f));
    EXPECT_CALL(apply_recursor, apply(rec, flag_t, y, rand)).WillOnce(Return(rec_app));
    EXPECT_CALL(apply_recursor, apply(rec, flag_f, y, rand)).WillOnce(Return(pol_app));
    EXPECT_CALL(normalize, normalize(rec_app)).WillOnce(Return(rec_nf));
    EXPECT_CALL(normalize, normalize(pol_app)).WillOnce(Return(pol_nf));
    std::optional<asex_agent> child = gen.generate(rec);
    ASSERT_TRUE(child.has_value());
    EXPECT_EQ(child->rec.term, rec_nf);
    EXPECT_EQ(child->pol.term, pol_nf);
}

TEST_F(AsexChildGeneratorTest, NulloptIfEitherNormalizeFails) {
    EXPECT_CALL(sample_uniform, sample_uniform(8)).WillOnce(Return(0));
    EXPECT_CALL(encode_uint, encode_uint(0)).WillOnce(Return(rand));
    EXPECT_CALL(make_combinator, make_combinator()).WillOnce(Return(y));
    EXPECT_CALL(make_true, make_true()).WillOnce(Return(flag_t));
    EXPECT_CALL(make_false, make_false()).WillOnce(Return(flag_f));
    EXPECT_CALL(apply_recursor, apply(rec, flag_t, y, rand)).WillOnce(Return(rec_app));
    EXPECT_CALL(apply_recursor, apply(rec, flag_f, y, rand)).WillOnce(Return(pol_app));
    EXPECT_CALL(normalize, normalize(rec_app)).WillOnce(Return(std::nullopt));
    std::optional<asex_agent> child = gen.generate(rec);
    EXPECT_FALSE(child.has_value());
}
