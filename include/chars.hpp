#ifndef _GAT_CHARS_HPP_
#define _GAT_CHARS_HPP_

#include "gat.hpp"

namespace gat::chars {

    // Parser Char:
    //               one = [^]
    //            bdigit = [01]
    //             digit = [0-9]
    //            xdigit = [0-9A-Za-z]
    //         uppercase = [A-Z]
    //         lowercase = [a-z]
    //            letter = [A-Za-z]
    //              word = [A-Za-z0-9_]
    //               dot = [^\n\r]
    //        whitespace = [ \n\r\t\v\f]
    //       exact<Char> = [%]
    //      set<Char...> = [%...]
    //    any_of<Char[]> = [%]
    // range<Char, Char> = [$1-$2]
    //  not<Parser Char> = [^$]

    // string<Char[]> = Parser StringView

}

#endif // _GAT_CHARS_HPP_
