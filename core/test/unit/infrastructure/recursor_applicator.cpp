#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "infrastructure/recursor_applicator.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

using ::testing::NiceMock;
using ::testing::Return;

struct MockMakeApp {
    MOCK_METHOD(std::shared_ptr<expr>, make_app, (std::shared_ptr<expr>, std::shared_ptr<expr>));
};

using test_applicator_t = recursor_applicator<MockMakeApp>;

struct RecursorApplicatorTest : public ::testing::Test {
    NiceMock<MockMakeApp> make_app;
    test_applicator_t applicator{make_app};
    std::shared_ptr<expr> rec_term = std::make_shared<expr>(expr{expr::var{0}});
    std::shared_ptr<expr> flag = std::make_shared<expr>(expr{expr::var{1}});
    std::shared_ptr<expr> y = std::make_shared<expr>(expr{expr::var{2}});
    std::shared_ptr<expr> rand = std::make_shared<expr>(expr{expr::var{3}});
    recursor rec{rec_term};
    std::shared_ptr<expr> with_flag = std::make_shared<expr>(expr{expr::app{rec_term, flag}});
    std::shared_ptr<expr> with_y = std::make_shared<expr>(expr{expr::app{with_flag, y}});
    std::shared_ptr<expr> result = std::make_shared<expr>(expr{expr::app{with_y, rand}});
};

TEST_F(RecursorApplicatorTest, ApplyIsLeftAssociated) {
    EXPECT_CALL(make_app, make_app(rec_term, flag)).WillOnce(Return(with_flag));
    EXPECT_CALL(make_app, make_app(with_flag, y)).WillOnce(Return(with_y));
    EXPECT_CALL(make_app, make_app(with_y, rand)).WillOnce(Return(result));
    EXPECT_EQ(applicator.apply(rec, flag, y, rand), result);
}
