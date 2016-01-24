//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <iostream>
#include <cstdio>
#include "gtest/gtest.h"
#include "libjson.h"
#include "serializer/Serializer.h"

using namespace ser;

class SerializerUnittest : public ::testing::Test
{
protected:
    OffsetTable const & getOffsetTable(Serializer& serializer) const
    {
        return serializer.offsetTable();
    }

    FieldsTable const & getFieldsTable(Serializer& serializer) const
    {
        return serializer.fieldsTable();
    }
};

TEST_F(SerializerUnittest, Write)
{
    Serializer serializer;
    serializer.Init(".", "Data", SerializerOpenModeWrite);

    // Create 4D field with JILK storage plus stride 2
    const int isize = 6, jsize = 8, ksize = 7, lsize = 4;
    const int kstride = 2, lstride = kstride*ksize, istride = lstride*lsize, jstride = istride*isize;
    const int ibstrided = istride*sizeof(double), jbstrided = jstride*sizeof(double),
              kbstrided = kstride*sizeof(double), lbstrided = lstride*sizeof(double);
    const int ibstridef = istride*sizeof(float), jbstridef = jstride*sizeof(float),
              kbstridef = kstride*sizeof(float), lbstridef = lstride*sizeof(float);
    const int allocsize = jstride*jsize;
    std::vector<double> dataD(allocsize);
    std::vector<float> dataF(allocsize);

    for (int i = 0; i < isize; ++i)
        for (int j = 0; j < jsize; ++j)
            for (int k = 0; k < ksize; ++k)
                for (int l = 0; l < lsize; ++l)
                {
                    dataD[i*istride + j*jstride + k*kstride + l*lstride]
                        = i*12.25 + j*k*1. - 2.75 / (l+10.);
                    dataF[i*istride + j*jstride + k*kstride + l*lstride]
                        = i*12.25 + j*k*1. - 2.75 / (l+10.);
                }

    // Register various versions of this field
    serializer.RegisterField("ik"  , "double", 8, isize,     1, ksize,     1, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer.RegisterField("jk"  , "float" , 4,     1, jsize, ksize,     1, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer.RegisterField("ikl" , "double", 8, isize,     1, ksize, lsize, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer.RegisterField("ijkl", "float" , 4, isize, jsize, ksize, lsize, 0, 0, 0, 0, 0, 0, 0, 0);

    // Setting a savepoint and serializing all fields
    Savepoint sp;
    sp.Init("MySavepoint");
    sp.AddMetainfo("Value", 3.125);
    sp.AddMetainfo("ValueF", 3.625f);
    sp.AddMetainfo("MyName", "Andrea Arteaga");
    sp.AddMetainfo("MyAge", 26);
    sp.AddMetainfo("ILikeThis", true);

    const int ilevel = 3, jlevel = 0, llevel = 2;

    const double*   pIKData = dataD.data() + jstride*jlevel + lstride*llevel;
    const float*    pJKData = dataF.data() + istride*ilevel + lstride*llevel;
    const double*  pIKLData = dataD.data() + jstride*jlevel;
    const float*  pIJKLData = dataF.data();

    serializer.WriteField(  "ik", sp,   pIKData, ibstrided, jbstrided, kbstrided, lbstrided);
    serializer.WriteField(  "jk", sp,   pJKData, ibstridef, jbstridef, kbstridef, lbstridef);
    serializer.WriteField( "ikl", sp,  pIKLData, ibstrided, jbstrided, kbstrided, lbstrided);
    serializer.WriteField("ijkl", sp, pIJKLData, ibstridef, jbstridef, kbstridef, lbstridef);

    // Check metainfo
    std::ifstream dbfs("Data.json");
    std::string dbtxt((std::istreambuf_iterator<char>(dbfs)),
                       std::istreambuf_iterator<char>());
    dbfs.close();
    JSONNode dbnode = libjson::parse(dbtxt);
    JSONNode fieldstable, offsettable, globalinfo;
    ASSERT_EQ(3, dbnode.size());
    ASSERT_NO_THROW(fieldstable = dbnode.at("FieldsTable"));
    ASSERT_NO_THROW(offsettable = dbnode.at("OffsetTable"));
    ASSERT_NO_THROW(globalinfo  = dbnode.at("GlobalMetainfo"));
    ASSERT_EQ(4, fieldstable.size());
    ASSERT_EQ(1, offsettable.size());
    ASSERT_LE(1, globalinfo.size());
    ASSERT_EQ(std::string("centralized"), globalinfo.at("__format").as_string());

    // Check savepoint metainfo
    Savepoint spload;
    ASSERT_EQ(std::string("MySavepoint"), offsettable[0]["__name"].as_string());
    ASSERT_EQ(0, offsettable[0]["__id"].as_int());
    ASSERT_EQ(3.125, offsettable[0]["Value"].as_float());
    ASSERT_EQ(3.625f, offsettable[0]["ValueF"].as_float());
    ASSERT_EQ(std::string("Andrea Arteaga"), offsettable[0]["MyName"].as_string());
    ASSERT_EQ(26, offsettable[0]["MyAge"].as_int());
    ASSERT_TRUE(offsettable[0]["ILikeThis"].as_bool());

    {   // Check IK

        // Read data
        std::ifstream fs("Data_ik.dat", std::ios::binary);
        fs.seekg(0, fs.end);
        int filelength = fs.tellg();
        fs.seekg(0, fs.beg);
        ASSERT_EQ(sizeof(double)*isize*ksize, filelength);

        std::vector<char> rawdata(filelength);
        fs.read(rawdata.data(), filelength);
        fs.close();

        // Check data
        const double* pNewData = reinterpret_cast<const double*>(rawdata.data());
        for (int i = 0; i < isize; ++i)
            for (int k = 0; k < ksize; ++k)
            {
                const int j = jlevel, l = llevel;
                const double value = *(pNewData + k*isize + i);
                const double reference = dataD[i*istride + j*jstride + k*kstride + l*lstride];
                ASSERT_EQ(reference, value) << "i=" << i << ", k=" << k;
            }
    }

    {   // Check JK

        // Read data
        std::ifstream fs("Data_jk.dat", std::ios::binary);
        fs.seekg(0, fs.end);
        int filelength = fs.tellg();
        fs.seekg(0, fs.beg);
        ASSERT_EQ(sizeof(float)*jsize*ksize, filelength);

        std::vector<char> rawdata(filelength);
        fs.read(rawdata.data(), filelength);
        fs.close();

        // Check data
        const float* pNewData = reinterpret_cast<const float*>(rawdata.data());
        for (int j = 0; j < jsize; ++j)
            for (int k = 0; k < ksize; ++k)
            {
                const int i = ilevel, l = llevel;
                const float value = *(pNewData + k*jsize + j);
                const float reference = dataF[i*istride + j*jstride + k*kstride + l*lstride];
                ASSERT_EQ(reference, value) << "j=" << j << ", k=" << k;
            }
    }

    {   // Check IKL

        // Read data
        std::ifstream fs("Data_ikl.dat", std::ios::binary);
        fs.seekg(0, fs.end);
        int filelength = fs.tellg();
        fs.seekg(0, fs.beg);
        ASSERT_EQ(sizeof(double)*isize*ksize*lsize, filelength);

        std::vector<char> rawdata(filelength);
        fs.read(rawdata.data(), filelength);
        fs.close();

        // Check data
        const double* pNewData = reinterpret_cast<const double*>(rawdata.data());
        for (int i = 0; i < isize; ++i)
            for (int k = 0; k < ksize; ++k)
                for (int l = 0; l < lsize; ++l)
                {
                    const int j = jlevel;
                    const double value = *(pNewData + l*ksize*isize + k*isize + i);
                    const double reference = dataD[i*istride + j*jstride + k*kstride + l*lstride];
                    ASSERT_EQ(reference, value) << "i=" << i << ", k=" << k << ", l=" << l;
                }
    }

    {   // Check IJKL

        // Read data
        std::ifstream fs("Data_ijkl.dat", std::ios::binary);
        fs.seekg(0, fs.end);
        int filelength = fs.tellg();
        fs.seekg(0, fs.beg);
        ASSERT_EQ(sizeof(float)*isize*jsize*ksize*lsize, filelength);

        std::vector<char> rawdata(filelength);
        fs.read(rawdata.data(), filelength);
        fs.close();

        // Check data
        const float* pNewData = reinterpret_cast<const float*>(rawdata.data());
        for (int i = 0; i < isize; ++i)
            for (int j = 0; j < jsize; ++j)
                for (int k = 0; k < ksize; ++k)
                    for (int l = 0; l < lsize; ++l)
                    {
                        const float value = *(pNewData + l*ksize*jsize*isize + k*jsize*isize + j*isize + i);
                        const float reference = dataF[i*istride + j*jstride + k*kstride + l*lstride];
                        ASSERT_EQ(reference, value) << "i=" << i << ", j=" << j << ", k=" << k << ", l=" << l;
                    }
    }

    // Second savepoint: some fields are rewritten, others are kept the same
    dataD[0] += 1.;
    dataF[0] += 1.;
    // ik does not change
    // jk does not change
    // ikl changes
    // ijkl changes

    sp.Init("SecondSavepoint");

    serializer.WriteField(  "ik", sp,   pIKData, ibstrided, jbstrided, kbstrided, lbstrided);
    serializer.WriteField(  "jk", sp,   pJKData, ibstridef, jbstridef, kbstridef, lbstridef);
    serializer.WriteField( "ikl", sp,  pIKLData, ibstrided, jbstrided, kbstrided, lbstrided);
    serializer.WriteField("ijkl", sp, pIJKLData, ibstridef, jbstridef, kbstridef, lbstridef);

    // Load offsettable again
    dbfs.open("Data.json");
    dbtxt = std::string((std::istreambuf_iterator<char>(dbfs)),
                         std::istreambuf_iterator<char>());
    dbfs.close();
    dbnode = libjson::parse(dbtxt);
    ASSERT_NO_THROW(offsettable = dbnode["OffsetTable"]);
    ASSERT_EQ(2, offsettable.size());

    ASSERT_EQ(std::string("SecondSavepoint"), offsettable[1]["__name"].as_string());
    ASSERT_EQ(1, offsettable[1]["__id"].as_int());
    ASSERT_EQ(4, offsettable[1]["__offsets"].size());
    for (int i = 0; i < 4; ++i)
    {
        const std::string fieldname = offsettable[1]["__offsets"][i].name();
        if (fieldname.size() < 3)
            ASSERT_EQ(0, offsettable[1]["__offsets"][i][0].as_int());
        else
            ASSERT_LT(0, offsettable[1]["__offsets"][i][0].as_int());
    }

    // Check binary data size
    for (int i = 0; i < 4; ++i)
    {
        const std::string fieldname = offsettable[1]["__offsets"][i].name();
        int filelength_expect;
        if (fieldname ==   "ik") filelength_expect = 1*sizeof(double)*isize*  1  *ksize*  1  ;
        if (fieldname ==   "jk") filelength_expect = 1*sizeof(float )*  1  *jsize*ksize*  1  ;
        if (fieldname ==  "ikl") filelength_expect = 2*sizeof(double)*isize*  1  *ksize*lsize;
        if (fieldname == "ijkl") filelength_expect = 2*sizeof(float )*isize*jsize*ksize*lsize;

        std::ifstream fs(("Data_" + fieldname + ".dat").c_str(), std::ios::binary);
        fs.seekg(0, fs.end);
        int filelength = fs.tellg();
        fs.close();
        ASSERT_EQ(filelength_expect, filelength);
    }

    // Cleanup files
    std::remove("Data.json");
    std::remove("Data_ik.dat");
    std::remove("Data_jk.dat");
    std::remove("Data_ikl.dat");
    std::remove("Data_ijkl.dat");
}

TEST_F(SerializerUnittest, Read)
{
    Serializer serializer_write;
    serializer_write.Init(".", "Umpeppo", SerializerOpenModeWrite);

    // Create 4D field with JILK storage plus stride 2
    const int isize = 6, jsize = 8, ksize = 7, lsize = 4;
    const int kstride = 2, lstride = kstride*ksize, istride = lstride*lsize, jstride = istride*isize;
    const int ibstride = istride*sizeof(double), jbstride = jstride*sizeof(double),
              kbstride = kstride*sizeof(double), lbstride = lstride*sizeof(double);
    const int allocsize = jstride*jsize;
    std::vector<double> data(allocsize);

    for (int i = 0; i < isize; ++i)
        for (int j = 0; j < jsize; ++j)
            for (int k = 0; k < ksize; ++k)
                for (int l = 0; l < lsize; ++l)
                {
                    data[i*istride + j*jstride + k*kstride + l*lstride]
                        = i*12.25 + j*k*1. - 2.75 / (l+10.);
                }

    // Register various versions of this field
    serializer_write.RegisterField("ik"  , "double", 8, isize,     1, ksize,     1, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer_write.RegisterField("jk"  , "double", 8,     1, jsize, ksize,     1, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer_write.RegisterField("ikl" , "double", 8, isize,     1, ksize, lsize, 0, 0, 0, 0, 0, 0, 0, 0);
    serializer_write.RegisterField("ijkl", "double", 8, isize, jsize, ksize, lsize, 0, 0, 0, 0, 0, 0, 0, 0);

    // Setting a savepoint and serializing all fields
    Savepoint sp;
    sp.Init("MySavepoint");

    const int ilevel = 3, jlevel = 0, llevel = 2;

    const double*   pIKData = data.data() + jstride*jlevel + lstride*llevel;
    const double*   pJKData = data.data() + istride*ilevel + lstride*llevel;
    const double*  pIKLData = data.data() + jstride*jlevel;
    const double* pIJKLData = data.data();

    serializer_write.WriteField(  "ik", sp,   pIKData, ibstride, jbstride, kbstride, lbstride);
    serializer_write.WriteField(  "jk", sp,   pJKData, ibstride, jbstride, kbstride, lbstride);
    serializer_write.WriteField( "ikl", sp,  pIKLData, ibstride, jbstride, kbstride, lbstride);
    serializer_write.WriteField("ijkl", sp, pIJKLData, ibstride, jbstride, kbstride, lbstride);

    /* Now open serializer in read mode */
    Serializer serializer;
    ASSERT_NO_THROW(serializer.Init(".", "Umpeppo", SerializerOpenModeRead));

    OffsetTable const & offsettable = getOffsetTable(serializer);
    FieldsTable const & fieldstable = getFieldsTable(serializer);

    ASSERT_EQ(4, fieldstable.size());
    ASSERT_EQ(1, offsettable.size());
    ASSERT_EQ(0, offsettable.GetSavepointID(sp));
    ASSERT_TRUE(fieldstable.HasField(  "ik"));
    ASSERT_TRUE(fieldstable.HasField(  "jk"));
    ASSERT_TRUE(fieldstable.HasField( "ikl"));
    ASSERT_TRUE(fieldstable.HasField("ijkl"));

    const DataFieldInfo& info_ik   = fieldstable.Find(  "ik");
    const DataFieldInfo& info_jk   = fieldstable.Find(  "jk");
    const DataFieldInfo& info_ikl  = fieldstable.Find( "ikl");
    const DataFieldInfo& info_ijkl = fieldstable.Find("ijkl");

    ASSERT_EQ(std::string("ik"), info_ik.name());
    ASSERT_EQ(isize, info_ik.iSize());
    ASSERT_EQ(  1  , info_ik.jSize());
    ASSERT_EQ(ksize, info_ik.kSize());
    ASSERT_EQ(  1  , info_ik.jSize());

    ASSERT_EQ(std::string("jk"), info_jk.name());
    ASSERT_EQ(  1  , info_jk.iSize());
    ASSERT_EQ(jsize, info_jk.jSize());
    ASSERT_EQ(ksize, info_jk.kSize());
    ASSERT_EQ(  1  , info_jk.lSize());

    ASSERT_EQ(std::string("ikl"), info_ikl.name());
    ASSERT_EQ(isize, info_ikl.iSize());
    ASSERT_EQ(  1  , info_ikl.jSize());
    ASSERT_EQ(ksize, info_ikl.kSize());
    ASSERT_EQ(lsize, info_ikl.lSize());

    ASSERT_EQ(std::string("ijkl"), info_ijkl.name());
    ASSERT_EQ(isize, info_ijkl.iSize());
    ASSERT_EQ(jsize, info_ijkl.jSize());
    ASSERT_EQ(ksize, info_ijkl.kSize());
    ASSERT_EQ(lsize, info_ijkl.lSize());

    // Cleanup files
    std::remove("Umpeppo.json");
    std::remove("Umpeppo_ik.dat");
    std::remove("Umpeppo_jk.dat");
    std::remove("Umpeppo_ikl.dat");
    std::remove("Umpeppo_ijkl.dat");
}

/*
TEST_F(SerializerUnittest, DataFields)
{
    Serializer serializer;
    serializer.Init(".", "Data", SerializerOpenModeWrite);

    const int iSize = 12, jSize = 14, kSize = 16;

    IJKSize size; size.Init(iSize, jSize, kSize);
    KBoundary kboundary0; kboundary0.Init(0, 0);
    KBoundary kboundary1; kboundary1.Init(0, 1);

    IJKRealField fieldIJK;
    fieldIJK.Init("fieldIJK", size, kboundary0);

    IJKIntField fieldInt;
    fieldInt.Init("fieldInt", size, kboundary0);

    IKRealField fieldIK;
    fieldIK.Init("fieldIK", size, kboundary1);

    // Put some data
    for (int i = 0; i < iSize; ++i)
        for (int j = 0; j < jSize; ++j)
            for (int k = 0; k < kSize; ++k)
            {
                fieldIJK(i, j, k) = (Real)1.5*i*k + 2.*j + 1.25;
                fieldInt(i, j, k) = i + j + j*k - 21;
                fieldIK(i, j, k) = 2.75 * i - k;
            }

    // Serialize fields
    Savepoint sp;
    sp.Init("Trallallero");
    sp.AddMetainfo("Step", 0);
    serializer.WriteField("", fieldIJK, sp);
    serializer.WriteField("", fieldInt, sp);
    serializer.WriteField("", fieldIK, sp);

    // Cleanup
    std::remove("Data.json");
    std::remove("Data_fieldIJK.dat");
    std::remove("Data_fieldIK.dat");
}
*/
