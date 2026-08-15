#ifndef RECURSOR_HPP
#define RECURSOR_HPP

#include <memory>
#include "value_objects/expr.hpp"

struct recursor {
    std::shared_ptr<expr> term;
    auto operator<=>(const recursor&) const = default;
};

#endif
