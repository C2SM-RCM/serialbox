//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <iostream>
#include "gtest/gtest.h"
#include "serializer/Savepoint.h"
#include "serializer/OffsetTable.h"
#include "serializer/Checksum.h"
#include "libjson.h"

using namespace ser;

class OffsetTableUnittest : public ::testing::Test
{
public:

    std::vector<Savepoint> const& GetSavepoints(OffsetTable& table)
    {
        return table.savepoints();
    }
};

TEST_F(OffsetTableUnittest, Savepoint)
{
    Savepoint sp;
    sp.Init("FastWavesUnittest.Divergence-in");
    sp.AddMetainfo("LargeTimeStep", 1);
    sp.AddMetainfo("RKStageNumber", 2);
    sp.AddMetainfo("ldyn_bbc", false);
    sp.AddMetainfo("hd", .5);

    JSONNode node = sp.ToJSON();

    Savepoint sp2;
    sp2.FromJSON(node);
    ASSERT_EQ(sp, sp2);

    sp2.AddMetainfo("Other", 42);
    ASSERT_NE(sp, sp2);

    Savepoint sp3;
    sp3.Init("FastWavesUnittest.Divergence-in");
    sp3.AddMetainfo("LargeTimeStep", 1);
    sp3.AddMetainfo("RKStageNumber", 3);
    sp3.AddMetainfo("ldyn_bbc", false);
    sp3.AddMetainfo("hd", .5);
    ASSERT_NE(sp, sp3);

    Savepoint sp4;
    sp4.Init("Test");
    sp4.AddMetainfo("Step", 5);

    Savepoint sp5;
    sp5.Init("Test");
    sp5.AddMetainfo("Step", 6);

    ASSERT_NE(sp4, sp5);
}

TEST_F(OffsetTableUnittest, Checksum)
{
    // Assert that checksum computation is correct
    const double data[4] = { 3.14, 456.1, 25.3, 0.000012 };
    const std::string checksum = computeChecksum(data, 4*sizeof(double));
    const std::string checksum_ref = "39DA3F5AEE8A68A8B5083E9BCBB252069EFE223BFCA28AEE7CD3BC49F8379C7";
    ASSERT_EQ(checksum_ref, checksum);

    // Define some savepoints
    Savepoint sp0, sp1;
    sp0.Init("FastWavesUnittest.Divergence-in");
    sp0.AddMetainfo("LargeTimeStep", 1);
    sp0.AddMetainfo("RKStageNumber", 2);
    sp0.AddMetainfo("ldyn_bbc", false);
    sp1.Init("DycoreUnittest.DoStep-out");
    sp1.AddMetainfo("LargeTimeStep", 2);
    sp1.AddMetainfo("hd", .5);

    // Test that offset table recognizes checksums
    OffsetTable table;
    table.AddNewSavepoint(sp0, 0);
    table.AddNewSavepoint(sp1, 1);
    OffsetTable::offset_t offset;

    ASSERT_FALSE(table.AlreadySerialized("Field1", computeChecksum(data, 4), offset));
    ASSERT_NO_THROW(table.AddFieldRecord(0, "Field1",   0, computeChecksum(data, 4)));
    ASSERT_FALSE(table.AlreadySerialized("Field1", computeChecksum(data, 8), offset));
    ASSERT_NO_THROW(table.AddFieldRecord(1, "Field1", 100, computeChecksum(data, 8)));
    ASSERT_FALSE(table.AlreadySerialized("Field2", computeChecksum(data, 8), offset));
    ASSERT_NO_THROW(table.AddFieldRecord(0, "Field2", 200, computeChecksum(data, 8)));

    ASSERT_TRUE(table.AlreadySerialized("Field1", computeChecksum(data, 4), offset));
    ASSERT_EQ(0, offset);
    ASSERT_TRUE(table.AlreadySerialized("Field1", computeChecksum(data, 8), offset));
    ASSERT_EQ(100, offset);
    ASSERT_TRUE(table.AlreadySerialized("Field2", computeChecksum(data, 8), offset));
    ASSERT_EQ(200, offset);
    ASSERT_NO_THROW(table.AddFieldRecord(1, "Field2", 200, computeChecksum(data, 8)));

    ASSERT_EQ(  0, table.GetOffset(sp0, "Field1"));
    ASSERT_EQ(100, table.GetOffset(sp1, "Field1"));
    ASSERT_EQ(200, table.GetOffset(sp0, "Field2"));
    ASSERT_EQ(200, table.GetOffset(sp1, "Field2"));
}

TEST_F(OffsetTableUnittest, TableToJSON)
{
    Savepoint sp0, sp1;
    sp0.Init("FastWavesUnittest.Divergence-in");
    sp0.AddMetainfo("LargeTimeStep", 1);
    sp0.AddMetainfo("RKStageNumber", 2);
    sp0.AddMetainfo("ldyn_bbc", false);
    sp1.Init("DycoreUnittest.DoStep-out");
    sp1.AddMetainfo("LargeTimeStep", 2);
    sp1.AddMetainfo("hd", .5);

    // Just for the sake of getting some valid checksums
    float somedata[] = { 1.1f, 2.2f, 3.3f, 4.4f };

    // Fill table
    OffsetTable table;
    ASSERT_NO_THROW(table.AddNewSavepoint(sp0, 0));
    ASSERT_NO_THROW(table.AddNewSavepoint(sp1, 1));
    ASSERT_NO_THROW(table.AddFieldRecord(sp0, "Field1",   0, computeChecksum(somedata, 4)));
    ASSERT_NO_THROW(table.AddFieldRecord(  0, "Field2",   0, computeChecksum(somedata, 8)));
    ASSERT_NO_THROW(table.AddFieldRecord(  1, "Field1", 100, computeChecksum(somedata, 12)));
    ASSERT_NO_THROW(table.AddFieldRecord(sp1, "Field2", 100, computeChecksum(somedata, 16)));

    //Generate table JSON
    JSONNode tableNode = table.TableToJSON();
    ASSERT_EQ(JSON_ARRAY, tableNode.type());
    ASSERT_EQ(2, tableNode.size());

    // Check first savepoint
    ASSERT_EQ(std::string("FastWavesUnittest.Divergence-in"), tableNode[0]["__name"].as_string());
    ASSERT_EQ(0, tableNode[0]["__id"].as_int());
    ASSERT_EQ(1, tableNode[0]["LargeTimeStep"].as_int());
    ASSERT_EQ(2, tableNode[0]["RKStageNumber"].as_int());
    ASSERT_FALSE(tableNode[0]["ldyn_bbc"].as_bool());
    ASSERT_EQ((int)JSON_ARRAY, (int)tableNode[0]["__offsets"][0].type());
    ASSERT_EQ(std::string("Field1"), tableNode[0]["__offsets"][0].name());
    ASSERT_EQ(0, tableNode[0]["__offsets"][0][0].as_int());
    ASSERT_EQ(computeChecksum(somedata, 4), tableNode[0]["__offsets"][0][1].as_string());
    ASSERT_EQ(0, tableNode[0]["__offsets"][1][0].as_int());
    ASSERT_EQ(computeChecksum(somedata, 8), tableNode[0]["__offsets"][1][1].as_string());

    // Check second savepoint
    ASSERT_EQ(std::string("DycoreUnittest.DoStep-out"), tableNode[1]["__name"].as_string());
    ASSERT_EQ(1, tableNode[1]["__id"].as_int());
    ASSERT_EQ(2, tableNode[1]["LargeTimeStep"].as_int());
    ASSERT_EQ(0.5, tableNode[1]["hd"].as_float());
    ASSERT_EQ((int)JSON_ARRAY, (int)tableNode[1]["__offsets"][0].type());
    ASSERT_EQ(std::string("Field1"), tableNode[1]["__offsets"][0].name());
    ASSERT_EQ(100, tableNode[1]["__offsets"][0][0].as_int());
    ASSERT_EQ(computeChecksum(somedata, 12), tableNode[1]["__offsets"][0][1].as_string());
    ASSERT_EQ(100, tableNode[1]["__offsets"][1][0].as_int());
    ASSERT_EQ(computeChecksum(somedata, 16), tableNode[1]["__offsets"][1][1].as_string());

    // Interpret JSON for table
    OffsetTable table2;
    table2.TableFromJSON(tableNode);

    // Check savepoints
    std::vector<Savepoint> const & sp = GetSavepoints(table2);
    ASSERT_EQ(2, sp.size());
    ASSERT_EQ(sp0, sp[0]);
    ASSERT_EQ(sp1, sp[1]);
    ASSERT_EQ(0, table2.GetSavepointID(sp[0]));
    ASSERT_EQ(1, table2.GetSavepointID(sp[1]));

    // Check methods
    OffsetTable::offset_t offset;
    ASSERT_EQ(  0, table2.GetOffset(sp0, "Field1"));
    ASSERT_EQ(  0, table2.GetOffset(sp0, "Field2"));
    ASSERT_EQ(100, table2.GetOffset(sp1, "Field1"));
    ASSERT_EQ(100, table2.GetOffset(sp1, "Field2"));
    ASSERT_TRUE(table2.AlreadySerialized("Field1", computeChecksum(somedata, 4), offset));
    ASSERT_EQ(  0, offset);
    ASSERT_TRUE(table2.AlreadySerialized("Field1", computeChecksum(somedata, 12), offset));
    ASSERT_EQ(100, offset);
    ASSERT_TRUE(table2.AlreadySerialized("Field2", computeChecksum(somedata, 8), offset));
    ASSERT_EQ(  0, offset);
    ASSERT_TRUE(table2.AlreadySerialized("Field2", computeChecksum(somedata, 16), offset));
    ASSERT_EQ(100, offset);
    ASSERT_FALSE(table2.AlreadySerialized("Field1", computeChecksum(somedata, 8), offset));
    ASSERT_FALSE(table2.AlreadySerialized("Field2", computeChecksum(somedata, 4), offset));
}

