#include "BinarySerializer.h"
#include "Checksum.h"
#include <cstring>

using namespace ser;

std::string BinarySerializer::WriteArray(const void* data, int bytesPerElement,
        int isize, int jsize, int ksize, int lsize,
        int istride, int jstride, int kstride, int lstride,
        std::vector<char>& binaryData) const
{
    const char* pData = reinterpret_cast<const char*>(data);

    // Compute data size and reserve memory
    const int dataSize = bytesPerElement * (isize*jsize*ksize*lsize);
    binaryData.resize(dataSize);
    char* pVec = binaryData.data();

    // Write data in column-major (fortran) ordering
    for (int l = 0; l < lsize; ++l)
        for (int k = 0; k < ksize; ++k)
            for (int j = 0; j < jsize; ++j)
                for (int i = 0; i < isize; ++i)
                {
                    const char* pIn = pData + (i*istride + j*jstride + k*kstride + l*lstride);
                    std::memcpy(pVec, pIn, bytesPerElement);
                    pVec += bytesPerElement;
                }

    // Compute checksum
    return computeChecksum(binaryData.data(), binaryData.size());
}

void BinarySerializer::ReadArray(void* data, int bytesPerElement,
        int isize, int jsize, int ksize, int lsize,
        int istride, int jstride, int kstride, int lstride, 
        const std::vector<char>& binaryData) const
{
    char* pData = reinterpret_cast<char*>(data);
    const char* pVec = binaryData.data();

    // Read data in column-major (fortran) ordering
    for (int l = 0; l < lsize; ++l)
        for (int k = 0; k < ksize; ++k)
            for (int j = 0; j < jsize; ++j)
                for (int i = 0; i < isize; ++i)
                {
                    char* pOut = pData + i*istride + j*jstride + k*kstride + l*lstride;
                    std::memcpy(pOut, pVec, bytesPerElement);
                    pVec += bytesPerElement;
                }
}

