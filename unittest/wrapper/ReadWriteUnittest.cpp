//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "gtest/gtest.h"
#include "wrapper/SerializationWrapper.h"
#include "serializer/Serializer.h"
#include <vector>

using namespace ser;

class ReadWriteUnittest: public ::testing::Test {
protected:

    // These vectors are used as Fortran-style storage
    std::vector<int> fieldInt2, fieldInt3;
    std::vector<double> fieldReal1, fieldReal3;
    std::vector<int> fieldcheckInt2, fieldcheckInt3;
    std::vector<double> fieldcheckReal1, fieldcheckReal3;

    std::string realtype;
    int iSize, jSize, kSize;
    int intSize, realSize;

    Serializer ser;
    Savepoint sp;

    void SetUp() {
        iSize = 12;
        jSize = 18;
        kSize = 10;

#ifdef _SINGLEPRECISION_
        realtype = "float";
#else
        realtype = "double";
#endif
        intSize = sizeof(int);
        realSize = sizeof(double);

        // Allocate fields
        fieldInt2.resize(iSize*kSize);
        fieldInt3.resize(iSize*jSize*kSize);
        fieldReal1.resize(jSize);
        fieldReal3.resize(iSize*jSize*kSize);
        fieldcheckInt2.resize(iSize*kSize);
        fieldcheckInt3.resize(iSize*jSize*kSize);
        fieldcheckReal1.resize(jSize);
        fieldcheckReal3.resize(iSize*jSize*kSize);

        // Initialize serialization objects
        ser.Init("./", "ReadWriteUnittest", SerializerOpenModeWrite);
        sp.Init("TestSavepoint");

        // Register fields
        ser.RegisterField("int3", "int", intSize, iSize, jSize, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("int2", "int", intSize, iSize, 1, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("real3", realtype, realSize, iSize, jSize, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("real1", realtype, realSize, 1, jSize, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);

        FillFields();
    }

    void TearDown()
    {
        std::remove("ReadWriteUnittest.json");
        std::remove("ReadWriteUnittest_int3.dat");
        std::remove("ReadWriteUnittest_int2.dat");
        std::remove("ReadWriteUnittest_real3.dat");
        std::remove("ReadWriteUnittest_real1.dat");
    }

    void FillFields() {
        for (int i = 0; i < iSize; ++i)
            for (int j = 0; j < jSize; ++j)
                for (int k = 0; k < kSize; ++k)
                {
                    fieldInt3[i + j*iSize + k*iSize*jSize] = i + 2*j - 12*k*i;
                    fieldReal3[i + j*iSize + k*iSize*jSize] = 1.125*i + 2.25*j - 12.5*k*i;
                }

        for (int i = 0; i < iSize; ++i)
            for (int k = 0; k < kSize; ++k)
            {
                fieldInt2[i + k*iSize] = i - 12*k*i;
            }

        for (int j = 0; j < jSize; ++j)
        {
            fieldReal1[j] = j*j+2.875;
        }
    }

    template<typename T>
    bool CheckVector(const std::vector<T>& ref, const std::vector<T>& val)
    {
        int size = ref.size();
        EXPECT_EQ(size, val.size());

        for (int i = 0; i < size; ++i)
            if (ref[i] != val[i])
                return false;

        return true;
    }
};

/**
 * In this test we will write a couple of fields using the C API
 * and retrieve the data using the C++ API
 */
TEST_F(ReadWriteUnittest, Write)
{
    // Write fields
    fs_write_field(&ser, &sp, "int3", 4, fieldInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    fs_write_field(&ser, &sp, "int2", 4, fieldInt2.data(), intSize, 0, intSize*iSize, 0);
    fs_write_field(&ser, &sp, "real3", 5, fieldReal3.data(), realSize, realSize*iSize, realSize*iSize*jSize, 0);
    fs_write_field(&ser, &sp, "real1", 5, fieldReal1.data(), 0, realSize, 0, 0);

    // Load with C++
    ser.ReadField("int3", sp, fieldcheckInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    ser.ReadField("int2", sp, fieldcheckInt2.data(), intSize, 0, intSize*iSize, 0);
    ser.ReadField("real3", sp, fieldcheckReal3.data(), realSize, realSize*iSize, realSize*iSize*jSize, 0);
    ser.ReadField("real1", sp, fieldcheckReal1.data(), 0, realSize, 0, 0);

    // Check
    ASSERT_TRUE(CheckVector(fieldInt3, fieldcheckInt3));
    ASSERT_TRUE(CheckVector(fieldInt2, fieldcheckInt2));
    ASSERT_TRUE(CheckVector(fieldReal3, fieldcheckReal3));
    ASSERT_TRUE(CheckVector(fieldReal1, fieldcheckReal1));
}

/**
 * In this test we will write a couple of fields using the C++ API
 * and retrieve the data using the C API
 */
TEST_F(ReadWriteUnittest, Read)
{
    // Write with C++
    ser.WriteField("int3", sp, fieldInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    ser.WriteField("int2", sp, fieldInt2.data(), intSize, 0, intSize*iSize, 0);
    ser.WriteField("real3", sp, fieldReal3.data(), realSize, realSize*iSize, realSize*iSize*jSize, 0);
    ser.WriteField("real1", sp, fieldReal1.data(), 0, realSize, 0, 0);

    // Load fields
    fs_read_field(&ser, &sp, "int3", 4, fieldcheckInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    fs_read_field(&ser, &sp, "int2", 4, fieldcheckInt2.data(), intSize, 0, intSize*iSize, 0);
    fs_read_field(&ser, &sp, "real3", 5, fieldcheckReal3.data(), realSize, realSize*iSize, realSize*iSize*jSize, 0);
    fs_read_field(&ser, &sp, "real1", 5, fieldcheckReal1.data(), 0, realSize, 0, 0);

    // Check
    ASSERT_TRUE(CheckVector(fieldInt3, fieldcheckInt3));
    ASSERT_TRUE(CheckVector(fieldInt2, fieldcheckInt2));
    ASSERT_TRUE(CheckVector(fieldReal3, fieldcheckReal3));
    ASSERT_TRUE(CheckVector(fieldReal1, fieldcheckReal1));
}

