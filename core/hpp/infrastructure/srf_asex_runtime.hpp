#ifndef SRF_ASEX_RUNTIME_HPP
#define SRF_ASEX_RUNTIME_HPP

#include <cstddef>
#include <cstdint>
#include "infrastructure/srf_asex_manifest.hpp"
#include "value_objects/policy.hpp"
#include "value_objects/step_result.hpp"

template<typename IEvaluate>
struct srf_asex_runtime {
    srf_asex_runtime(std::size_t n, std::size_t r, uint64_t max_steps, uint64_t max_bytes,
                     uint64_t sample_nodes, uint64_t seed, IEvaluate& evaluate);
    void initialize();
    step_result step();
    double best_reward() const;
    const policy& best_policy() const;
private:
    srf_asex_manifest<IEvaluate> manifest_;
};

template<typename IEvaluate>
srf_asex_runtime<IEvaluate>::srf_asex_runtime(std::size_t n, std::size_t r, uint64_t max_steps,
                                             uint64_t max_bytes, uint64_t sample_nodes,
                                             uint64_t seed, IEvaluate& evaluate)
    : manifest_(n, r, max_steps, max_bytes, sample_nodes, seed, evaluate) {
}

template<typename IEvaluate>
void srf_asex_runtime<IEvaluate>::initialize() {
    manifest_.initializer_.initialize();
}

template<typename IEvaluate>
step_result srf_asex_runtime<IEvaluate>::step() {
    return manifest_.stepper_.step();
}

template<typename IEvaluate>
double srf_asex_runtime<IEvaluate>::best_reward() const {
    return manifest_.stepper_.best_reward();
}

template<typename IEvaluate>
const policy& srf_asex_runtime<IEvaluate>::best_policy() const {
    return manifest_.stepper_.best_policy();
}

#endif
