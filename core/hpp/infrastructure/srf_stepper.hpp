#ifndef SRF_STEPPER_HPP
#define SRF_STEPPER_HPP

#include "value_objects/population.hpp"

template<typename Agent, typename ReproductionContext, typename ISelect, typename IReproduce,
         typename IProduceInitialAgent>
struct srf_stepper {
    srf_stepper(ISelect& select, IReproduce& reproduce,
                IProduceInitialAgent& produce_initial_agent);
    population<Agent> step(const population<Agent>& in);
private:
    ISelect& select_;
    IReproduce& reproduce_;
    IProduceInitialAgent& produce_initial_agent_;
};

template<typename Agent, typename ReproductionContext, typename ISelect, typename IReproduce,
         typename IProduceInitialAgent>
srf_stepper<Agent, ReproductionContext, ISelect, IReproduce, IProduceInitialAgent>::srf_stepper(
    ISelect& select, IReproduce& reproduce, IProduceInitialAgent& produce_initial_agent)
    : select_(select)
    , reproduce_(reproduce)
    , produce_initial_agent_(produce_initial_agent) {
}

template<typename Agent, typename ReproductionContext, typename ISelect, typename IReproduce,
         typename IProduceInitialAgent>
population<Agent>
srf_stepper<Agent, ReproductionContext, ISelect, IReproduce, IProduceInitialAgent>::step(
    const population<Agent>& in) {
    auto contexts = select_.select(in);
    population<Agent> out{in.capacity};
    for(const ReproductionContext& ctx : contexts) {
        auto children = reproduce_.reproduce(ctx);
        for(const Agent& child : children)
            out.agents.push_back(child);
    }
    while(out.agents.size() < out.capacity)
        out.agents.push_back(produce_initial_agent_.produce());
    return out;
}

#endif
