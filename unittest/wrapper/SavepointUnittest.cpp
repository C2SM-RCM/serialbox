//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "gtest/gtest.h"
#include "serializer/Savepoint.h"
#include "wrapper/SerializationWrapper.h"

using namespace ser;

/**
 * In this test we test the basic C API
 */
TEST(SavepointUnittest, Basic)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(fs_create_savepoint("MySavepoint", 11));

    ASSERT_EQ("MySavepoint", sp.name());

    // Add some metainfo with the C++ API
    sp.AddMetainfo("StepNumber", 42);
    sp.AddMetainfo("TimeStepSize", 0.125);
    sp.AddMetainfo("SavepointAuthor", "Andrea Arteaga");

    // Add some metainfo with the C API
    fs_add_savepoint_metainfo_b(&sp, "ILikeThisAPI", 12, true);
    fs_add_savepoint_metainfo_d(&sp, "HowManyCookiesIEat", 18, 7.5);
    fs_add_savepoint_metainfo_s(&sp, "MyOS", 4, "GNU/Linux", 9);

    // Check metainfo
    ASSERT_EQ(6, sp.metainfo().size());
    ASSERT_EQ(42, sp.metainfo().AsInt("StepNumber"));
    ASSERT_EQ(0.125, sp.metainfo().AsDouble("TimeStepSize"));
    ASSERT_EQ(std::string("Andrea Arteaga"), sp.metainfo().AsString("SavepointAuthor"));
    ASSERT_EQ(true, sp.metainfo().AsBool("ILikeThisAPI"));
    ASSERT_EQ(7.5, sp.metainfo().AsDouble("HowManyCookiesIEat"));
    ASSERT_EQ(std::string("GNU/Linux"), sp.metainfo().AsString("MyOS"));

    fs_destroy_savepoint(&sp);
}

/**
 * In this test we test the metainfo retrieval C API
 */
TEST(SavepointUnittest, Metainfo)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(fs_create_savepoint("MySavepoint", 11));

    // Add some metainfo with the C++ API
    sp.AddMetainfo("StepNumber", 42);
    sp.AddMetainfo("TimeStepSize", 0.125);
    sp.AddMetainfo("SavepointAuthor", "Andrea Arteaga");

    // Add some metainfo with the C API
    fs_add_savepoint_metainfo_b(&sp, "YouLikeThisAPI", 14, true);
    fs_add_savepoint_metainfo_d(&sp, "HowManyCookiesIEat", 18, 7.5);
    fs_add_savepoint_metainfo_s(&sp, "MyOS", 4, "GNU/Linux", 9);

    // Get metainfo number
    ASSERT_EQ(6, fs_savepoint_metainfo_size(&sp));

    // Get keys of metainfo
    int key_length[6];
    fs_savepoint_key_lengths(&sp, key_length);
    ASSERT_EQ(18, key_length[0]);
    ASSERT_EQ( 4, key_length[1]);
    ASSERT_EQ(15, key_length[2]);
    ASSERT_EQ(10, key_length[3]);
    ASSERT_EQ(12, key_length[4]);
    ASSERT_EQ(14, key_length[5]);

    // Get keys
    std::string keys_str[6];
    char* keys[6];
    for(int i = 0; i < 6; ++i)
        keys[i] = new char[key_length[i]];

    fs_savepoint_get_keys(&sp, &keys[0]);
    for(int i = 0; i < 6; ++i)
    {
        keys_str[i].assign(keys[i], key_length[i]);
        delete[] keys[i];
    }
    ASSERT_EQ(std::string("HowManyCookiesIEat"), keys_str[0]);
    ASSERT_EQ(std::string("MyOS"), keys_str[1]);
    ASSERT_EQ(std::string("SavepointAuthor"), keys_str[2]);
    ASSERT_EQ(std::string("StepNumber"), keys_str[3]);
    ASSERT_EQ(std::string("TimeStepSize"), keys_str[4]);
    ASSERT_EQ(std::string("YouLikeThisAPI"), keys_str[5]);

    // Get type
    int types[6];
    fs_savepoint_get_types(&sp, types);
    ASSERT_EQ(-4, types[0]);
    ASSERT_EQ( 9, types[1]);
    ASSERT_EQ(14, types[2]);
    ASSERT_EQ(-2, types[3]);
    ASSERT_EQ(-4, types[4]);
    ASSERT_EQ(-1, types[5]);

    // Get values
    bool bvalue;
    int ivalue;
    double dvalue;
    std::string svalue;
    fs_get_savepoint_metainfo_d(&sp, "HowManyCookiesIEat", 18, &dvalue); ASSERT_EQ(7.5, dvalue);
    svalue.resize(9); fs_get_savepoint_metainfo_s(&sp, "MyOS", 4, &svalue[0]); ASSERT_EQ(std::string("GNU/Linux"), svalue);
    svalue.resize(14); fs_get_savepoint_metainfo_s(&sp, "SavepointAuthor", 15, &svalue[0]); ASSERT_EQ(std::string("Andrea Arteaga"), svalue);
    fs_get_savepoint_metainfo_i(&sp, "StepNumber", 10, &ivalue); ASSERT_EQ(42, ivalue);
    fs_get_savepoint_metainfo_d(&sp, "TimeStepSize", 12, &dvalue); ASSERT_EQ(0.125, dvalue);
    fs_get_savepoint_metainfo_b(&sp, "YouLikeThisAPI", 14, &bvalue); ASSERT_EQ(true, bvalue);

    fs_destroy_savepoint(&sp);

}

