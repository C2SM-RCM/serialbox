//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <iostream>
#include <sstream>
#include "gtest/gtest.h"

#include "serializer/BinarySerializer.h"
#include "serializer/Checksum.h"

using namespace ser;

TEST(BinarySerializerUnittest, TestItAll)
{
    BinarySerializer bs;
    bs.Init();

    // Create some data (layout: JKIL, plus one stride in l)
    const int isize = 5, jsize = 6, ksize = 9, lsize = 8;
    const int lstride = 2, istride = lstride*lsize, kstride = istride*isize, jstride = kstride*ksize;
    const int allocsize = jstride*jsize;
    ASSERT_EQ(5*6*9*8*2, allocsize);
    double *pData      = new double[allocsize];
    double *pDataCheck = new double[allocsize];

    for (int i = 0; i < isize; ++i)
        for (int j = 0; j < jsize; ++j)
            for (int k = 0; k < ksize; ++k)
                for (int l = 0; l < lsize; ++l)
                {
                    pData[i*istride + j*jstride + l*lstride + k*kstride]
                        = i*3. + j*2.5 - k*10.25 + l*6.825;
                }

    // Serialize array
    std::vector<char> sVector;
    const std::string checksum =
    bs.WriteArray(pData, sizeof(double), isize, jsize, ksize, lsize,
        istride*sizeof(double), jstride*sizeof(double), kstride*sizeof(double), lstride*sizeof(double), sVector);

    // Check binary data
    const double* sData = reinterpret_cast<const double*>(sVector.data());
    for (int l = 0; l < lsize; ++l)
        for (int k = 0; k < ksize; ++k)
            for (int j = 0; j < jsize; ++j)
                for (int i = 0; i < isize; ++i)
                {
                    ASSERT_EQ(pData[i*istride + j*jstride + l*lstride + k*kstride], *sData);
                    sData += 1;
                }

    // Check checksum
    ASSERT_EQ(computeChecksum(sVector.data(), sVector.size()), checksum);

    // Prepare string stream
    bs.ReadArray(pDataCheck, sizeof(double), isize, jsize, ksize, lsize,
        istride*sizeof(double), jstride*sizeof(double), kstride*sizeof(double), lstride*sizeof(double), sVector);

    // Check reconstructed array
    for (int i = 0; i < isize; ++i)
        for (int j = 0; j < jsize; ++j)
            for (int k = 0; k < ksize; ++k)
                for (int l = 0; l < lsize; ++l)
                {
                    ASSERT_EQ(
                        pData     [i*istride + j*jstride + l*lstride + k*kstride],
                        pDataCheck[i*istride + j*jstride + l*lstride + k*kstride]
                        );
                }

    // Release memory
    delete[] pData;
    delete[] pDataCheck;
}

