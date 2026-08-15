#ifndef ASEX_CHILD_GENERATOR_HPP
#define ASEX_CHILD_GENERATOR_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include "value_objects/asex_agent.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"
#include "debug_assert.hpp"

template<typename ISampleUniform, typename IEncodeUint, typename IMakeTrue, typename IMakeFalse,
         typename IMakeCombinator, typename IApplyRecursor, typename INormalize>
struct asex_child_generator {
    asex_child_generator(ISampleUniform& sample_uniform, IEncodeUint& encode_uint,
                         IMakeTrue& make_true, IMakeFalse& make_false,
                         IMakeCombinator& make_combinator, IApplyRecursor& apply_recursor,
                         INormalize& normalize, uint64_t bit_width);
    std::optional<asex_agent> generate(const recursor& rec);
private:
    ISampleUniform& sample_uniform_;
    IEncodeUint& encode_uint_;
    IMakeTrue& make_true_;
    IMakeFalse& make_false_;
    IMakeCombinator& make_combinator_;
    IApplyRecursor& apply_recursor_;
    INormalize& normalize_;
    uint64_t bit_width_;
};

template<typename ISampleUniform, typename IEncodeUint, typename IMakeTrue, typename IMakeFalse,
         typename IMakeCombinator, typename IApplyRecursor, typename INormalize>
asex_child_generator<ISampleUniform, IEncodeUint, IMakeTrue, IMakeFalse, IMakeCombinator,
                     IApplyRecursor, INormalize>::
    asex_child_generator(ISampleUniform& sample_uniform, IEncodeUint& encode_uint,
                         IMakeTrue& make_true, IMakeFalse& make_false,
                         IMakeCombinator& make_combinator, IApplyRecursor& apply_recursor,
                         INormalize& normalize, uint64_t bit_width)
    : sample_uniform_(sample_uniform)
    , encode_uint_(encode_uint)
    , make_true_(make_true)
    , make_false_(make_false)
    , make_combinator_(make_combinator)
    , apply_recursor_(apply_recursor)
    , normalize_(normalize)
    , bit_width_(bit_width) {
}

template<typename ISampleUniform, typename IEncodeUint, typename IMakeTrue, typename IMakeFalse,
         typename IMakeCombinator, typename IApplyRecursor, typename INormalize>
std::optional<asex_agent> asex_child_generator<ISampleUniform, IEncodeUint, IMakeTrue, IMakeFalse,
                                               IMakeCombinator, IApplyRecursor,
                                               INormalize>::generate(const recursor& rec) {
    DEBUG_ASSERT(bit_width_ > 0);
    DEBUG_ASSERT(bit_width_ <= 63);
    uint64_t n = 1ULL << bit_width_;
    uint64_t bits = sample_uniform_.sample_uniform(n);
    std::shared_ptr<expr> rand = encode_uint_.encode_uint(bits);
    std::shared_ptr<expr> y = make_combinator_.make_combinator();
    std::shared_ptr<expr> flag_true = make_true_.make_true();
    std::shared_ptr<expr> flag_false = make_false_.make_false();
    std::shared_ptr<expr> rec_term = apply_recursor_.apply(rec, flag_true, y, rand);
    std::shared_ptr<expr> pol_term = apply_recursor_.apply(rec, flag_false, y, rand);
    std::optional<std::shared_ptr<expr>> rec_nf = normalize_.normalize(rec_term);
    if(!rec_nf.has_value())
        return std::nullopt;
    std::optional<std::shared_ptr<expr>> pol_nf = normalize_.normalize(pol_term);
    if(!pol_nf.has_value())
        return std::nullopt;
    return asex_agent{recursor{*rec_nf}, policy{*pol_nf}};
}

#endif
