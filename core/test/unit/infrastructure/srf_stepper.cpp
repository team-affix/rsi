#include <memory>
#include <optional>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/srf_stepper.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/asex_seed.hpp"
#include "value_objects/asex_selection.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/step_result.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockSelect {
    MOCK_METHOD(asex_selection, select, ());
};

struct MockReproduce {
    MOCK_METHOD((std::vector<asex_seed>), reproduce, (const asex_progenitor&));
};

struct MockGerminate {
    MOCK_METHOD((std::optional<asex_agent>), germinate, (const asex_seed&));
};

struct MockProduceInitialAgent {
    MOCK_METHOD(asex_agent, produce, ());
};

struct MockAddToBuffer {
    MOCK_METHOD(void, add, (asex_agent));
};

struct MockAtCapacity {
    MOCK_METHOD(bool, at_capacity, (), (const));
};

struct MockCommitBuffer {
    MOCK_METHOD(void, commit, ());
};

using test_stepper_t =
    srf_stepper<MockSelect, MockReproduce, MockGerminate, MockProduceInitialAgent, MockAddToBuffer,
                MockAtCapacity, MockCommitBuffer>;

struct SrfStepperTest : public ::testing::Test {
    NiceMock<MockSelect> select;
    NiceMock<MockReproduce> reproduce;
    NiceMock<MockGerminate> germinate;
    NiceMock<MockProduceInitialAgent> produce;
    NiceMock<MockAddToBuffer> add_to_buffer;
    NiceMock<MockAtCapacity> at_capacity;
    NiceMock<MockCommitBuffer> commit_buffer;
    test_stepper_t stepper{select, reproduce, germinate, produce, add_to_buffer, at_capacity,
                           commit_buffer};
    std::shared_ptr<expr> t = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> t1 = std::make_shared<expr>(expr{expr::var{1}});
    asex_agent agent{recursor{t}, policy{t}};
    asex_agent agent1{recursor{t1}, policy{t1}};
    asex_progenitor progenitor{agent};
    asex_progenitor progenitor1{agent1};
    asex_seed seed0{progenitor, 0, 1};
    asex_seed seed1{progenitor, 1, 2};
    asex_seed seed_p1{progenitor1, 0, 3};
    asex_selection selection{{progenitor}, 4.0, agent.pol};
};

TEST_F(SrfStepperTest, EmptySelectHoleFillsThenCommits) {
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(asex_selection{{}, 4.0, agent.pol}));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent)).WillOnce(Return(agent));
    EXPECT_CALL(add_to_buffer, add(agent)).Times(2);
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 0u);
}

TEST_F(SrfStepperTest, GerminatesBothSeedsEvenIfFirstFails) {
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    asex_agent child{recursor{tc}, policy{tc}};
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(selection));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0, seed1}));
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(germinate, germinate(seed1)).WillOnce(Return(child));
    EXPECT_CALL(add_to_buffer, add(child));
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 1u);
}

TEST_F(SrfStepperTest, UnviableSeedsHoleFill) {
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(selection));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0, seed1}));
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(germinate, germinate(seed1)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(produce, produce()).WillOnce(Return(agent));
    EXPECT_CALL(add_to_buffer, add(agent));
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 0u);
}

TEST_F(SrfStepperTest, StopsMidSeedWhenAtCapacity) {
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    asex_agent child{recursor{tc}, policy{tc}};
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(selection));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0, seed1}));
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(child));
    EXPECT_CALL(germinate, germinate(seed1)).Times(0);
    EXPECT_CALL(add_to_buffer, add(child));
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 1u);
}

TEST_F(SrfStepperTest, StopsBetweenProgenitorsWhenAtCapacity) {
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    asex_agent child{recursor{tc}, policy{tc}};
    asex_selection two{{progenitor, progenitor1}, 4.0, agent.pol};
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(two));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0}));
    EXPECT_CALL(reproduce, reproduce(progenitor1)).Times(0);
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(child));
    EXPECT_CALL(add_to_buffer, add(child));
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 1u);
}

TEST_F(SrfStepperTest, AllViableFillsWithoutHoleFill) {
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> td = std::make_shared<expr>(expr{expr::var{3}});
    asex_agent child0{recursor{tc}, policy{tc}};
    asex_agent child1{recursor{td}, policy{td}};
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(selection));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0, seed1}));
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(child0));
    EXPECT_CALL(germinate, germinate(seed1)).WillOnce(Return(child1));
    EXPECT_CALL(add_to_buffer, add(child0));
    EXPECT_CALL(add_to_buffer, add(child1));
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 2u);
}

TEST_F(SrfStepperTest, TwoProgenitorsSumViableCounts) {
    std::shared_ptr<expr> tc = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> td = std::make_shared<expr>(expr{expr::var{3}});
    asex_agent child0{recursor{tc}, policy{tc}};
    asex_agent child1{recursor{td}, policy{td}};
    asex_selection two{{progenitor, progenitor1}, 4.0, agent.pol};
    ON_CALL(at_capacity, at_capacity()).WillByDefault(Return(true));
    EXPECT_CALL(select, select()).WillOnce(Return(two));
    EXPECT_CALL(at_capacity, at_capacity())
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(reproduce, reproduce(progenitor))
        .WillOnce(Return(std::vector<asex_seed>{seed0}));
    EXPECT_CALL(reproduce, reproduce(progenitor1))
        .WillOnce(Return(std::vector<asex_seed>{seed_p1}));
    EXPECT_CALL(germinate, germinate(seed0)).WillOnce(Return(child0));
    EXPECT_CALL(germinate, germinate(seed_p1)).WillOnce(Return(child1));
    EXPECT_CALL(add_to_buffer, add(child0));
    EXPECT_CALL(add_to_buffer, add(child1));
    EXPECT_CALL(produce, produce()).Times(0);
    EXPECT_CALL(commit_buffer, commit());
    step_result result = stepper.step();
    EXPECT_EQ(result.best_reward, 4.0);
    EXPECT_EQ(result.best_model.term, t);
    EXPECT_EQ(result.viable_seed_count, 2u);
}
