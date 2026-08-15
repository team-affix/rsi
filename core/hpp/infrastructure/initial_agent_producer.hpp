#ifndef INITIAL_AGENT_PRODUCER_HPP
#define INITIAL_AGENT_PRODUCER_HPP

#include "value_objects/asex_agent.hpp"

template<typename ISampleNf>
struct initial_agent_producer {
    initial_agent_producer(ISampleNf& sample_nf);
    asex_agent produce();
private:
    ISampleNf& sample_nf_;
};

template<typename ISampleNf>
initial_agent_producer<ISampleNf>::initial_agent_producer(ISampleNf& sample_nf)
    : sample_nf_(sample_nf) {
}

template<typename ISampleNf>
asex_agent initial_agent_producer<ISampleNf>::produce() {
    recursor rec{sample_nf_.sample()};
    policy pol{sample_nf_.sample()};
    return asex_agent{rec, pol};
}

#endif
