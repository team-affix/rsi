#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/srf_asex_manifest.hpp"
#include "value_objects/policy.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockEvaluate {
    MOCK_METHOD(double, evaluate, (const policy&));
};

struct SrfAsexRuntimeIntegrationTest : public ::testing::Test {
    using manifest_t = srf_asex_manifest<MockEvaluate>;
    SrfAsexRuntimeIntegrationTest()
        : manifest(4, 2, 8, 100000, 2, 1, evaluate) {
    }
    NiceMock<MockEvaluate> evaluate;
    manifest_t manifest;
};

TEST_F(SrfAsexRuntimeIntegrationTest, StartsEmpty) {
    EXPECT_EQ(manifest.get_population().size(), 0u);
    EXPECT_EQ(manifest.get_population().capacity(), 4u);
}

TEST_F(SrfAsexRuntimeIntegrationTest, InitializeThenStepsKeepN) {
    ON_CALL(evaluate, evaluate).WillByDefault(Return(0.0));
    manifest.get_runtime().initialize();
    EXPECT_EQ(manifest.get_population().size(), 4u);
    manifest.get_runtime().step();
    EXPECT_EQ(manifest.get_population().size(), 4u);
    manifest.get_runtime().step();
    EXPECT_EQ(manifest.get_population().size(), 4u);
}
