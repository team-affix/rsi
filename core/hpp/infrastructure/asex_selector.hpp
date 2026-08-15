#ifndef ASEX_SELECTOR_HPP
#define ASEX_SELECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <vector>
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/population.hpp"

template<typename IEvaluate>
struct asex_selector {
    asex_selector(IEvaluate& evaluate, std::size_t g);
    std::vector<asex_reproduction_context> select(const population<asex_agent>& pop);
private:
    struct scored_agent {
        std::size_t index;
        double score;
    };
    struct score_greater {
        bool operator()(const scored_agent& a, const scored_agent& b) const;
    };
    IEvaluate& evaluate_;
    std::size_t g_;
};

template<typename IEvaluate>
bool asex_selector<IEvaluate>::score_greater::operator()(const scored_agent& a,
                                                        const scored_agent& b) const {
    if(a.score != b.score)
        return a.score > b.score;
    return a.index < b.index;
}

template<typename IEvaluate>
asex_selector<IEvaluate>::asex_selector(IEvaluate& evaluate, std::size_t g)
    : evaluate_(evaluate)
    , g_(g) {
}

template<typename IEvaluate>
std::vector<asex_reproduction_context>
asex_selector<IEvaluate>::select(const population<asex_agent>& pop) {
    std::vector<scored_agent> scored;
    for(std::size_t i = 0; i < pop.agents.size(); ++i) {
        double score = evaluate_.evaluate(pop.agents[i].pol);
        scored.push_back(scored_agent{i, score});
    }
    std::sort(scored.begin(), scored.end(), score_greater{});
    std::size_t take = scored.size() < g_ ? scored.size() : g_;
    std::vector<asex_reproduction_context> contexts;
    for(std::size_t i = 0; i < take; ++i)
        contexts.push_back(asex_reproduction_context{pop.agents[scored[i].index]});
    return contexts;
}

#endif
