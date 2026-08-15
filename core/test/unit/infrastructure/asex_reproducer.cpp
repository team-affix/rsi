#include <memory>
#include <optional>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/asex_reproducer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockGenerateChild {
    MOCK_METHOD((std::optional<asex_agent>), generate, (const recursor&));
};

using test_reproducer_t = asex_reproducer<MockGenerateChild>;

struct AsexReproducerTest : public ::testing::Test {
    NiceMock<MockGenerateChild> generate_child;
    test_reproducer_t reproducer{generate_child, 2, 3};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_agent agent{recursor{t}, policy{t}};
    asex_reproduction_context ctx{agent};
};

TEST_F(AsexReproducerTest, ProducesUpToRChildren) {
    EXPECT_CALL(generate_child, generate)
        .WillOnce(Return(agent))
        .WillOnce(Return(agent));
    std::vector<asex_agent> kids = reproducer.reproduce(ctx);
    EXPECT_EQ(kids.size(), 2u);
}

TEST_F(AsexReproducerTest, SharedRetryBudgetKeepsBornKids) {
    EXPECT_CALL(generate_child, generate)
        .WillOnce(Return(agent))
        .WillOnce(Return(std::nullopt))
        .WillOnce(Return(std::nullopt))
        .WillOnce(Return(std::nullopt));
    std::vector<asex_agent> kids = reproducer.reproduce(ctx);
    EXPECT_EQ(kids.size(), 1u);
}
