#ifndef POPULATION_HPP
#define POPULATION_HPP

#include <cstddef>
#include <vector>
#include "debug_assert.hpp"

template<typename Agent>
struct population {
    population(std::size_t n);
    std::size_t capacity() const;
    std::size_t size() const;
    const Agent& get(std::size_t id) const;
    void add(Agent agent);
    void clear();
private:
    std::size_t n_;
    std::vector<Agent> agents_;
};

template<typename Agent>
population<Agent>::population(std::size_t n)
    : n_(n)
    , agents_() {
    agents_.reserve(n);
}

template<typename Agent>
std::size_t population<Agent>::capacity() const {
    return n_;
}

template<typename Agent>
std::size_t population<Agent>::size() const {
    return agents_.size();
}

template<typename Agent>
const Agent& population<Agent>::get(std::size_t id) const {
    DEBUG_ASSERT(id < agents_.size());
    return agents_[id];
}

template<typename Agent>
void population<Agent>::add(Agent agent) {
    DEBUG_ASSERT(agents_.size() < n_);
    agents_.push_back(agent);
}

template<typename Agent>
void population<Agent>::clear() {
    agents_.clear();
}

#endif
