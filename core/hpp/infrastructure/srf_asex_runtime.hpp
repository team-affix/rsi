#ifndef SRF_ASEX_RUNTIME_HPP
#define SRF_ASEX_RUNTIME_HPP

#include "value_objects/population.hpp"

template<typename Agent, typename IStep>
struct srf_asex_runtime {
    srf_asex_runtime(population<Agent>& pop, IStep& stepper);
    void step();
    const population<Agent>& get_population() const;
private:
    population<Agent>& pop_;
    IStep& stepper_;
};

template<typename Agent, typename IStep>
srf_asex_runtime<Agent, IStep>::srf_asex_runtime(population<Agent>& pop, IStep& stepper)
    : pop_(pop)
    , stepper_(stepper) {
}

template<typename Agent, typename IStep>
void srf_asex_runtime<Agent, IStep>::step() {
    pop_ = stepper_.step(pop_);
}

template<typename Agent, typename IStep>
const population<Agent>& srf_asex_runtime<Agent, IStep>::get_population() const {
    return pop_;
}

#endif
