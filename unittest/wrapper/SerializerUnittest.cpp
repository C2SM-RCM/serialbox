#include "gtest/gtest.h"
#include "serializer/Serializer.h"
#include "wrapper/SerializationWrapper.h"
#include <string>

using namespace ser;

TEST(SerializerUnittest, Initialization)
{
    // Create serializer
    std::string directory = "./";
    std::string prefix = "myprefix";
    Serializer& ser = *reinterpret_cast<Serializer*>(fs_create_serializer(
                directory.data(), directory.size(),
                prefix.data(), prefix.size(), 'w'));

    ASSERT_EQ(directory, ser.directory());
    ASSERT_EQ(prefix, ser.prefix());
    ASSERT_EQ(SerializerOpenModeWrite, ser.mode());

    ASSERT_EQ('w', fs_serializer_openmode(&ser));

    fs_destroy_serializer(&ser);
}

TEST(SerializerUnittest, Metainfo)
{
    Serializer ser;
    ser.Init("./", "SerializerUnittestMetainfo", SerializerOpenModeWrite);

    // Add some metainfo with C++ API
    ser.AddMetainfo("Day", 31);
    ser.AddMetainfo("Month", "March");
    ser.AddMetainfo("Year", 2015.25);

    // Add some metainfo with C API
    fs_add_serializer_metainfo_b(&ser, "IAmHappy", 8, true);
    fs_add_serializer_metainfo_i(&ser, "Answer", 6, 42);
    fs_add_serializer_metainfo_s(&ser, "MyFullName", 10, "Andrea Arteaga", 14);

    // Count metainfo not starting with double underscores
    std::vector<std::string> keys = ser.globalMetainfo().keys();
    int count = 0;
    for (std::vector<std::string>::const_iterator iter = keys.begin(); iter != keys.end(); ++iter)
    {
        if (iter->size() < 2 || iter->at(0) != '_' || iter->at(1) != '_')
            ++count;
    }
    ASSERT_EQ(6, count);

    // Check that metainfo is consistent
    ASSERT_EQ(31, ser.globalMetainfo().AsInt("Day"));
    ASSERT_EQ(std::string("March"), ser.globalMetainfo().AsString("Month"));
    ASSERT_EQ(2015.25, ser.globalMetainfo().AsDouble("Year"));
    ASSERT_TRUE(ser.globalMetainfo().AsBool("IAmHappy"));
    ASSERT_EQ(42, ser.globalMetainfo().AsInt("Answer"));
    ASSERT_EQ(std::string("Andrea Arteaga"), ser.globalMetainfo().AsString("MyFullName"));
}

