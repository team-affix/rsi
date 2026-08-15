#ifndef ASEX_AGENT_HPP
#define ASEX_AGENT_HPP

#include "value_objects/policy.hpp"
#include "value_objects/recursor.hpp"

struct asex_agent {
    recursor rec;
    policy pol;
    auto operator<=>(const asex_agent&) const = default;
};

#endif
