//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "CentralizedFileFormat.h"

#include "OffsetTable.h"
#include "FieldsTable.h"
#include "DataFieldInfo.h"
#include "MetainfoSet.h"
#include "libjson.h"
#include <fstream>
#include <iostream>
#include <exception>
#include <cstdio>

using namespace ser;

void CentralizedFileFormat::Init(const std::string& directory, const std::string& prefix,
                                 MetainfoSet& globalMetainfo, FieldsTable& fieldsTable, OffsetTable& offsetTable)
{
    FileFormat::Init(directory, prefix, globalMetainfo, fieldsTable, offsetTable);
    databaseFile_ = directory_ + prefix_ + ".json";
}

std::string CentralizedFileFormat::FormatName() const
{
    return "centralized";
}

void CentralizedFileFormat::ImportTables()
{
    // Open database file and get length
    std::ifstream fs(databaseFile_.c_str());
    std::string json_txt;
    fs.seekg(0, std::ios::end);
    int fileLength = fs.tellg();

    // If file length is 0, then do not interpret JSON and clean the tables
    if (fileLength <= 0)
    {
        fs.close();

        pGlobalMetainfo_->Cleanup();
        pFieldsTable_->Cleanup();
        pOffsetTable_->Cleanup();
        return;
    }

    // Read json
    json_txt.reserve(fileLength);
    fs.seekg(0, std::ios::beg);
    json_txt.assign((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    fs.close();

    // Interpret JSON
    try
    {
        JSONNode node = libjson::parse(json_txt);

        GlobalInfoFromJSON(node["GlobalMetainfo"]);
        pFieldsTable_->TableFromJSON(node["FieldsTable"]);
        pOffsetTable_->TableFromJSON(node["OffsetTable"]);
    }
    catch(std::exception&)
    {
        std::cerr << "Error: JSON code in file " << databaseFile_ << " is malformed.\n"
                  << "Unable to read data.\n";
        std::exit(1);
    }
}

void CentralizedFileFormat::WriteTables(const std::string&)
{
    // Discard fieldName, as both tables are written completely onto the file

    JSONNode globalInfoNode = GlobalInfoToJSON();
    JSONNode fieldsTableNode = pFieldsTable_->TableToJSON();
    JSONNode offsetTableNode = pOffsetTable_->TableToJSON();

    JSONNode mainNode;
    mainNode.push_back(globalInfoNode);
    mainNode.push_back(fieldsTableNode);
    mainNode.push_back(offsetTableNode);

    std::ofstream fs(databaseFile_.c_str());
    fs << mainNode.write_formatted() << "\n";
    fs.close();
}

void CentralizedFileFormat::CleanTables()
{
    // Import tables to get list of files
    ImportTables();

    // Remove data files
    for (FieldsTable::const_iterator iter = pFieldsTable_->begin(); iter != pFieldsTable_->end(); ++iter)
    {
        std::ostringstream fname;
        fname << directory_ << "/" << prefix_ << "_" << iter->second.name() << ".dat";
        std::remove(fname.str().c_str());
    }

    // Remove database file
    std::ostringstream fname;
    fname << directory_ << "/" << prefix_ << ".dat";
    std::remove(fname.str().c_str());

    // Clean tables
    pGlobalMetainfo_->Cleanup();
    pFieldsTable_->Cleanup();
    pOffsetTable_->Cleanup();
}

OffsetTable::offset_t CentralizedFileFormat::OpenStreamAppend(std::ofstream& stream, const std::string& fieldName, const Savepoint&) const
{
    // Discard savepoint because we are just appending data to the file

    std::ostringstream fname;
    fname << directory_ << "/" << prefix_ << "_" << fieldName << ".dat";

    stream.open(fname.str().c_str(), std::ios::app | std::ios::binary);

    return stream.tellp();
}

void CentralizedFileFormat::OpenStreamRead(std::ifstream& stream, const std::string& fieldName, const Savepoint& savepoint) const
{
    std::ostringstream fname;
    fname << directory_ << "/" << prefix_ << "_" << fieldName << ".dat";

    // Check if field is already saved at savepoint and get offset
    OffsetTable::offset_t offset;
    try {
        offset = pOffsetTable_->GetOffset(savepoint, fieldName);
    }
    catch (SerializationException)
    {
        // Throw more detailed exception
        std::ostringstream errorstr;
        errorstr << "Error: field " << fieldName << " is not saved into serializer "
            << "at savepoint " << savepoint.ToString();
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }

    stream.open(fname.str().c_str(), std::ios::binary);
    stream.seekg(offset);
}

JSONNode CentralizedFileFormat::GlobalInfoToJSON() const
{
    JSONNode node;
    node.set_name("GlobalMetainfo");

    const std::vector<JSONNode>& nodes = pGlobalMetainfo_->GenerateNodes();
    for (std::vector<JSONNode>::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter)
        node.push_back(*iter);

    return node;
}

void CentralizedFileFormat::GlobalInfoFromJSON(const JSONNode& node)
{
    for (JSONNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
            pGlobalMetainfo_->AddNode(*iter);
}

