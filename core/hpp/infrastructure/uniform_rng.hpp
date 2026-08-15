#ifndef UNIFORM_RNG_HPP
#define UNIFORM_RNG_HPP

#include <cstdint>
#include <random>

struct uniform_rng {
    uniform_rng(uint64_t seed);
    uint64_t sample_uniform(uint64_t n);
private:
    std::mt19937_64 rng_;
};

#endif
