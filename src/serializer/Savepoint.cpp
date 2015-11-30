//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "Savepoint.h"
#include "libjson.h"
#include "SerializationException.h"
#include <iostream>
#include <sstream>
#include <exception>

using namespace ser;

void Savepoint::Init(const std::string& name)
{
    metainfo_.Cleanup();
    name_ = name;
}

JSONNode Savepoint::ToJSON(int id) const
{
    // Generate basic node
    JSONNode node;
    node.push_back(JSONNode("__name", name_));
    if (id >= 0)
        node.push_back(JSONNode("__id", id));

    // Add metainfo to the node
    const std::vector<JSONNode>& mn = metainfo_.GenerateNodes();
    for (std::vector<JSONNode>::const_iterator iter = mn.begin(); iter != mn.end(); ++iter)
        node.push_back(*iter);

    return node;
}

void Savepoint::FromJSON(const JSONNode& node)
{
    // Name
    try
    {
        name_ = node.at("__name").as_string();
    }
    catch(std::out_of_range&)
    {
        SerializationException exception;
        std::ostringstream ss;
        ss << "Error: Savepoint node does not have a name:\n"
           << node.write_formatted() << "\n";
        exception.Init(ss.str());
        throw exception;
    }

    // Metainfo
    // Nodes whose name starts with __ are skipped
    metainfo_.Cleanup();
    for (JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
        if (!(iter->name()[0] == '_' && iter->name()[1] == '_'))
            metainfo_.AddNode(*iter);
}

std::string Savepoint::ToString() const
{
    std::ostringstream ss;
    ss << name_ << metainfo_.ToString();
    return ss.str();
}

bool Savepoint::operator==(const Savepoint& other) const
{
    return name_ == other.name_ && metainfo_ == other.metainfo_;
}

bool Savepoint::operator!=(const Savepoint& other) const
{
    return !(*this == other);
}

bool Savepoint::operator<(const Savepoint& other) const
{
    if (name_ != other.name_)
        return name_ < other.name_;

    return metainfo_ < other.metainfo_;
}

