#include <memory>
#include <optional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/bounded_normalizer.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

struct MockRuntime {
    MOCK_METHOD(void, step, (), ());
    MOCK_METHOD(bool, done, (), (const));
    MOCK_METHOD(std::shared_ptr<expr>, output, (), (const));
    MOCK_METHOD(std::size_t, space_usage, (), (const));
};

struct MockMakeRuntime {
    MOCK_METHOD(MockRuntime&, make, (std::shared_ptr<expr>));
};

using test_normalizer_t = bounded_normalizer<MockMakeRuntime>;

struct BoundedNormalizerTest : public ::testing::Test {
    BoundedNormalizerTest();
    NiceMock<MockMakeRuntime> make_runtime;
    NiceMock<MockRuntime> runtime;
    test_normalizer_t normalizer{make_runtime, 4, 100};
    std::shared_ptr<expr> term = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> out = std::make_shared<expr>(expr{expr::var{1}});
};

BoundedNormalizerTest::BoundedNormalizerTest() {
    ON_CALL(make_runtime, make(term)).WillByDefault(ReturnRef(runtime));
    ON_CALL(runtime, space_usage()).WillByDefault(Return(1));
    ON_CALL(runtime, done()).WillByDefault(Return(false));
    ON_CALL(runtime, output()).WillByDefault(Return(out));
}

TEST_F(BoundedNormalizerTest, ReturnsOutputWhenAlreadyDone) {
    ON_CALL(runtime, done()).WillByDefault(Return(true));
    std::optional<std::shared_ptr<expr>> got = normalizer.normalize(term);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(*got, out);
}

TEST_F(BoundedNormalizerTest, NulloptWhenMaxStepsExceeded) {
    test_normalizer_t tight{make_runtime, 0, 100};
    EXPECT_EQ(tight.normalize(term), std::nullopt);
}

TEST_F(BoundedNormalizerTest, NulloptWhenSpaceUsageEqualsMaxBytes) {
    ON_CALL(runtime, space_usage()).WillByDefault(Return(100));
    EXPECT_CALL(runtime, step()).Times(0);
    EXPECT_EQ(normalizer.normalize(term), std::nullopt);
}

TEST_F(BoundedNormalizerTest, NulloptWhenMaxBytesExceeded) {
    ON_CALL(runtime, space_usage()).WillByDefault(Return(101));
    EXPECT_EQ(normalizer.normalize(term), std::nullopt);
}

TEST_F(BoundedNormalizerTest, StepsThenReturnsOutput) {
    EXPECT_CALL(runtime, done()).WillOnce(Return(false)).WillRepeatedly(Return(true));
    std::optional<std::shared_ptr<expr>> got = normalizer.normalize(term);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(*got, out);
}

TEST_F(BoundedNormalizerTest, NulloptWhenMaxBytesExceededAfterStep) {
    EXPECT_CALL(runtime, step()).WillOnce([&] {
        ON_CALL(runtime, space_usage()).WillByDefault(Return(101));
    });
    EXPECT_EQ(normalizer.normalize(term), std::nullopt);
}
