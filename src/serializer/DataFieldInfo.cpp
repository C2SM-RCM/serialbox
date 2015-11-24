#include "DataFieldInfo.h"
#include "libjson.h"
#include <sstream>

using namespace ser;

bool DataFieldInfo::operator==(const DataFieldInfo& other) const
{
    // Compare the least expensive elements first
    return (
            bytesPerElement_ == other.bytesPerElement_ &&
            rank_ == other.rank_ &&

            iSize_ == other.iSize_ &&
            jSize_ == other.jSize_ &&
            kSize_ == other.kSize_ &&
            lSize_ == other.lSize_ &&

            iMinusHalo_ == other.iMinusHalo_ &&
            iPlusHalo_  == other.iPlusHalo_ &&
            jMinusHalo_ == other.jMinusHalo_ &&
            jPlusHalo_  == other.jPlusHalo_ &&
            kMinusHalo_ == other.kMinusHalo_ &&
            kPlusHalo_  == other.kPlusHalo_ &&
            lMinusHalo_ == other.lMinusHalo_ &&
            lPlusHalo_  == other.lPlusHalo_ &&

            name_ == other.name_ &&
            type_ == other.type_ &&
            metainfo_ == other.metainfo_
        );
}

std::string DataFieldInfo::ToString() const
{
    std::ostringstream strm;
    strm << name_ << " ("
         << iSize_ << "x" << jSize_ << "x" << kSize_ << "x" << lSize_
         << ") " << metainfo_.ToString();
    return strm.str();
}

JSONNode DataFieldInfo::ToJSON(int id) const
{
    JSONNode fieldNode;
    fieldNode.set_name("DataFieldInfo");

    // Basic information
    fieldNode.push_back(JSONNode("__name", name_));
    if (id >= 0)
        fieldNode.push_back(JSONNode("__id", id));

    fieldNode.push_back(JSONNode("__elementtype", type_));
    fieldNode.push_back(JSONNode("__bytesperelement", bytesPerElement_));
    fieldNode.push_back(JSONNode("__rank", rank_));

    // Size
    fieldNode.push_back(JSONNode("__isize", iSize_));
    fieldNode.push_back(JSONNode("__jsize", jSize_));
    fieldNode.push_back(JSONNode("__ksize", kSize_));
    if (lSize_ != 1) fieldNode.push_back(JSONNode("__lsize", lSize_));

    // Halo
    fieldNode.push_back(JSONNode("__iminushalosize", iMinusHalo_));
    fieldNode.push_back(JSONNode("__iplushalosize", iPlusHalo_));
    fieldNode.push_back(JSONNode("__jminushalosize", jMinusHalo_));
    fieldNode.push_back(JSONNode("__jplushalosize", jPlusHalo_));
    fieldNode.push_back(JSONNode("__kminushalosize", kMinusHalo_));
    fieldNode.push_back(JSONNode("__kplushalosize", kPlusHalo_));
    if (lSize_ != 1)
    {
        fieldNode.push_back(JSONNode("__lminushalosize", lMinusHalo_));
        fieldNode.push_back(JSONNode("__lplushalosize", lPlusHalo_));
    }

    // Encode the metadata associated with the field
    std::vector<JSONNode> metainfo = metainfo_.GenerateNodes();
    for (std::vector<JSONNode>::const_iterator iter = metainfo.begin(); iter != metainfo.end(); ++iter)
    {
        fieldNode.push_back(*iter);
    }

    return fieldNode;
}


void DataFieldInfo::FromJSON(const JSONNode& node)
{
    // Fill default fields
    name_ = "";
    type_ = "";
    bytesPerElement_ = 0;
    rank_ = 0;
    iSize_ = 1;
    jSize_ = 1;
    kSize_ = 1;
    lSize_ = 1;
    iMinusHalo_ = 0;
    jMinusHalo_ = 0;
    kMinusHalo_ = 0;
    lMinusHalo_ = 0;
    iPlusHalo_ = 0;
    jPlusHalo_ = 0;
    kPlusHalo_ = 0;
    lPlusHalo_ = 0;

    metainfo_.Cleanup();
    for (JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
    {
        // Standard information
             if (iter->name() == "__name") name_ = iter->as_string();
        else if (iter->name() == "__id") { /* Discard ID */ }
        else if (iter->name() == "__elementtype") type_ = iter->as_string();
        else if (iter->name() == "__bytesperelement") bytesPerElement_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__rank") rank_ = static_cast<int>(iter->as_int());

        // Sizes
        else if (iter->name() == "__isize") iSize_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__jsize") jSize_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__ksize") kSize_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__lsize") lSize_ = static_cast<int>(iter->as_int());

        // Halo sizes
        else if (iter->name() == "__iminushalosize") iMinusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__jminushalosize") jMinusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__kminushalosize") kMinusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__lminushalosize") lMinusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__iplushalosize" ) iPlusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__jplushalosize" ) jPlusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__kplushalosize" ) kPlusHalo_ = static_cast<int>(iter->as_int());
        else if (iter->name() == "__lplushalosize" ) lPlusHalo_ = static_cast<int>(iter->as_int());

        // Other
        else
        {
            metainfo_.AddNode(*iter);
        }
    }
}

