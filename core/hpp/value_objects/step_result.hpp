#ifndef STEP_RESULT_HPP
#define STEP_RESULT_HPP

#include <cstddef>
#include "value_objects/policy.hpp"

struct step_result {
    double best_reward;
    policy best_model;
    std::size_t viable_seed_count;
    auto operator<=>(const step_result&) const = default;
};

#endif
