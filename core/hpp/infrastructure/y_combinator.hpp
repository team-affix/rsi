#ifndef Y_COMBINATOR_HPP
#define Y_COMBINATOR_HPP

#include <memory>
#include "value_objects/expr.hpp"

template<typename IMakeAbs, typename IMakeApp, typename IMakeVar>
struct y_combinator {
    y_combinator(IMakeAbs& make_abs, IMakeApp& make_app, IMakeVar& make_var);
    std::shared_ptr<expr> make_combinator();
private:
    IMakeAbs& make_abs_;
    IMakeApp& make_app_;
    IMakeVar& make_var_;
};

template<typename IMakeAbs, typename IMakeApp, typename IMakeVar>
y_combinator<IMakeAbs, IMakeApp, IMakeVar>::y_combinator(
    IMakeAbs& make_abs, IMakeApp& make_app, IMakeVar& make_var)
    : make_abs_(make_abs)
    , make_app_(make_app)
    , make_var_(make_var) {
}

template<typename IMakeAbs, typename IMakeApp, typename IMakeVar>
std::shared_ptr<expr> y_combinator<IMakeAbs, IMakeApp, IMakeVar>::make_combinator() {
    std::shared_ptr<expr> x = make_var_.make_var(0);
    std::shared_ptr<expr> xx = make_app_.make_app(x, x);
    std::shared_ptr<expr> f = make_var_.make_var(1);
    std::shared_ptr<expr> fxx = make_app_.make_app(f, xx);
    std::shared_ptr<expr> inner = make_abs_.make_abs(fxx);
    std::shared_ptr<expr> applied = make_app_.make_app(inner, inner);
    return make_abs_.make_abs(applied);
}

#endif
