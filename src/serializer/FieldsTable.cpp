//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "FieldsTable.h"
#include "DataFieldInfo.h"
#include "SerializationException.h"
#include "libjson.h"
#include <sstream>

using namespace ser;
void FieldsTable::RegisterField(const DataFieldInfo& dataFieldInfo)
{
    const std::string name = dataFieldInfo.name();

    if (HasField(name))
    {
        // Field is already registered
        std::string msg = "Error: trying to register the field " + name
                        + ", which is already registered\n";
        SerializationException exception;
        exception.Init(msg);
        throw exception;
    }

    // Register field
    data_[name] = dataFieldInfo;
}

bool FieldsTable::HasField(const std::string& fieldName) const
{
    return data_.count(fieldName);
}

const DataFieldInfo& FieldsTable::Find(const std::string& fieldName) const
{
    const std::map<std::string, DataFieldInfo>::const_iterator iter = data_.find(fieldName);

    if (iter == data_.end())
    {
        // Field does not exist
        std::string msg = "Error: field " + fieldName + " is not registered\n";
        SerializationException exception;
        exception.Init(msg);
        throw exception;
    }

    return iter->second;
}

DataFieldInfo& FieldsTable::Find(const std::string& fieldName)
{
    const std::map<std::string, DataFieldInfo>::iterator iter = data_.find(fieldName);

    if (iter == data_.end())
    {
        // Field does not exist
        std::string msg = "Error: field " + fieldName + " is not registered\n";
        SerializationException exception;
        exception.Init(msg);
        throw exception;
    }

    return iter->second;
}

FieldsTable::const_iterator FieldsTable::begin() const
{
    return data_.begin();
}
FieldsTable::const_iterator FieldsTable::end() const
{
    return data_.end();
}


JSONNode FieldsTable::FieldToJSON(const std::string& fieldName) const
{
    const DataFieldInfo& info = Find(fieldName);
    return info.ToJSON();
}

JSONNode FieldsTable::TableToJSON() const
{
    int id = 0;
    JSONNode node(JSON_ARRAY);
    node.set_name("FieldsTable");
    for (std::map<std::string, DataFieldInfo>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
    {
        node.push_back(iter->second.ToJSON(id++));
    }
    return node;
}

std::string FieldsTable::FieldFromJSON(const JSONNode& node)
{
    DataFieldInfo info;
    info.FromJSON(node);
    RegisterField(info);
    return info.name();
}

void FieldsTable::TableFromJSON(const JSONNode& node)
{
    Cleanup();

    for (JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
    {
        DataFieldInfo info;
        info.FromJSON(*iter);
        RegisterField(info);
    }
}

std::string FieldsTable::ToString() const
{
    std::ostringstream ss;
    ss << "FieldsTable [\n";
    for (std::map<std::string, DataFieldInfo>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
    {
        ss << "    " << iter->second.ToString() << "\n";
    }
    ss << "]\n";
    return ss.str();
}

