#include "FileFormat.h"

using namespace ser;

void FileFormat::Init (const std::string& directory, const std::string& prefix,
                       MetainfoSet& globalMetainfo, FieldsTable& fieldsTable, OffsetTable& offsetTable)
{
    // Remove trailing /
    int size = directory.size();
    while (directory[size-1] == '/')
        --size;

    directory_ = directory.substr(0, size) + '/';
    prefix_ = prefix;
    pGlobalMetainfo_ = &globalMetainfo;
    pFieldsTable_ = &fieldsTable;
    pOffsetTable_ = &offsetTable;
}

