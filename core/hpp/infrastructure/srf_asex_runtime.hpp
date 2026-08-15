#ifndef SRF_ASEX_RUNTIME_HPP
#define SRF_ASEX_RUNTIME_HPP

template<typename IInitialize, typename IStep>
struct srf_asex_runtime {
    srf_asex_runtime(IInitialize& initialize, IStep& step);
    void initialize();
    void step();
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
void srf_asex_runtime<IInitialize, IStep>::step() {
    step_.step();
}

#endif
