#ifndef NAT_REWARD_HPP
#define NAT_REWARD_HPP

#include <cstdint>
#include <optional>
#include "nat_decoder.hpp"
#include "value_objects/policy.hpp"

struct nat_reward {
    nat_reward(uint64_t peak, uint64_t jackpot);
    double evaluate(const policy& pol);
private:
    uint64_t peak_;
    uint64_t jackpot_;
};

inline nat_reward::nat_reward(uint64_t peak, uint64_t jackpot)
    : peak_(peak)
    , jackpot_(jackpot) {
}

inline double nat_reward::evaluate(const policy& pol) {
    std::optional<uint64_t> decoded = decode_nat(pol.term.get());
    if(!decoded.has_value())
        return 0;
    double n = static_cast<double>(*decoded);
    double peak = static_cast<double>(peak_);
    if(*decoded == jackpot_)
        return peak * peak + 10.0;
    return (2.0 * peak) * n - n * n;
}

#endif
