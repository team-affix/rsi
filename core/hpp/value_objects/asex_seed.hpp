#ifndef ASEX_SEED_HPP
#define ASEX_SEED_HPP

#include <cstddef>
#include <cstdint>
#include "value_objects/asex_progenitor.hpp"

struct asex_seed {
    asex_progenitor progenitor;
    std::size_t index;
    uint64_t rand;
    auto operator<=>(const asex_seed&) const = default;
};

#endif
