#ifndef SRF_ASEX_MANIFEST_HPP
#define SRF_ASEX_MANIFEST_HPP

#include <cstddef>
#include <cstdint>
#include "infrastructure/asex_child_generator.hpp"
#include "infrastructure/asex_reproducer.hpp"
#include "infrastructure/asex_selector.hpp"
#include "infrastructure/bounded_normalizer.hpp"
#include "infrastructure/church.hpp"
#include "infrastructure/expr_factory.hpp"
#include "infrastructure/initial_agent_producer.hpp"
#include "infrastructure/lc_runtime_maker.hpp"
#include "infrastructure/le_bitstring_encoder.hpp"
#include "infrastructure/nf_sampler.hpp"
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/recursor_applicator.hpp"
#include "infrastructure/srf_asex_runtime.hpp"
#include "infrastructure/srf_stepper.hpp"
#include "infrastructure/uniform_rng.hpp"
#include "infrastructure/y_combinator.hpp"
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/population.hpp"
#include "debug_assert.hpp"

template<typename IEvaluate>
struct srf_asex_manifest {
    using expr_nodes_t = rc_pool<expr>;
    using expr_factory_t = expr_factory<expr_nodes_t>;
    using church_t = church<expr_factory_t, expr_factory_t>;
    using y_combinator_t = y_combinator<expr_factory_t, expr_factory_t, expr_factory_t>;
    using encoder_t =
        le_bitstring_encoder<church_t, church_t, expr_factory_t, expr_factory_t, expr_factory_t>;
    using applicator_t = recursor_applicator<expr_factory_t>;
    using rng_t = uniform_rng;
    using sampler_t = nf_sampler<rng_t, expr_factory_t, expr_factory_t, expr_factory_t>;
    using runtime_maker_t = lc_runtime_maker;
    using normalizer_t = bounded_normalizer<runtime_maker_t>;
    using producer_t = initial_agent_producer<sampler_t>;
    using child_gen_t =
        asex_child_generator<rng_t, encoder_t, church_t, church_t, y_combinator_t, applicator_t,
                             normalizer_t>;
    using reproducer_t = asex_reproducer<child_gen_t>;
    using selector_t = asex_selector<IEvaluate>;
    using stepper_t =
        srf_stepper<asex_agent, asex_reproduction_context, selector_t, reproducer_t, producer_t>;
    using runtime_t = srf_asex_runtime<asex_agent, stepper_t>;

    srf_asex_manifest(std::size_t n, std::size_t r, std::size_t max_retries, uint64_t max_steps,
                      uint64_t max_bytes, uint64_t sample_nodes, uint64_t bit_width, uint64_t seed,
                      IEvaluate& evaluate);
    runtime_t& get_runtime();
    const population<asex_agent>& get_population() const;
private:
    expr_nodes_t expr_nodes_;
    expr_nodes_t out_nodes_;
    expr_factory_t factory_;
    church_t church_;
    y_combinator_t y_;
    encoder_t encoder_;
    applicator_t applicator_;
    rng_t rng_;
    sampler_t sampler_;
    runtime_maker_t runtime_maker_;
    normalizer_t normalizer_;
    producer_t producer_;
    child_gen_t child_gen_;
    reproducer_t reproducer_;
    selector_t selector_;
    stepper_t stepper_;
    population<asex_agent> population_;
    runtime_t runtime_;
};

template<typename IEvaluate>
srf_asex_manifest<IEvaluate>::srf_asex_manifest(std::size_t n, std::size_t r,
                                               std::size_t max_retries, uint64_t max_steps,
                                               uint64_t max_bytes, uint64_t sample_nodes,
                                               uint64_t bit_width, uint64_t seed,
                                               IEvaluate& evaluate)
    : expr_nodes_()
    , out_nodes_()
    , factory_(expr_nodes_)
    , church_(factory_, factory_)
    , y_(factory_, factory_, factory_)
    , encoder_(church_, church_, factory_, factory_, factory_)
    , applicator_(factory_)
    , rng_(seed)
    , sampler_(rng_, factory_, factory_, factory_, sample_nodes)
    , runtime_maker_(out_nodes_)
    , normalizer_(runtime_maker_, max_steps, max_bytes)
    , producer_(sampler_)
    , child_gen_(rng_, encoder_, church_, church_, y_, applicator_, normalizer_, bit_width)
    , reproducer_(child_gen_, r, max_retries)
    , selector_(evaluate, n / r)
    , stepper_(selector_, reproducer_, producer_)
    , population_(n)
    , runtime_(population_, stepper_) {
    DEBUG_ASSERT(r > 0);
}

template<typename IEvaluate>
typename srf_asex_manifest<IEvaluate>::runtime_t& srf_asex_manifest<IEvaluate>::get_runtime() {
    return runtime_;
}

template<typename IEvaluate>
const population<asex_agent>& srf_asex_manifest<IEvaluate>::get_population() const {
    return population_;
}

#endif
