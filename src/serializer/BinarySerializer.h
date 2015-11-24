#pragma once

#include <iostream>
#include <vector>

namespace ser {

    class BinarySerializer
    {
    public:
        BinarySerializer() { }

        /**
         * Initializes the binary serializer
         */
        void Init() { }

        /**
         * Write array to output stream
         *
         * The input to this routine is a raw array containing the values
         * which have to be written to the file. This method serializes these values
         * into a single vector by copying the data. The result will be in lexicographic
         * column-major (i.e. increasing strides) order.
         *
         * The provided buffer to store the serialized data is allocated by the routine.
         *
         * @param data The pointer to the binary data
         * @param bytesPerElement Size of every single element
         * @param isize The size of the array in the first dimension
         * @param jsize The size of the array in the second dimension
         * @param ksize The size of the array in the third dimension
         * @param lsize The size of the array in the fourth dimension
         * @param istride The stride (in bytes) of the array in the first dimension
         * @param jstride The stride (in bytes) of the array in the second dimension
         * @param kstride The stride (in bytes) of the array in the third dimension
         * @param lstride The stride (in bytes) of the array in the fourth dimension
         * @param binaryData The data serialized into a buffer
         *
         * @return The checksum of the binary data is returned
         */
        std::string WriteArray(const void* data, int bytesPerElement,
                               int isize, int jsize, int ksize, int lsize,
                               int istride, int jstride, int kstride, int lstride,
                               std::vector<char>& binaryData) const;

        /**
        * Read array from input stream
        *
        * The serialized binary data present in the input buffer in lexicographic
        * column-major order are emplaced in the provided multi-dimensional raw array
        * described by the given sizes and strides. The array is assumed to be already
        * allocated with a sufficient size.
        *
        * @param data The pointer to the binary data array that must be filled
        * @param bytesPerElement Size of every single element
        * @param isize The size of the array in the first dimension
        * @param jsize The size of the array in the second dimension
        * @param ksize The size of the array in the third dimension
        * @param lsize The size of the array in the fourth dimension
        * @param istride The stride (in bytes) of the array in the first dimension
        * @param jstride The stride (in bytes) of the array in the second dimension
        * @param kstride The stride (in bytes) of the array in the third dimension
        * @param lstride The stride (in bytes) of the array in the fourth dimension
        * @param binaryData The pointer to the serial binary data
        */
        void ReadArray(void* data, int bytesPerElement,
                       int isize, int jsize, int ksize, int lsize,
                       int istride, int jstride, int kstride, int lstride,
                       const std::vector<char>& binaryData) const;
    };

} // namespace ser
