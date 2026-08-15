#ifndef NF_SAMPLER_HPP
#define NF_SAMPLER_HPP

#include <cstdint>
#include <memory>
#include "value_objects/expr.hpp"
#include "debug_assert.hpp"

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
struct nf_sampler {
    nf_sampler(ISampleUniform& sample_uniform, IMakeAbs& make_abs, IMakeApp& make_app,
               IMakeVar& make_var, uint64_t sample_nodes);
    std::shared_ptr<expr> sample();
private:
    std::shared_ptr<expr> sample_nf(uint32_t binders, uint64_t budget);
    std::shared_ptr<expr> sample_ne(uint32_t binders, uint64_t budget);
    std::shared_ptr<expr> sample_app(uint32_t binders, uint64_t budget);
    std::shared_ptr<expr> sample_var(uint32_t binders);
    uint64_t min_nf(uint32_t binders);
    ISampleUniform& sample_uniform_;
    IMakeAbs& make_abs_;
    IMakeApp& make_app_;
    IMakeVar& make_var_;
    uint64_t sample_nodes_;
};

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::nf_sampler(
    ISampleUniform& sample_uniform, IMakeAbs& make_abs, IMakeApp& make_app, IMakeVar& make_var,
    uint64_t sample_nodes)
    : sample_uniform_(sample_uniform)
    , make_abs_(make_abs)
    , make_app_(make_app)
    , make_var_(make_var)
    , sample_nodes_(sample_nodes) {
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr> nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::sample() {
    DEBUG_ASSERT(sample_nodes_ >= 2);
    return sample_nf(0, sample_nodes_);
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
uint64_t nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::min_nf(uint32_t binders) {
    if(binders == 0)
        return 2;
    return 1;
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr>
nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::sample_var(uint32_t binders) {
    DEBUG_ASSERT(binders > 0);
    uint32_t index = static_cast<uint32_t>(sample_uniform_.sample_uniform(binders));
    return make_var_.make_var(index);
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr>
nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::sample_app(uint32_t binders,
                                                                    uint64_t budget) {
    uint64_t rest = budget - 1;
    uint64_t min_left = 1;
    uint64_t min_right = min_nf(binders);
    uint64_t max_left = rest - min_right;
    uint64_t left_span = max_left - min_left + 1;
    uint64_t left_budget = min_left + sample_uniform_.sample_uniform(left_span);
    uint64_t right_budget = rest - left_budget;
    return make_app_.make_app(sample_ne(binders, left_budget), sample_nf(binders, right_budget));
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr>
nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::sample_ne(uint32_t binders,
                                                                   uint64_t budget) {
    DEBUG_ASSERT(binders > 0);
    if(budget < 3)
        return sample_var(binders);
    if(sample_uniform_.sample_uniform(2) == 0)
        return sample_var(binders);
    return sample_app(binders, budget);
}

template<typename ISampleUniform, typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr>
nf_sampler<ISampleUniform, IMakeAbs, IMakeApp, IMakeVar>::sample_nf(uint32_t binders,
                                                                   uint64_t budget) {
    DEBUG_ASSERT(budget >= min_nf(binders));
    if(binders == 0)
        return make_abs_.make_abs(sample_nf(1, budget - 1));
    if(budget == 1)
        return sample_var(binders);
    bool can_app = budget >= 3;
    uint64_t nchoices = can_app ? 3 : 2;
    uint64_t pick = sample_uniform_.sample_uniform(nchoices);
    if(pick == 0)
        return sample_var(binders);
    if(pick == 1)
        return make_abs_.make_abs(sample_nf(binders + 1, budget - 1));
    return sample_app(binders, budget);
}

#endif
