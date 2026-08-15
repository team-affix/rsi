#ifndef NEXT_GENERATION_BUFFER_HPP
#define NEXT_GENERATION_BUFFER_HPP

#include <cstddef>
#include <utility>
#include <vector>
#include "debug_assert.hpp"

template<typename Agent, typename IClear, typename IAddAgent>
struct next_generation_buffer {
    next_generation_buffer(IClear& clear, IAddAgent& add_agent, std::size_t n);
    void add(Agent agent);
    std::size_t size() const;
    bool at_capacity() const;
    void commit();
private:
    IClear& clear_;
    IAddAgent& add_agent_;
    std::size_t n_;
    std::vector<Agent> staged_;
};

template<typename Agent, typename IClear, typename IAddAgent>
next_generation_buffer<Agent, IClear, IAddAgent>::next_generation_buffer(IClear& clear,
                                                                        IAddAgent& add_agent,
                                                                        std::size_t n)
    : clear_(clear)
    , add_agent_(add_agent)
    , n_(n)
    , staged_() {
    staged_.reserve(n);
}

template<typename Agent, typename IClear, typename IAddAgent>
void next_generation_buffer<Agent, IClear, IAddAgent>::add(Agent agent) {
    DEBUG_ASSERT(staged_.size() < n_);
    staged_.push_back(agent);
}

template<typename Agent, typename IClear, typename IAddAgent>
std::size_t next_generation_buffer<Agent, IClear, IAddAgent>::size() const {
    return staged_.size();
}

template<typename Agent, typename IClear, typename IAddAgent>
bool next_generation_buffer<Agent, IClear, IAddAgent>::at_capacity() const {
    return staged_.size() >= n_;
}

template<typename Agent, typename IClear, typename IAddAgent>
void next_generation_buffer<Agent, IClear, IAddAgent>::commit() {
    DEBUG_ASSERT(staged_.size() == n_);
    clear_.clear();
    for(Agent& agent : staged_)
        add_agent_.add(std::move(agent));
    staged_.clear();
}

#endif
