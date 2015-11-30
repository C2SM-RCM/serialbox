//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <iostream>
#include "gtest/gtest.h"
#include "serializer/MetainfoSet.h"
#include "libjson.h"

using namespace ser;

TEST(MetainfoUnittest, MetainfoSet)
{
    MetainfoSet set;
    set.AddMetainfo("m1", 1);
    set.AddMetainfo("m2", 1.1f);
    set.AddMetainfo("m3", 1.2);
    set.AddMetainfo("m4", "hello");

    // Extract values
    int i;
    float f;
    double d;
    std::string s;

    // Extract values
    ASSERT_NO_THROW(set.ExtractValue("m1", i));
    ASSERT_NO_THROW(set.ExtractValue("m2", f));
    ASSERT_NO_THROW(set.ExtractValue("m3", d));
    ASSERT_NO_THROW(set.ExtractValue("m4", s));

    // Check values
    ASSERT_EQ(1, i);
    ASSERT_EQ(1.1f, f);
    ASSERT_EQ(1.2, d);
    ASSERT_EQ(std::string("hello"), s);

    // Check exceptions
    ASSERT_THROW(set.ExtractValue("m1", f), SerializationException);
    ASSERT_THROW(set.ExtractValue("m2", d), SerializationException);
    ASSERT_THROW(set.ExtractValue("m3", i), SerializationException);
    ASSERT_THROW(set.ExtractValue("m4", i), SerializationException);

    // Check generated JSON
    JSONNode node;
    const std::vector<JSONNode> nodes = set.GenerateNodes();
    for (std::vector<JSONNode>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter)
        node.push_back(*iter);

    ASSERT_EQ(std::string("{\"m1\":1,\"m2\":1.1,\"m3\":1.2,\"m4\":\"hello\"}"), node.write());
}

TEST(MetainfoUnittest, JSONInput)
{
    MetainfoSet set;
    JSONNode node;

    node = JSONNode("b1", 5);
    ASSERT_NO_THROW(set.AddNode(node));

    node = JSONNode("b2", 5.6);
    ASSERT_NO_THROW(set.AddNode(node));

    node = JSONNode(JSON_NULL);
    ASSERT_THROW(set.AddNode(node), SerializationException);
}
