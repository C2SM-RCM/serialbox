#include <iostream>
#include "serializer/FieldsTable.h"
#include "serializer/DataFieldInfo.h"
#include "libjson.h"
#include "gtest/gtest.h"

using namespace ser;

class FieldsTableUnittest : public ::testing::Test
{
protected:
    FieldsTable table;

    void TearDown()
    {
        table.Cleanup();
    }

    int getTableSize()
    {
        return table.size();
    }
};

TEST_F(FieldsTableUnittest, Serilalize)
{
    // Filling some info
    DataFieldInfo info1, info2;

    info1.Init(
            "Field1", "double", 8, 3,
            38, 24, 60, 1,
            3, 3, 3, 3, 0, 0, 0, 0);
    info1.AddMetainfo("ADV", true);

    info2.Init(
            "Field2", "double", 8, 3,
            38, 24, 61, 1,
            3, 3, 3, 3, 0, 1, 0, 0);
    info2.AddMetainfo("Init", 7.4);

    // Registering fields into table
    table.RegisterField(info1);
    table.RegisterField(info2);

    // Checking table
    ASSERT_EQ(2, getTableSize());
    ASSERT_TRUE(table.HasField("Field1"));
    ASSERT_TRUE(table.HasField("Field2"));
    ASSERT_FALSE(table.HasField("Field3"));

    ASSERT_TRUE(info1 == table.Find("Field1"));
    ASSERT_TRUE(info2 == table.Find("Field2"));

    JSONNode node = table.TableToJSON();

    ASSERT_EQ(node.name(), "FieldsTable");
    ASSERT_EQ(node.type(), JSON_ARRAY);
    ASSERT_EQ(node.size(), 2);

    // Check first field info
    ASSERT_EQ(node[0].name(), "DataFieldInfo");
    ASSERT_EQ(node[0]["__name"].as_string(), "Field1");
    ASSERT_EQ(node[0]["__id"].as_int(), 0);
    ASSERT_EQ(node[0]["__isize"].as_int(), 38);
    ASSERT_EQ(node[0]["__jsize"].as_int(), 24);
    ASSERT_EQ(node[0]["__ksize"].as_int(), 60);
    ASSERT_EQ(node[0]["__iminushalosize"].as_int(), 3);
    ASSERT_EQ(node[0]["__iplushalosize"].as_int(), 3);
    ASSERT_EQ(node[0]["__jminushalosize"].as_int(), 3);
    ASSERT_EQ(node[0]["__jplushalosize"].as_int(), 3);
    ASSERT_EQ(node[0]["__kminushalosize"].as_int(), 0);
    ASSERT_EQ(node[0]["__kplushalosize"].as_int(), 0);
    ASSERT_EQ(node[0]["ADV"].as_bool(), true);

    // Check second field info
    ASSERT_EQ(node[1].name(), "DataFieldInfo");
    ASSERT_EQ(node[1]["__name"].as_string(), "Field2");
    ASSERT_EQ(node[1]["__id"].as_int(), 1);
    ASSERT_EQ(node[1]["__isize"].as_int(), 38);
    ASSERT_EQ(node[1]["__jsize"].as_int(), 24);
    ASSERT_EQ(node[1]["__ksize"].as_int(), 61);
    ASSERT_EQ(node[1]["__iminushalosize"].as_int(), 3);
    ASSERT_EQ(node[1]["__iplushalosize"].as_int(), 3);
    ASSERT_EQ(node[1]["__jminushalosize"].as_int(), 3);
    ASSERT_EQ(node[1]["__jplushalosize"].as_int(), 3);
    ASSERT_EQ(node[1]["__kminushalosize"].as_int(), 0);
    ASSERT_EQ(node[1]["__kplushalosize"].as_int(), 1);
    ASSERT_EQ(node[1]["Init"].as_float(), 7.4);
}

TEST_F(FieldsTableUnittest, DeserilalizeField)
{
    const std::string field_txt =
        "{"
        "  \"__name\" : \"Field1\","
        "  \"__id\" : 0,"
        "  \"__elementtype\" : \"double\", "
        "  \"__bytesperelement\" : 8,"
        "  \"__rank\" : 3,"
        "  \"__isize\" : 38,"
        "  \"__jsize\" : 24,"
        "  \"__ksize\" : 60,"
        "  \"__iminushalosize\" : 3,"
        "  \"__iplushalosize\" : 3,"
        "  \"__jminushalosize\" : 3,"
        "  \"__jplushalosize\" : 3,"
        "  \"__kminushalosize\" : 0,"
        "  \"__kplushalosize\" : 0,"
        "  \"ADV\" : true"
        "}"
    ;

    JSONNode node = libjson::parse(field_txt);
    DataFieldInfo info1;
    info1.FromJSON(node);

    ASSERT_EQ("Field1", info1.name());
    ASSERT_EQ("double", info1.type());
    ASSERT_EQ(8, info1.bytesPerElement());
    ASSERT_EQ(3, info1.rank());
    ASSERT_EQ(38, info1.iSize());
    ASSERT_EQ(24, info1.jSize());
    ASSERT_EQ(60, info1.kSize());
    ASSERT_EQ(3, info1.iMinusHaloSize());
    ASSERT_EQ(3, info1.iPlusHaloSize());
    ASSERT_EQ(3, info1.jMinusHaloSize());
    ASSERT_EQ(3, info1.jPlusHaloSize());
    ASSERT_EQ(0, info1.kMinusHaloSize());
    ASSERT_EQ(0, info1.kPlusHaloSize());
    ASSERT_EQ(true, info1.metainfo().AsBool("ADV"));
}

TEST_F(FieldsTableUnittest, DeserilalizeTable)
{
    const std::string table_txt =
        "["
        "  {"
        "    \"__name\" : \"Field1\","
        "    \"__id\" : 0,"
        "    \"__elementtype\" : \"double\", "
        "    \"__bytesperelement\" : 8,"
        "    \"__rank\" : 3,"
        "    \"__isize\" : 38,"
        "    \"__jsize\" : 24,"
        "    \"__ksize\" : 60,"
        "    \"__iminushalosize\" : 3,"
        "    \"__iplushalosize\" : 3,"
        "    \"__jminushalosize\" : 3,"
        "    \"__jplushalosize\" : 3,"
        "    \"__kminushalosize\" : 0,"
        "    \"__kplushalosize\" : 0,"
        "    \"ADV\" : true"
        "  },"
        "  {"
        "    \"__name\" : \"Field2\","
        "    \"__id\" : 1,"
        "    \"__elementtype\" : \"int\", "
        "    \"__bytesperelement\" : 4,"
        "    \"__rank\" : 2,"
        "    \"__isize\" : 1,"
        "    \"__jsize\" : 24,"
        "    \"__ksize\" : 61,"
        "    \"__iminushalosize\" : 0,"
        "    \"__iplushalosize\" : 0,"
        "    \"__jminushalosize\" : 3,"
        "    \"__jplushalosize\" : 3,"
        "    \"__kminushalosize\" : 0,"
        "    \"__kplushalosize\" : 1,"
        "    \"Init\" : 7.25"
        "  }"
        "]"
    ;

    DataFieldInfo info;

    JSONNode node = libjson::parse(table_txt);
    table.TableFromJSON(node);
    ASSERT_EQ(2, getTableSize());

    // Check first field
    ASSERT_NO_THROW(info = table.Find("Field1"));
    ASSERT_EQ("Field1", info.name());
    ASSERT_EQ("double", info.type());
    ASSERT_EQ(8, info.bytesPerElement());
    ASSERT_EQ(3, info.rank());
    ASSERT_EQ(38, info.iSize());
    ASSERT_EQ(24, info.jSize());
    ASSERT_EQ(60, info.kSize());
    ASSERT_EQ(3, info.iMinusHaloSize());
    ASSERT_EQ(3, info.iPlusHaloSize());
    ASSERT_EQ(3, info.jMinusHaloSize());
    ASSERT_EQ(3, info.jPlusHaloSize());
    ASSERT_EQ(0, info.kMinusHaloSize());
    ASSERT_EQ(0, info.kPlusHaloSize());
    ASSERT_EQ(true, info.metainfo().AsBool("ADV"));

    // Check first field
    ASSERT_NO_THROW(info = table.Find("Field2"));
    ASSERT_EQ("Field2", info.name());
    ASSERT_EQ("int", info.type());
    ASSERT_EQ(4, info.bytesPerElement());
    ASSERT_EQ(2, info.rank());
    ASSERT_EQ(1, info.iSize());
    ASSERT_EQ(24, info.jSize());
    ASSERT_EQ(61, info.kSize());
    ASSERT_EQ(0, info.iMinusHaloSize());
    ASSERT_EQ(0, info.iPlusHaloSize());
    ASSERT_EQ(3, info.jMinusHaloSize());
    ASSERT_EQ(3, info.jPlusHaloSize());
    ASSERT_EQ(0, info.kMinusHaloSize());
    ASSERT_EQ(1, info.kPlusHaloSize());
    ASSERT_EQ(7.25, info.metainfo().AsReal("Init"));
}

