#pragma once

namespace ser {
    // define the floating point type
#ifdef SINGLEPRECISION
    typedef float Real;
#else
    typedef double Real;
#endif
    typedef const Real ConstReal;

} //namespace ser
