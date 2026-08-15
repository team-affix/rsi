#ifndef ASEX_REPRODUCTION_CONTEXT_HPP
#define ASEX_REPRODUCTION_CONTEXT_HPP

#include "value_objects/asex_agent.hpp"

struct asex_reproduction_context {
    asex_agent agent;
    auto operator<=>(const asex_reproduction_context&) const = default;
};

#endif
