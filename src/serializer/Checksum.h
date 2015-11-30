#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <string>

    /**
    * Computes the SHA-256 checksum of the data and returns it
    * as a 64-character long string.
    *
    * @param data The buffer containing the data
    * @param length The extent in bytes of this buffer
    *
    * @return A C++ string with the checksum in upper-case hexadecimal is returned
    */
    std::string computeChecksum(const void* data, int length);

