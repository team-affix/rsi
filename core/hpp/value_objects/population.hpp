#ifndef POPULATION_HPP
#define POPULATION_HPP

#include <cstddef>
#include <vector>

template<typename Agent>
struct population {
    std::vector<Agent> agents;
    std::size_t capacity;
    population(std::size_t capacity);
    auto operator<=>(const population&) const = default;
};

template<typename Agent>
population<Agent>::population(std::size_t capacity)
    : agents()
    , capacity(capacity) {
}

#endif
