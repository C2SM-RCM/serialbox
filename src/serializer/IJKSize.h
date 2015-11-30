#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <cassert>
#include "Definitions.h"

namespace ser {
    /**
    * @class IJKSize
    * Container for i, j, k Size
    */
    class IJKSize
    {
    public:

        IJKSize()
        {
            iSize_ = 0;
            jSize_ = 0;
            kSize_ = 0;
        }

        ~IJKSize() {}


        IJKSize(const IJKSize& other)
        {
            *this = other;
        }

        IJKSize& operator= (const IJKSize& other)
        {
            iSize_ = other.iSize_;
            jSize_ = other.jSize_;
            kSize_ = other.kSize_;
            // by convention
            return *this;
        }

        bool operator== (const IJKSize& other) const
        {
            return (
                        (iSize() == other.iSize()) &&
                        (jSize() == other.jSize()) &&
                        (kSize() == other.kSize())
                        );
        }

        /**
        * Init the container
        * @param iSize size in i dimension
        * @param jSize size in j dimension
        * @param kSize size in k dimension
        */

        void Init(const int iSize, const int jSize, const int kSize)
        {
            // assert that sizes are not negative
            assert(iSize >= 0 && jSize >= 0 && kSize >= 0);

            // store the size per dimension
            iSize_ = iSize;
            jSize_ = jSize;
            kSize_ = kSize;
        }

        /**
        * Size in i dimension
        * @return size
        */

        int iSize() const { return iSize_; }

        /**
        * Size in j dimension
        * @return size
        */

        int jSize() const { return jSize_; }

        /**
        * Size in k dimension
        * @return size
        */

        int kSize() const { return kSize_; }

        /**
        * @return true if the size is empty
        */

        bool empty() const { return iSize_ <= 0 || jSize_ <=0 || kSize_ <=0; }

    private:
        int iSize_;
        int jSize_;
        int kSize_;
    };

} // namespace ser
