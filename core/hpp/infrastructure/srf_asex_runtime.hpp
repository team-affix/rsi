#ifndef SRF_ASEX_RUNTIME_HPP
#define SRF_ASEX_RUNTIME_HPP

#include "value_objects/step_result.hpp"

template<typename IInitialize, typename IStep>
struct srf_asex_runtime {
    srf_asex_runtime(IInitialize& initialize, IStep& step);
    void initialize();
    step_result step();
private:
    IInitialize& initialize_;
    IStep& step_;
};

template<typename IInitialize, typename IStep>
srf_asex_runtime<IInitialize, IStep>::srf_asex_runtime(IInitialize& initialize, IStep& step)
    : initialize_(initialize)
    , step_(step) {
}

template<typename IInitialize, typename IStep>
void srf_asex_runtime<IInitialize, IStep>::initialize() {
    initialize_.initialize();
}

template<typename IInitialize, typename IStep>
step_result srf_asex_runtime<IInitialize, IStep>::step() {
    return step_.step();
}

#endif
