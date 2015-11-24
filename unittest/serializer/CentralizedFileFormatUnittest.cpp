#include <iostream>
#include "gtest/gtest.h"
#include "serializer/CentralizedFileFormat.h"
#include "serializer/OffsetTable.h"
#include "serializer/FieldsTable.h"
#include "libjson.h"
#include "serializer/DataFieldInfo.h"

using namespace ser;

class CentralizedFileFormatUnittest : public ::testing::Test
{
protected:
    std::string databaseFile(const CentralizedFileFormat& ff)
    {
        return ff.databaseFile();
    }
};

TEST_F(CentralizedFileFormatUnittest, Write)
{
    MetainfoSet globalMetainfo;
    FieldsTable fieldsTable;
    OffsetTable offsetTable;
    CentralizedFileFormat format;
    format.Init(".", "Field", globalMetainfo, fieldsTable, offsetTable);

    ASSERT_EQ(std::string("./Field.json"), databaseFile(format));

    // Check that tables are void
    ASSERT_EQ(0, globalMetainfo.size());
    ASSERT_EQ(0, fieldsTable.size());
    ASSERT_EQ(0, offsetTable.size());
}

