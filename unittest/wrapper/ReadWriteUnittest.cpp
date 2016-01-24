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
    std::vector<double> fieldDouble1, fieldDouble3;
    std::vector<float> fieldFloat1, fieldFloat3;
    std::vector<int> fieldcheckInt2, fieldcheckInt3;
    std::vector<double> fieldcheckDouble1, fieldcheckDouble3;
    std::vector<float> fieldcheckFloat1, fieldcheckFloat3;

    int iSize, jSize, kSize;
    int intSize, doubleSize, floatSize;

    Serializer ser;
    Savepoint sp;

    void SetUp() {
        iSize = 12;
        jSize = 18;
        kSize = 10;

        intSize = sizeof(int);
        doubleSize = sizeof(double);
        floatSize = sizeof(float);

        // Allocate fields
        fieldInt2.resize(iSize*kSize);
        fieldInt3.resize(iSize*jSize*kSize);
        fieldDouble1.resize(jSize);
        fieldDouble3.resize(iSize*jSize*kSize);
        fieldFloat1.resize(jSize);
        fieldFloat3.resize(iSize*jSize*kSize);
        fieldcheckInt2.resize(iSize*kSize);
        fieldcheckInt3.resize(iSize*jSize*kSize);
        fieldcheckDouble1.resize(jSize);
        fieldcheckDouble3.resize(iSize*jSize*kSize);
        fieldcheckFloat1.resize(jSize);
        fieldcheckFloat3.resize(iSize*jSize*kSize);

        // Initialize serialization objects
        ser.Init("./", "ReadWriteUnittest", SerializerOpenModeWrite);
        sp.Init("TestSavepoint");

        // Register fields
        ser.RegisterField("int3", "int", intSize, iSize, jSize, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("int2", "int", intSize, iSize, 1, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("double3", "double", doubleSize, iSize, jSize, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("double1", "double", doubleSize, 1, jSize, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("float3", "float", floatSize, iSize, jSize, kSize, 1, 0, 0, 0, 0, 0, 0, 0, 0);
        ser.RegisterField("float1", "float", floatSize, 1, jSize, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);

        FillFields();
    }

    void TearDown()
    {
        std::remove("ReadWriteUnittest.json");
        std::remove("ReadWriteUnittest_int3.dat");
        std::remove("ReadWriteUnittest_int2.dat");
        std::remove("ReadWriteUnittest_double3.dat");
        std::remove("ReadWriteUnittest_double1.dat");
        std::remove("ReadWriteUnittest_float3.dat");
        std::remove("ReadWriteUnittest_float1.dat");
    }

    void FillFields() {
        for (int i = 0; i < iSize; ++i)
            for (int j = 0; j < jSize; ++j)
                for (int k = 0; k < kSize; ++k)
                {
                    fieldInt3[i + j*iSize + k*iSize*jSize] = i + 2*j - 12*k*i;
                    fieldDouble3[i + j*iSize + k*iSize*jSize] = 1.125*i + 2.25*j - 12.5*k*i;
                    fieldFloat3[i + j*iSize + k*iSize*jSize] = 1.5f*i + 4.25f*j - 16.0f*k*i;
                }

        for (int i = 0; i < iSize; ++i)
            for (int k = 0; k < kSize; ++k)
            {
                fieldInt2[i + k*iSize] = i - 12*k*i;
            }

        for (int j = 0; j < jSize; ++j)
        {
            fieldDouble1[j] = j*j+2.875;
            fieldFloat1[j] = j*j+3.5f;
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
    fs_write_field(&ser, &sp, "double3", 7, fieldDouble3.data(), doubleSize, doubleSize*iSize, doubleSize*iSize*jSize, 0);
    fs_write_field(&ser, &sp, "double1", 7, fieldDouble1.data(), 0, doubleSize, 0, 0);
    fs_write_field(&ser, &sp, "float3", 6, fieldFloat3.data(), floatSize, floatSize*iSize, floatSize*iSize*jSize, 0);
    fs_write_field(&ser, &sp, "float1", 6, fieldFloat1.data(), 0, floatSize, 0, 0);

    // Load with C++
    ser.ReadField("int3", sp, fieldcheckInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    ser.ReadField("int2", sp, fieldcheckInt2.data(), intSize, 0, intSize*iSize, 0);
    ser.ReadField("double3", sp, fieldcheckDouble3.data(), doubleSize, doubleSize*iSize, doubleSize*iSize*jSize, 0);
    ser.ReadField("double1", sp, fieldcheckDouble1.data(), 0, doubleSize, 0, 0);
    ser.ReadField("float3", sp, fieldcheckFloat3.data(), floatSize, floatSize*iSize, floatSize*iSize*jSize, 0);
    ser.ReadField("float1", sp, fieldcheckFloat1.data(), 0, floatSize, 0, 0);

    // Check
    ASSERT_TRUE(CheckVector(fieldInt3, fieldcheckInt3));
    ASSERT_TRUE(CheckVector(fieldInt2, fieldcheckInt2));
    ASSERT_TRUE(CheckVector(fieldDouble3, fieldcheckDouble3));
    ASSERT_TRUE(CheckVector(fieldDouble1, fieldcheckDouble1));
    ASSERT_TRUE(CheckVector(fieldFloat3, fieldcheckFloat3));
    ASSERT_TRUE(CheckVector(fieldFloat1, fieldcheckFloat1));
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
    ser.WriteField("double3", sp, fieldDouble3.data(), doubleSize, doubleSize*iSize, doubleSize*iSize*jSize, 0);
    ser.WriteField("double1", sp, fieldDouble1.data(), 0, doubleSize, 0, 0);
    ser.WriteField("float3", sp, fieldFloat3.data(), floatSize, floatSize*iSize, floatSize*iSize*jSize, 0);
    ser.WriteField("float1", sp, fieldFloat1.data(), 0, floatSize, 0, 0);

    // Load fields
    fs_read_field(&ser, &sp, "int3", 4, fieldcheckInt3.data(), intSize, intSize*iSize, intSize*iSize*jSize, 0);
    fs_read_field(&ser, &sp, "int2", 4, fieldcheckInt2.data(), intSize, 0, intSize*iSize, 0);
    fs_read_field(&ser, &sp, "double3", 7, fieldcheckDouble3.data(), doubleSize, doubleSize*iSize, doubleSize*iSize*jSize, 0);
    fs_read_field(&ser, &sp, "double1", 7, fieldcheckDouble1.data(), 0, doubleSize, 0, 0);
    fs_read_field(&ser, &sp, "float3", 6, fieldcheckFloat3.data(), floatSize, floatSize*iSize, floatSize*iSize*jSize, 0);
    fs_read_field(&ser, &sp, "float1", 6, fieldcheckFloat1.data(), 0, floatSize, 0, 0);

    // Check
    ASSERT_TRUE(CheckVector(fieldInt3, fieldcheckInt3));
    ASSERT_TRUE(CheckVector(fieldInt2, fieldcheckInt2));
    ASSERT_TRUE(CheckVector(fieldDouble3, fieldcheckDouble3));
    ASSERT_TRUE(CheckVector(fieldDouble1, fieldcheckDouble1));
    ASSERT_TRUE(CheckVector(fieldFloat3, fieldcheckFloat3));
    ASSERT_TRUE(CheckVector(fieldFloat1, fieldcheckFloat1));
}

