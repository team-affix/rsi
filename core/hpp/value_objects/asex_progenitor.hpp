#ifndef ASEX_PROGENITOR_HPP
#define ASEX_PROGENITOR_HPP

#include "value_objects/asex_agent.hpp"

struct asex_progenitor {
    asex_agent parent;
    auto operator<=>(const asex_progenitor&) const = default;
};

#endif
