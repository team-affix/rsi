#ifndef ASEX_SELECTION_HPP
#define ASEX_SELECTION_HPP

#include <vector>
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/policy.hpp"

struct asex_selection {
    std::vector<asex_progenitor> progenitors;
    double best_reward;
    policy best_policy;
    auto operator<=>(const asex_selection&) const = default;
};

#endif
