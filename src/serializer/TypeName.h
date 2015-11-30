#pragma once
//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <string>

namespace ser {

template<typename TData>
inline std::string type_name();

#define TYPENAMEFUNCTION(type) \
    template<> \
    inline std::string type_name<type>() { return #type ; }

TYPENAMEFUNCTION(int)
TYPENAMEFUNCTION(float)
TYPENAMEFUNCTION(double)

} // namespace ser
