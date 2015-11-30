//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "OffsetTable.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include "SerializationException.h"
#include "libjson.h"
#include <cmath>

using namespace ser;
void OffsetTable::Cleanup()
{
    savepoints_.clear();
    entries_.clear();
    savepointIndex_.clear();
}

int OffsetTable::AddNewSavepoint(const Savepoint& savepoint, int sid)
{
    // Check that savepoint does not exist by looking at the index
    std::map<Savepoint, int>::const_iterator iter = savepointIndex_.find(savepoint);

    if (iter != savepointIndex_.end())
    {
        std::ostringstream msg;
        msg << "Error: savepoint " << savepoint.ToString() << " is already registered in table "
            << "at position " << iter->second << "\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    // Check requested ID
    if (sid >= 0 && static_cast<unsigned>(sid) != entries_.size())
    {
        std::ostringstream msg;
        msg << "Error: requested ID " << sid << " for savepoint " << savepoint.ToString() << ", but"
            << " it would be registered with ID " << entries_.size() << "\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    // Register savepoint
    sid = entries_.size();
    savepoints_.push_back(savepoint);
    entries_.push_back(OffsetTableEntry());
    savepointIndex_[savepoint] = sid;
    return sid;
}

void OffsetTable::AddFieldRecord(const Savepoint& savepoint, const std::string& fieldName,
                                 offset_t offset, const checksum_t& checksum)
{
    // Find savepoint ID
    std::map<Savepoint, int>::const_iterator iter = savepointIndex_.find(savepoint);
    if (iter == savepointIndex_.end())
    {
        std::ostringstream msg;
        msg << "Error: savepoint " << savepoint.ToString() << " is not registered in table\n"
            << "Saving the field " << fieldName << " at offset " << offset << " is not possible\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    AddFieldRecord(iter->second, fieldName, offset, checksum);
}

void OffsetTable::AddFieldRecord(int savepointID, const std::string& fieldName,
                                 offset_t offset, const checksum_t& checksum)
{
    // Check that ID is valid
    if (static_cast<unsigned>(savepointID) >= entries_.size())
    {
        std::ostringstream msg;
        msg << "Error: savepoint with ID " << savepointID << " is not registered in table\n"
            << "Saving the field " << fieldName << " at offset " << offset << " is not possible\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    // Find the right row of the table
    OffsetTableEntry& entry = entries_[savepointID];

    // Set the offset of the record
    // If the recod already exists, it is overwritten
    OffsetTableEntryValue value;
    value.offset = offset;
    value.checksum = checksum;
    entry[fieldName] = value;
}

int OffsetTable::GetSavepointID(const Savepoint& savepoint) const
{
    std::map<Savepoint, int>::const_iterator iter = savepointIndex_.find(savepoint);
    return (iter == savepointIndex_.end() ? -1 : iter->second);
}

int OffsetTable::GetOffset(const Savepoint& savepoint, const std::string& fieldName) const
{
    // Find savepoint ID
    std::map<Savepoint, int>::const_iterator iter = savepointIndex_.find(savepoint);
    if (iter == savepointIndex_.end())
    {
        std::ostringstream msg;
        msg << "Error: savepoint " << savepoint.ToString() << " is not registered in table\n"
            << "Retrieving the offset of the field " << fieldName <<  " is not possible\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    return GetOffset(iter->second, fieldName);
}

int OffsetTable::GetOffset(int savepointID, const std::string& fieldName) const
{
    // Check that ID is valid
    if (static_cast<unsigned>(savepointID) >= entries_.size())
    {
        std::ostringstream msg;
        msg << "Error: savepoint with ID " << savepointID << " is not registered in table\n"
            << "Retrieving the offset of the field " << fieldName <<  " is not possible\n";
        SerializationException exception;
        exception.Init(msg.str());
        throw exception;
    }

    // Check that field is saved at savepoint or return -1
    // If there is return offset
    const OffsetTableEntry& entry = entries_[savepointID];
    const_iterator iter = entry.find(fieldName);
    if (iter == entry.end())
        return -1;
    else
        return iter->second.offset;
}

int OffsetTable::AlreadySerialized(const std::string& fieldName, const std::string& checksum) const
{
    // Loop backwards
    for (std::vector<OffsetTableEntry>::const_reverse_iterator iter = entries_.rbegin(); iter != entries_.rend(); ++iter)
    {
        OffsetTableEntry::const_iterator entry = iter->find(fieldName);
        if (entry == iter->end())
            continue;
        if (entry->second.checksum == checksum)
            return entry->second.offset;
    }
    return -1;
}

std::string OffsetTable::ToString() const
{
    const int size = savepoints_.size();
    const int width = std::ceil(std::log10(static_cast<double>(size)));

    std::ostringstream ss;

    ss << "OffsetTable {\n";
    for (int i = 0; i < size; ++i)
    {
        // Print savepoint
        ss << "  " << std::setw(width) << i << ": " << savepoints_[i].ToString() << "  ";

        // Print offsets
        const OffsetTableEntry& entry = entries_[i];
        ss << "( ";
        for (const_iterator iter = entry.begin(); iter != entry.end(); ++iter)
            ss << iter->first << ":" << iter->second.offset << " ";
        ss << ")\n";
    }
    ss << "}\n";

    return ss.str();
}

std::vector<std::string> OffsetTable::FieldsAtSavepoint(const Savepoint& savepoint) const
{
    int spid = GetSavepointID(savepoint);
    if (spid < 0)
        return std::vector<std::string>();

    std::vector<std::string> ret;
    const OffsetTableEntry& entry = entries_.at(spid);
    for (OffsetTableEntry::const_iterator iter = entry.begin(); iter != entry.end(); ++iter)
        ret.push_back(iter->first);

    return ret;
}

JSONNode OffsetTable::TableToJSON() const
{
    JSONNode node(JSON_ARRAY);
    node.set_name("OffsetTable");
    for (unsigned savepointID = 0; savepointID < savepoints_.size(); ++savepointID)
    {
        JSONNode entrynode = savepoints_[savepointID].ToJSON(savepointID);
        JSONNode offsetsnode;
        offsetsnode.set_name("__offsets");

        const OffsetTableEntry& entry = entries_[savepointID];
        for (const_iterator iter = entry.begin(); iter != entry.end(); ++iter)
        {
            JSONNode valueNode(JSON_ARRAY);
            valueNode.set_name(iter->first);
            valueNode.push_back(JSONNode("", iter->second.offset));
            valueNode.push_back(JSONNode("", iter->second.checksum));
            offsetsnode.push_back(valueNode);
        }

        entrynode.push_back(offsetsnode);

        node.push_back(entrynode);
    }

    return node;
}

JSONNode OffsetTable::FieldToJSON(const std::string& fieldName) const
{
    JSONNode node(JSON_ARRAY);
    node.set_name("OffsetTable");
    for (unsigned savepointID = 0; savepointID < savepoints_.size(); ++savepointID)
    {
        const OffsetTableEntry& entry = entries_[savepointID];
        const_iterator iter = entry.find(fieldName);

        // If the field is not serialized, proceed to the next savepoint
        if (iter == entry.end())
            continue;

        JSONNode entrynode = savepoints_[savepointID].ToJSON(savepointID);
        entrynode.push_back(JSONNode("__offset", iter->second.offset));

        node.push_back(entrynode);
    }

    return node;
}

void OffsetTable::TableFromJSON(const JSONNode& node)
{
    Cleanup();

    for(JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
    {
        JSONNode savepointNode = *iter;

        // Find savepoint ID and remove it
        JSONNode::iterator iditer = savepointNode.find("__id");
        const int savepointID = iditer->as_int();
        savepointNode.erase(iditer);

        // Find offsets node and remove it
        JSONNode::iterator offsetsiter = savepointNode.find("Offsets");
        JSONNode offsetsnode = *offsetsiter;
        savepointNode.erase(offsetsiter);

        // Register savepoint
        Savepoint savepoint;
        savepoint.FromJSON(savepointNode);
        AddNewSavepoint(savepoint, savepointID);

        // Interpret offsets and add records
        for (JSONNode::const_iterator o_iter = offsetsnode.begin(); o_iter != offsetsnode.end(); ++o_iter)
        {
            int offset = (*o_iter)[0].as_int();
            const std::string checksum = (*o_iter)[1].as_string();
            AddFieldRecord(savepointID, o_iter->name(), offset, checksum);
        }
    }
}

// void OffsetTable::fieldFromJSON(const JSONNode& node)
// {
//     for(JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
//     {
//         JSONNode savepointNode = *iter;
//
//         // Find savepoint ID and remove it
//         JSONNode::iterator iditer = savepointNode.find("ID");
//         const int savepointID = iditer->as_int();
//         savepointNode.erase(iditer);
//
//         // Find offsets node and remove it
//         JSONNode::iterator offsetsiter = savepointNode.find("Offset");
//         const int offset = offsetsiter->as_int();
//         savepointNode.erase(offsetsiter);
//
//         // Get savepoint
//         Savepoint savepoint;
//         savepoint.FromJSON(savepointNode);
//
//         // Check if savepoint exists already
//         const_iterator siter = savepointIndex_.find(savepoint);
//
//         // TODO: continue implementation...
//         // But: How to register a savepoint?
//         // Possibility: allow non-consecutive IDs to be registerd, use void savepoints to
//         //              fill the voids and disallow explicitely savepoints with name == "".
//         //              When adding a savepoint, check that either the ID is free or that
//         //              the savepont owning the ID has name == "", in which case it can be
//         //              overwritten.
//     }
// }

