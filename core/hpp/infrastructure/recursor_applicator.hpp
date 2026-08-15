#ifndef RECURSOR_APPLICATOR_HPP
#define RECURSOR_APPLICATOR_HPP

#include <memory>
#include "value_objects/expr.hpp"
#include "value_objects/recursor.hpp"

template<typename IMakeApp>
struct recursor_applicator {
    recursor_applicator(IMakeApp& make_app);
    std::shared_ptr<expr> apply(const recursor& rec, std::shared_ptr<expr> flag,
                                std::shared_ptr<expr> y, std::shared_ptr<expr> rand);
private:
    IMakeApp& make_app_;
};

template<typename IMakeApp>
recursor_applicator<IMakeApp>::recursor_applicator(IMakeApp& make_app)
    : make_app_(make_app) {
}

template<typename IMakeApp>
std::shared_ptr<expr> recursor_applicator<IMakeApp>::apply(const recursor& rec,
                                                          std::shared_ptr<expr> flag,
                                                          std::shared_ptr<expr> y,
                                                          std::shared_ptr<expr> rand) {
    std::shared_ptr<expr> with_flag = make_app_.make_app(rec.term, flag);
    std::shared_ptr<expr> with_y = make_app_.make_app(with_flag, y);
    return make_app_.make_app(with_y, rand);
}

#endif
