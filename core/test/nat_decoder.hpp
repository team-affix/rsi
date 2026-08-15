#ifndef NAT_DECODER_HPP
#define NAT_DECODER_HPP

#include <cstdint>
#include <optional>
#include "value_objects/expr.hpp"

inline std::optional<uint64_t> decode_nat(const expr* term) {
    if(term == nullptr)
        return std::nullopt;
    uint64_t leading_abs = 0;
    const expr* cur = term;
    while(const expr::abs* a = std::get_if<expr::abs>(&cur->content)) {
        ++leading_abs;
        cur = a->body.get();
        if(cur == nullptr)
            return std::nullopt;
    }
    if(leading_abs == 0)
        return std::nullopt;
    return leading_abs - 1;
}

#endif
