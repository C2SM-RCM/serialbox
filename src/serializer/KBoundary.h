#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

namespace ser {

    /**
    * @class KBoundary
    * Container for boundary offsets in k direction.
    * Positive numbers indicate count arrow direction, negative numbers the opposite.
    * Therefore k-minus is typically negative.
    *
    *            ^ k-plus
    *            |
    *            v k-minus
    */
    class KBoundary
    {
    public:
        KBoundary()
        {
            kMinusOffset_ = 0;
            kPlusOffset_  = 0;
        }
        ~KBoundary() {}

        KBoundary(const KBoundary& other) { *this = other; }
        KBoundary& operator= (const KBoundary& other)
        {
            kMinusOffset_ = other.kMinusOffset_;
            kPlusOffset_  = other.kPlusOffset_;
            // by convention
            return *this;
        }
        bool operator== (const KBoundary& other) const
        {
            return (
                        (kMinusOffset() == other.kMinusOffset()) &&
                        (kPlusOffset()  == other.kPlusOffset())
                        );
        }

        /**
        * Init the container
        * @param kMinusOffset Offset of boundary in k-minus direction of domain
        * @param kPlusOffset Offset of boundary in k-plus direction of domain
        */
        void Init(const int kMinusOffset, const int kPlusOffset)
        {
            // store the size per dimension
            kMinusOffset_ = kMinusOffset;
            kPlusOffset_  = kPlusOffset;
        }

        /**
        * @return Offset in k-minus direction
        */
        int kMinusOffset() const { return kMinusOffset_; }

        /**
        * @return Offset in k-plus direction
        */
        int kPlusOffset() const { return kPlusOffset_; }

    private:
        int kMinusOffset_;
        int kPlusOffset_;
    };

} // namespace ser
