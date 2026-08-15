#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/population_initializer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockGetSize {
    MOCK_METHOD(std::size_t, size, (), (const));
};

struct MockGetCapacity {
    MOCK_METHOD(std::size_t, capacity, (), (const));
};

struct MockAddAgent {
    MOCK_METHOD(void, add, (asex_agent));
};

struct MockProduceInitialAgent {
    MOCK_METHOD(asex_agent, produce, ());
};

using test_initializer_t =
    population_initializer<MockGetSize, MockGetCapacity, MockAddAgent, MockProduceInitialAgent>;

struct PopulationInitializerTest : public ::testing::Test {
    NiceMock<MockGetSize> get_size;
    NiceMock<MockGetCapacity> get_capacity;
    NiceMock<MockAddAgent> add_agent;
    NiceMock<MockProduceInitialAgent> produce;
    test_initializer_t initializer{get_size, get_capacity, add_agent, produce};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_agent agent{recursor{t}, policy{t}};
};

TEST_F(PopulationInitializerTest, EmptyProducesAndAddsToN) {
    EXPECT_CALL(get_capacity, capacity()).WillRepeatedly(Return(2));
    EXPECT_CALL(get_size, size()).WillOnce(Return(0)).WillOnce(Return(1)).WillOnce(Return(2));
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent)).WillOnce(Return(agent));
    EXPECT_CALL(add_agent, add(agent)).Times(2);
    initializer.initialize();
}

TEST_F(PopulationInitializerTest, AlreadyFullDoesNotProduce) {
    EXPECT_CALL(get_capacity, capacity()).WillOnce(Return(2));
    EXPECT_CALL(get_size, size()).WillOnce(Return(2));
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(add_agent, add).Times(0);
    initializer.initialize();
}
