#include <cstddef>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/population_initializer.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

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
    PopulationInitializerTest();
    NiceMock<MockGetSize> get_size;
    NiceMock<MockGetCapacity> get_capacity;
    NiceMock<MockAddAgent> add_agent;
    NiceMock<MockProduceInitialAgent> produce;
    test_initializer_t initializer{get_size, get_capacity, add_agent, produce};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_agent agent{recursor{t}, policy{t}};
    std::size_t n_;
};

PopulationInitializerTest::PopulationInitializerTest()
    : n_(0) {
    ON_CALL(get_size, size()).WillByDefault([this] { return n_; });
    ON_CALL(get_capacity, capacity()).WillByDefault(Return(2));
    ON_CALL(add_agent, add(_)).WillByDefault([this](asex_agent) { ++n_; });
    ON_CALL(produce, produce()).WillByDefault(Return(agent));
}

TEST_F(PopulationInitializerTest, EmptyProducesAndAddsToN) {
    initializer.initialize();
    EXPECT_EQ(get_size.size(), 2u);
}

TEST_F(PopulationInitializerTest, AlreadyFullDoesNotProduce) {
    n_ = 2;
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(add_agent, add(_)).Times(0);
    initializer.initialize();
}
