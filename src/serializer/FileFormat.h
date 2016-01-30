#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <string>
#include "OffsetTable.h"

namespace ser {

    class DataFieldInfo;
    class OffsetTable;
    class FieldsTable;
    class Savepoint;
    class MetainfoSet;

    class FileFormat
    {
    public:
        FileFormat() { }
        virtual ~FileFormat() { }

        void Init(const std::string& directory, const std::string& prefix,
                  MetainfoSet& globalMetainfo, FieldsTable& fieldsTable, OffsetTable& offsetTable);

        std::string directory() const { return directory_; }

        std::string prefix() const { return prefix_; }

        /**
        * The name of the format is written to the global metainformation.
        *
        * @return The name of the format is returned.
        */
        virtual std::string FormatName() const =0;

        /**
        * Imports and fills the tables at beginning. This is used in read mode
        */
        virtual void ImportTables() =0;

        /**
        * Writes the tables to disk. This is used in write mode
        */
        virtual void WriteTables(const std::string& fieldName) =0;

        /**
        * Deletes the data and metadata on disk. This is used at beginning in write-only mode
        */
        virtual void CleanTables() =0;

        /**
        * Opens a stream for appending data
        *
        * @return The offset from the beginning of the file is returned
        */
        virtual OffsetTable::offset_t OpenStreamAppend(std::ofstream& stream, const std::string& fieldName, const Savepoint& savepoint) const =0;

        /**
        * Opens a stream for reading data
        */
        virtual void OpenStreamRead(std::ifstream& stream, const std::string& fieldName, const Savepoint& savepoint) const =0;

    protected:
        std::string directory_;
        std::string prefix_;
        FieldsTable *pFieldsTable_;
        OffsetTable *pOffsetTable_;
        MetainfoSet *pGlobalMetainfo_;
    };

} // namespace ser
