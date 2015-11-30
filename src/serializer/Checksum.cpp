//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "Checksum.h"
#include "sha256.h"
#include <iostream>
#include <sstream>

std::string computeChecksum(const void* data, int length)
{
    BYTE hash[32];
    sha256(data, length, hash);

    std::ostringstream ss;
    for (int i = 0; i < 32; ++i)
        ss << std::hex << std::uppercase << static_cast<int>(hash[i]);

    return ss.str();
}

