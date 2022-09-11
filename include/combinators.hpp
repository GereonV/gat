#ifndef _GAT_COMBINATORS_HPP_
#define _GAT_COMBINATORS_HPP_

#include "gat.hpp"

namespace gat::combinators {

    //              many<Parser    a   > = Parser a    # any
    //              some<Parser    a   > = Parser a    # any
    //            choice<Parser... a   > = Parser a    # any
    //          optional<Parser    a   > = Parser a?   # one
    //          sequence<Parser    a...> = Parser a...
    //          min<Int><Parser    a   > = Parser [a]
    //       map<a -> b><Parser    a   > = Parser b
    //      exactly<Int><Parser    a   > = Parser a[]
    //    left<Parser a><Parser    b   > = Parser a
    //   right<Parser a><Parser    b   > = Parser b
    //   ahead<Parser a><Parser    b   > = Parser a
    // between<Int><Int><Parser    a   > = Parser [a]

}

#endif // _GAT_COMBINATORS_HPP_
