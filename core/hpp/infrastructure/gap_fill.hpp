#ifndef GAP_FILL_HPP
#define GAP_FILL_HPP

#include "value_objects/population.hpp"

template<typename Agent, typename IProduceInitialAgent>
struct gap_fill {
    gap_fill(IProduceInitialAgent& produce_initial_agent);
    void fill(population<Agent>& pop);
private:
    IProduceInitialAgent& produce_initial_agent_;
};

template<typename Agent, typename IProduceInitialAgent>
gap_fill<Agent, IProduceInitialAgent>::gap_fill(IProduceInitialAgent& produce_initial_agent)
    : produce_initial_agent_(produce_initial_agent) {
}

template<typename Agent, typename IProduceInitialAgent>
void gap_fill<Agent, IProduceInitialAgent>::fill(population<Agent>& pop) {
    while(pop.agents.size() < pop.capacity)
        pop.agents.push_back(produce_initial_agent_.produce());
}

#endif
