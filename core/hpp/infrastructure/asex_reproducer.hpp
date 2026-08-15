#ifndef ASEX_REPRODUCER_HPP
#define ASEX_REPRODUCER_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include "value_objects/asex_progenitor.hpp"
#include "value_objects/asex_seed.hpp"

template<typename ISampleUniform>
struct asex_reproducer {
    asex_reproducer(ISampleUniform& sample_uniform, std::size_t r);
    std::vector<asex_seed> reproduce(const asex_progenitor& progenitor);
private:
    static constexpr uint64_t k_bit_width = 8;
    ISampleUniform& sample_uniform_;
    std::size_t r_;
};

template<typename ISampleUniform>
asex_reproducer<ISampleUniform>::asex_reproducer(ISampleUniform& sample_uniform, std::size_t r)
    : sample_uniform_(sample_uniform)
    , r_(r) {
}

template<typename ISampleUniform>
std::vector<asex_seed>
asex_reproducer<ISampleUniform>::reproduce(const asex_progenitor& progenitor) {
    uint64_t range = 1ULL << k_bit_width;
    std::vector<asex_seed> seeds;
    for(std::size_t i = 0; i < r_; ++i) {
        uint64_t bits = sample_uniform_.sample_uniform(range);
        seeds.push_back(asex_seed{progenitor, i, bits});
    }
    return seeds;
}

#endif
