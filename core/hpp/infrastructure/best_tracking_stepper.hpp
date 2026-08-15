#ifndef BEST_TRACKING_STEPPER_HPP
#define BEST_TRACKING_STEPPER_HPP

#include <limits>
#include "value_objects/policy.hpp"
#include "value_objects/step_result.hpp"

template<typename IStep>
struct best_tracking_stepper {
    best_tracking_stepper(IStep& step);
    step_result step();
    double best_reward() const;
    const policy& best_policy() const;
private:
    IStep& step_;
    double best_reward_;
    policy best_policy_;
};

template<typename IStep>
best_tracking_stepper<IStep>::best_tracking_stepper(IStep& step)
    : step_(step)
    , best_reward_(-std::numeric_limits<double>::infinity())
    , best_policy_{nullptr} {
}

template<typename IStep>
step_result best_tracking_stepper<IStep>::step() {
    step_result result = step_.step();
    if(result.best_reward > best_reward_) {
        best_reward_ = result.best_reward;
        best_policy_ = result.best_policy;
    }
    return result;
}

template<typename IStep>
double best_tracking_stepper<IStep>::best_reward() const {
    return best_reward_;
}

template<typename IStep>
const policy& best_tracking_stepper<IStep>::best_policy() const {
    return best_policy_;
}

#endif
