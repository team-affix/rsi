#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/next_generation_buffer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"

using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;

struct MockClear {
    MOCK_METHOD(void, clear, ());
};

struct MockAddAgent {
    MOCK_METHOD(void, add, (asex_agent));
};

using test_buffer_t = next_generation_buffer<asex_agent, MockClear, MockAddAgent>;

struct NextGenerationBufferTest : public ::testing::Test {
    NiceMock<MockClear> clear;
    NiceMock<MockAddAgent> add_agent;
    test_buffer_t buffer{clear, add_agent, 2};
    std::shared_ptr<expr> t0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    asex_agent a0{recursor{t0}, policy{t0}};
    asex_agent a1{recursor{t1}, policy{t1}};
};

TEST_F(NextGenerationBufferTest, AddIncreasesSize) {
    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_FALSE(buffer.at_capacity());
    buffer.add(a0);
    EXPECT_EQ(buffer.size(), 1u);
    EXPECT_FALSE(buffer.at_capacity());
    buffer.add(a1);
    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_TRUE(buffer.at_capacity());
}

TEST_F(NextGenerationBufferTest, CommitWhenNotAtNThrows) {
    buffer.add(a0);
    EXPECT_THROW(buffer.commit(), std::logic_error);
}

TEST_F(NextGenerationBufferTest, CommitClearsThenAddsThenEmpties) {
    StrictMock<MockClear> strict_clear;
    StrictMock<MockAddAgent> strict_add;
    test_buffer_t strict_buffer{strict_clear, strict_add, 2};
    strict_buffer.add(a0);
    strict_buffer.add(a1);
    {
        InSequence seq;
        EXPECT_CALL(strict_clear, clear());
        EXPECT_CALL(strict_add, add(a0));
        EXPECT_CALL(strict_add, add(a1));
    }
    strict_buffer.commit();
    EXPECT_EQ(strict_buffer.size(), 0u);
}
