#include "gtest/gtest.h"
#include "wrapper/SerializationWrapper.h"
#include "serializer/Serializer.h"

using namespace ser;

class FieldUnittest: public ::testing::Test {
protected:

    Serializer ser;

    void SetUp() {
        ser.Init("./", "FieldUnittest", SerializerOpenModeWrite);
    }

    void TearDown()
    {
        std::remove("FieldUnittest.json");
        std::remove("FieldUnittest_field1.dat");
        std::remove("FieldUnittest_field2.dat");
        std::remove("FieldUnittest_field3.dat");
        std::remove("FieldUnittest_field4.dat");
    }
};


TEST_F(FieldUnittest, Register)
{
    ASSERT_TRUE(ser.fieldnames().empty());
    ASSERT_EQ(0, fs_fields(&ser));

    // Register some fields with the C++ API
    ser.RegisterField("field1", "int"   , 4, 42,  1,  1, 12, 1, 1, 0, 0, 0, 0, 2, 2);
    ser.RegisterField("field2", "double", 8, 42, 28, 80,  1, 3, 3, 3, 3, 0, 1, 0, 0);

    // Register some fields with the C API
    fs_register_field(&ser, "field3", 6, "int"   , 3, 4, 42,  1, 80,  1, 3, 3, 0, 0, 0, 1, 0, 0);
    fs_register_field(&ser, "field4", 6, "double", 6, 8,  1, 28, 80, 12, 0, 0, 2, 2, 0, 0, 1, 1);

    // Check that fields exist
    ASSERT_TRUE(fs_field_exists(&ser, "field1", 6));
    ASSERT_TRUE(fs_field_exists(&ser, "field2", 6));
    ASSERT_TRUE(fs_field_exists(&ser, "field3", 6));
    ASSERT_TRUE(fs_field_exists(&ser, "field4", 6));

    ASSERT_EQ(4, ser.fieldnames().size());
    ASSERT_EQ(4, fs_fields(&ser));

    // Check fields
    const DataFieldInfo& field1 = ser.FindField("field1");
    ASSERT_EQ(std::string("field1"), field1.name());
    ASSERT_EQ(std::string("int"), field1.type());
    ASSERT_EQ(4, field1.bytesPerElement());
    ASSERT_EQ(2, field1.rank());
    ASSERT_EQ(42, field1.iSize());
    ASSERT_EQ( 1, field1.jSize());
    ASSERT_EQ( 1, field1.kSize());
    ASSERT_EQ(12, field1.lSize());
    ASSERT_EQ(1, field1.iMinusHaloSize());
    ASSERT_EQ(0, field1.jMinusHaloSize());
    ASSERT_EQ(0, field1.kMinusHaloSize());
    ASSERT_EQ(2, field1.lMinusHaloSize());
    ASSERT_EQ(1, field1.iPlusHaloSize());
    ASSERT_EQ(0, field1.jPlusHaloSize());
    ASSERT_EQ(0, field1.kPlusHaloSize());
    ASSERT_EQ(2, field1.lPlusHaloSize());

    const DataFieldInfo& field2 = ser.FindField("field2");
    ASSERT_EQ(std::string("field2"), field2.name());
    ASSERT_EQ(std::string("double"), field2.type());
    ASSERT_EQ(8, field2.bytesPerElement());
    ASSERT_EQ(3, field2.rank());
    ASSERT_EQ(42, field2.iSize());
    ASSERT_EQ(28, field2.jSize());
    ASSERT_EQ(80, field2.kSize());
    ASSERT_EQ(1, field2.lSize());
    ASSERT_EQ(3, field2.iMinusHaloSize());
    ASSERT_EQ(3, field2.jMinusHaloSize());
    ASSERT_EQ(0, field2.kMinusHaloSize());
    ASSERT_EQ(0, field2.lMinusHaloSize());
    ASSERT_EQ(3, field2.iPlusHaloSize());
    ASSERT_EQ(3, field2.jPlusHaloSize());
    ASSERT_EQ(1, field2.kPlusHaloSize());
    ASSERT_EQ(0, field2.lPlusHaloSize());

    const DataFieldInfo& field3 = ser.FindField("field3");
    ASSERT_EQ(std::string("field3"), field3.name());
    ASSERT_EQ(std::string("int"), field3.type());
    ASSERT_EQ(4, field3.bytesPerElement());
    ASSERT_EQ(2, field3.rank());
    ASSERT_EQ(42, field3.iSize());
    ASSERT_EQ( 1, field3.jSize());
    ASSERT_EQ(80, field3.kSize());
    ASSERT_EQ( 1, field3.lSize());
    ASSERT_EQ(3, field3.iMinusHaloSize());
    ASSERT_EQ(0, field3.jMinusHaloSize());
    ASSERT_EQ(0, field3.kMinusHaloSize());
    ASSERT_EQ(0, field3.lMinusHaloSize());
    ASSERT_EQ(3, field3.iPlusHaloSize());
    ASSERT_EQ(0, field3.jPlusHaloSize());
    ASSERT_EQ(1, field3.kPlusHaloSize());
    ASSERT_EQ(0, field3.lPlusHaloSize());

    const DataFieldInfo& field4 = ser.FindField("field4");
    ASSERT_EQ(std::string("field4"), field4.name());
    ASSERT_EQ(std::string("double"), field4.type());
    ASSERT_EQ(8, field4.bytesPerElement());
    ASSERT_EQ(3, field4.rank());
    ASSERT_EQ( 1, field4.iSize());
    ASSERT_EQ(28, field4.jSize());
    ASSERT_EQ(80, field4.kSize());
    ASSERT_EQ(12, field4.lSize());
    ASSERT_EQ(0, field4.iMinusHaloSize());
    ASSERT_EQ(2, field4.jMinusHaloSize());
    ASSERT_EQ(0, field4.kMinusHaloSize());
    ASSERT_EQ(1, field4.lMinusHaloSize());
    ASSERT_EQ(0, field4.iPlusHaloSize());
    ASSERT_EQ(2, field4.jPlusHaloSize());
    ASSERT_EQ(0, field4.kPlusHaloSize());
    ASSERT_EQ(1, field4.lPlusHaloSize());

    // Check with C API
    int bytesPerElement, iSize, jSize, kSize, lSize,
        iMinusHaloSize, iPlusHaloSize, jMinusHaloSize, jPlusHaloSize,
        kMinusHaloSize, kPlusHaloSize, lMinusHaloSize, lPlusHaloSize;

    fs_get_fieldinfo(&ser, "field1", 6,
        &bytesPerElement, &iSize, &jSize, &kSize, &lSize,
        &iMinusHaloSize, &iPlusHaloSize, &jMinusHaloSize, &jPlusHaloSize,
        &kMinusHaloSize, &kPlusHaloSize, &lMinusHaloSize, &lPlusHaloSize);
    ASSERT_EQ(4, bytesPerElement);
    ASSERT_EQ(42, iSize);
    ASSERT_EQ( 1, jSize);
    ASSERT_EQ( 1, kSize);
    ASSERT_EQ(12, lSize);
    ASSERT_EQ(1, iMinusHaloSize);
    ASSERT_EQ(0, jMinusHaloSize);
    ASSERT_EQ(0, kMinusHaloSize);
    ASSERT_EQ(2, lMinusHaloSize);
    ASSERT_EQ(1, iPlusHaloSize);
    ASSERT_EQ(0, jPlusHaloSize);
    ASSERT_EQ(0, kPlusHaloSize);
    ASSERT_EQ(2, lPlusHaloSize);

    fs_get_fieldinfo(&ser, "field2", 6,
        &bytesPerElement, &iSize, &jSize, &kSize, &lSize,
        &iMinusHaloSize, &iPlusHaloSize, &jMinusHaloSize, &jPlusHaloSize,
        &kMinusHaloSize, &kPlusHaloSize, &lMinusHaloSize, &lPlusHaloSize);
    ASSERT_EQ(8, bytesPerElement);
    ASSERT_EQ(42, iSize);
    ASSERT_EQ(28, jSize);
    ASSERT_EQ(80, kSize);
    ASSERT_EQ(1, lSize);
    ASSERT_EQ(3, iMinusHaloSize);
    ASSERT_EQ(3, jMinusHaloSize);
    ASSERT_EQ(0, kMinusHaloSize);
    ASSERT_EQ(0, lMinusHaloSize);
    ASSERT_EQ(3, iPlusHaloSize);
    ASSERT_EQ(3, jPlusHaloSize);
    ASSERT_EQ(1, kPlusHaloSize);
    ASSERT_EQ(0, lPlusHaloSize);

    fs_get_fieldinfo(&ser, "field3", 6,
        &bytesPerElement, &iSize, &jSize, &kSize, &lSize,
        &iMinusHaloSize, &iPlusHaloSize, &jMinusHaloSize, &jPlusHaloSize,
        &kMinusHaloSize, &kPlusHaloSize, &lMinusHaloSize, &lPlusHaloSize);
    ASSERT_EQ(4, bytesPerElement);
    ASSERT_EQ(42, iSize);
    ASSERT_EQ( 1, jSize);
    ASSERT_EQ(80, kSize);
    ASSERT_EQ( 1, lSize);
    ASSERT_EQ(3, iMinusHaloSize);
    ASSERT_EQ(0, jMinusHaloSize);
    ASSERT_EQ(0, kMinusHaloSize);
    ASSERT_EQ(0, lMinusHaloSize);
    ASSERT_EQ(3, iPlusHaloSize);
    ASSERT_EQ(0, jPlusHaloSize);
    ASSERT_EQ(1, kPlusHaloSize);
    ASSERT_EQ(0, lPlusHaloSize);

    fs_get_fieldinfo(&ser, "field4", 6,
        &bytesPerElement, &iSize, &jSize, &kSize, &lSize,
        &iMinusHaloSize, &iPlusHaloSize, &jMinusHaloSize, &jPlusHaloSize,
        &kMinusHaloSize, &kPlusHaloSize, &lMinusHaloSize, &lPlusHaloSize);
    ASSERT_EQ(8, bytesPerElement);
    ASSERT_EQ( 1, iSize);
    ASSERT_EQ(28, jSize);
    ASSERT_EQ(80, kSize);
    ASSERT_EQ(12, lSize);
    ASSERT_EQ(0, iMinusHaloSize);
    ASSERT_EQ(2, jMinusHaloSize);
    ASSERT_EQ(0, kMinusHaloSize);
    ASSERT_EQ(1, lMinusHaloSize);
    ASSERT_EQ(0, iPlusHaloSize);
    ASSERT_EQ(2, jPlusHaloSize);
    ASSERT_EQ(0, kPlusHaloSize);
    ASSERT_EQ(1, lPlusHaloSize);
}

TEST_F(FieldUnittest, FieldMetainfo)
{
    fs_register_field(&ser, "field1", 6, "int"   , 3, 4, 42,  1, 80,  1, 3, 3, 0, 0, 0, 1, 0, 0);
    fs_register_field(&ser, "field2", 6, "double", 6, 8,  1, 28, 80, 12, 0, 0, 2, 2, 0, 0, 1, 1);

    // Add some metainfo
    fs_add_field_metainfo_b(&ser, "field1", 6, "FirstField", 10, true);
    fs_add_field_metainfo_d(&ser, "field1", 6, "InitValue", 9, 10.75);
    fs_add_field_metainfo_i(&ser, "field1", 6, "Elements", 8, 42*80);

    fs_add_field_metainfo_b(&ser, "field2", 6, "FirstField", 10, false);
    fs_add_field_metainfo_s(&ser, "field2", 6, "AlternateName", 13, "density", 7);

    // Read metainfo
    ASSERT_EQ(3, ser.FindField("field1").metainfo().size());
    ASSERT_EQ(2, ser.FindField("field2").metainfo().size());

    ASSERT_EQ(true, ser.FindField("field1").metainfo().AsBool("FirstField"));
    ASSERT_EQ(10.75, ser.FindField("field1").metainfo().AsDouble("InitValue"));
    ASSERT_EQ(42*80, ser.FindField("field1").metainfo().AsInt("Elements"));
    ASSERT_EQ(false, ser.FindField("field2").metainfo().AsBool("FirstField"));
    ASSERT_EQ(std::string("density"), ser.FindField("field2").metainfo().AsString("AlternateName"));
}

