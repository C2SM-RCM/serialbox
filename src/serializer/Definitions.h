#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

namespace ser {
    // define the floating point type
#ifdef SINGLEPRECISION
    typedef float Real;
#else
    typedef double Real;
#endif
    typedef const Real ConstReal;

} //namespace ser
