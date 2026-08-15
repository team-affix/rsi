#ifndef ASEX_GERMINATOR_HPP
#define ASEX_GERMINATOR_HPP

#include <memory>
#include <optional>
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_seed.hpp"
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

template<typename IEncodeUint, typename IMakeTrue, typename IMakeFalse, typename IMakeCombinator,
         typename IApplyRecursor, typename INormalize>
struct asex_germinator {
    asex_germinator(IEncodeUint& encode_uint, IMakeTrue& make_true, IMakeFalse& make_false,
                    IMakeCombinator& make_combinator, IApplyRecursor& apply_recursor,
                    INormalize& normalize);
    std::optional<asex_agent> germinate(const asex_seed& seed);
private:
    IEncodeUint& encode_uint_;
    IApplyRecursor& apply_recursor_;
    INormalize& normalize_;
    std::shared_ptr<expr> y_;
    std::shared_ptr<expr> flag_true_;
    std::shared_ptr<expr> flag_false_;
};

template<typename IEncodeUint, typename IMakeTrue, typename IMakeFalse, typename IMakeCombinator,
         typename IApplyRecursor, typename INormalize>
asex_germinator<IEncodeUint, IMakeTrue, IMakeFalse, IMakeCombinator, IApplyRecursor,
                INormalize>::asex_germinator(IEncodeUint& encode_uint, IMakeTrue& make_true,
                                             IMakeFalse& make_false,
                                             IMakeCombinator& make_combinator,
                                             IApplyRecursor& apply_recursor,
                                             INormalize& normalize)
    : encode_uint_(encode_uint)
    , apply_recursor_(apply_recursor)
    , normalize_(normalize)
    , y_(make_combinator.make_combinator())
    , flag_true_(make_true.make_true())
    , flag_false_(make_false.make_false()) {
}

template<typename IEncodeUint, typename IMakeTrue, typename IMakeFalse, typename IMakeCombinator,
         typename IApplyRecursor, typename INormalize>
std::optional<asex_agent>
asex_germinator<IEncodeUint, IMakeTrue, IMakeFalse, IMakeCombinator, IApplyRecursor,
                INormalize>::germinate(const asex_seed& seed) {
    const recursor& rec = seed.progenitor.parent.rec;
    std::shared_ptr<expr> index = encode_uint_.encode_uint(seed.index);
    std::shared_ptr<expr> rand = encode_uint_.encode_uint(seed.rand);
    std::shared_ptr<expr> rec_term = apply_recursor_.apply(rec, flag_true_, y_, index, rand);
    std::shared_ptr<expr> pol_term = apply_recursor_.apply(rec, flag_false_, y_, index, rand);
    std::optional<std::shared_ptr<expr>> rec_nf = normalize_.normalize(rec_term);
    if(!rec_nf.has_value())
        return std::nullopt;
    std::optional<std::shared_ptr<expr>> pol_nf = normalize_.normalize(pol_term);
    if(!pol_nf.has_value())
        return std::nullopt;
    return asex_agent{recursor{*rec_nf}, policy{*pol_nf}};
}

#endif
