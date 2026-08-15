#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/srf_stepper.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/population.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

struct MockSelect {
    MOCK_METHOD((std::vector<asex_reproduction_context>), select,
                (const population<asex_agent>&));
};

struct MockReproduce {
    MOCK_METHOD((std::vector<asex_agent>), reproduce, (const asex_reproduction_context&));
};

struct MockProduceInitialAgent {
    MOCK_METHOD(asex_agent, produce, ());
};

using test_stepper_t = srf_stepper<asex_agent, asex_reproduction_context, MockSelect, MockReproduce,
                                   MockProduceInitialAgent>;

struct SrfStepperTest : public ::testing::Test {
    NiceMock<MockSelect> select;
    NiceMock<MockReproduce> reproduce;
    NiceMock<MockProduceInitialAgent> produce;
    test_stepper_t stepper{select, reproduce, produce};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    asex_agent agent{recursor{t}, policy{t}};
    asex_reproduction_context ctx{agent};
};

TEST_F(SrfStepperTest, EmptyInProducesToCapacity) {
    population<asex_agent> in{2};
    EXPECT_CALL(select, select).WillOnce(Return(std::vector<asex_reproduction_context>{}));
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent)).WillOnce(Return(agent));
    population<asex_agent> out = stepper.step(in);
    EXPECT_EQ(out.agents.size(), 2u);
    EXPECT_EQ(out.capacity, 2u);
}

TEST_F(SrfStepperTest, ParentsNeverInOutput) {
    std::shared_ptr<expr> tp = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> tf = std::make_shared<expr>(expr{expr::var{3}});
    asex_agent parent{recursor{tp}, policy{tp}};
    asex_agent child{recursor{tc}, policy{tc}};
    asex_agent filled{recursor{tf}, policy{tf}};
    population<asex_agent> in{2};
    in.agents.push_back(parent);
    in.agents.push_back(parent);
    asex_reproduction_context parent_ctx{parent};
    EXPECT_CALL(select, select)
        .WillOnce(Return(std::vector<asex_reproduction_context>{parent_ctx}));
    EXPECT_CALL(reproduce, reproduce)
        .WillOnce(Return(std::vector<asex_agent>{child}));
    EXPECT_CALL(produce, produce()).WillOnce(Return(filled));
    population<asex_agent> out = stepper.step(in);
    EXPECT_EQ(out.agents.size(), 2u);
    EXPECT_EQ(out.agents[0].rec.term, child.rec.term);
    EXPECT_EQ(out.agents[1].rec.term, filled.rec.term);
}

TEST_F(SrfStepperTest, StrictSelectReproduceProduceSequence) {
    StrictMock<MockSelect> strict_select;
    StrictMock<MockReproduce> strict_reproduce;
    StrictMock<MockProduceInitialAgent> strict_produce;
    test_stepper_t strict_stepper{strict_select, strict_reproduce, strict_produce};
    population<asex_agent> in{1};
    in.agents.push_back(agent);
    EXPECT_CALL(strict_select, select)
        .WillOnce(Return(std::vector<asex_reproduction_context>{ctx}));
    EXPECT_CALL(strict_reproduce, reproduce).WillOnce(Return(std::vector<asex_agent>{agent}));
    population<asex_agent> out = strict_stepper.step(in);
    EXPECT_EQ(out.agents.size(), 1u);
}
