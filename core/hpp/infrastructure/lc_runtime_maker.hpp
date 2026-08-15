#ifndef LC_RUNTIME_MAKER_HPP
#define LC_RUNTIME_MAKER_HPP

#include <memory>
#include "infrastructure/rc_pool.hpp"
#include "infrastructure/runtime.hpp"
#include "value_objects/expr.hpp"

struct lc_runtime_maker {
    lc_runtime_maker(rc_pool<expr>& out_nodes);
    runtime make(std::shared_ptr<expr> term);
private:
    rc_pool<expr>& out_nodes_;
};

#endif
