#ifndef ASEX_SELECTOR_HPP
#define ASEX_SELECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_progenitor.hpp"
#include "debug_assert.hpp"

template<typename IGetSize, typename IGetAgent, typename IEvaluate>
struct asex_selector {
    asex_selector(IGetSize& get_size, IGetAgent& get_agent, IEvaluate& evaluate, std::size_t g);
    std::vector<asex_progenitor> select();
private:
    IGetSize& get_size_;
    IGetAgent& get_agent_;
    IEvaluate& evaluate_;
    std::size_t g_;
};

template<typename IGetSize, typename IGetAgent, typename IEvaluate>
asex_selector<IGetSize, IGetAgent, IEvaluate>::asex_selector(IGetSize& get_size,
                                                            IGetAgent& get_agent,
                                                            IEvaluate& evaluate, std::size_t g)
    : get_size_(get_size)
    , get_agent_(get_agent)
    , evaluate_(evaluate)
    , g_(g) {
}

template<typename IGetSize, typename IGetAgent, typename IEvaluate>
std::vector<asex_progenitor> asex_selector<IGetSize, IGetAgent, IEvaluate>::select() {
    std::size_t n = get_size_.size();
    DEBUG_ASSERT(n >= g_);
    std::vector<std::pair<double, std::size_t>> scored;
    for(std::size_t i = 0; i < n; ++i) {
        const asex_agent& agent = get_agent_.get(i);
        scored.push_back({-evaluate_.evaluate(agent.pol), i});
    }
    std::sort(scored.begin(), scored.end());
    std::vector<asex_progenitor> result;
    for(std::size_t i = 0; i < g_; ++i)
        result.push_back(asex_progenitor{get_agent_.get(scored[i].second)});
    return result;
}

#endif
