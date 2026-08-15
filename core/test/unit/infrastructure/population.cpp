#include <memory>
#include <gtest/gtest.h>
#include "infrastructure/population.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"

struct PopulationTest : public ::testing::Test {
    population<asex_agent> pop{2};
    std::shared_ptr<expr> t0 = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    asex_agent a0{recursor{t0}, policy{t0}};
    asex_agent a1{recursor{t1}, policy{t1}};
};

TEST_F(PopulationTest, CtorSizeZeroCapacityN) {
    EXPECT_EQ(pop.size(), 0u);
    EXPECT_EQ(pop.capacity(), 2u);
}

TEST_F(PopulationTest, AddThenGetById) {
    pop.add(a0);
    pop.add(a1);
    EXPECT_EQ(pop.size(), 2u);
    EXPECT_EQ(pop.get(0).rec.term, t0);
    EXPECT_EQ(pop.get(1).rec.term, t1);
}

TEST_F(PopulationTest, ClearKeepsCapacity) {
    pop.add(a0);
    pop.clear();
    EXPECT_EQ(pop.size(), 0u);
    EXPECT_EQ(pop.capacity(), 2u);
    pop.add(a1);
    EXPECT_EQ(pop.get(0).rec.term, t1);
}
