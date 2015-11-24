#include "libjson.h"
#include "FileFormat.h"

namespace ser {

    /**
     * Implementation of the FileFormat interface that describes
    * the following format.
    *
    * The serializer is represented by a database JSON file, which contains both
    * fields and offset table. This file organizes the data, which is contained
    * in external DAT binary files. For every field, one DAT file exists.
    */
    class CentralizedFileFormat : public FileFormat
    {
        friend class CentralizedFileFormatUnittest;

    public:
        CentralizedFileFormat() { }

        /**
        * Initializes the file format
        *
        * @param directory The directory where the files will be saved or loaded
        * @param prefix The string all file names in the serializer will start with
        * @param globalMetaInfo A reference to the globa metainfo set which will be read or filled
        * @param fieldsTable A reference to the fields table which will be read or filled
        * @param offsetTable A reference to the offset table which will be read or filled
        */
        void Init(const std::string& directory, const std::string& prefix,
                  MetainfoSet& globalMetainfo, FieldsTable& fieldsTable, OffsetTable& offsetTable);

        virtual std::string FormatName() const;

        virtual void ImportTables();

        virtual void WriteTables(const std::string& fieldName);

        virtual void CleanTables();

        virtual int OpenStreamAppend(std::ofstream& stream, const std::string& fieldName, const Savepoint& savepoint) const;

        virtual void OpenStreamRead(std::ifstream& stream, const std::string& fieldName, const Savepoint& savepoint) const;

        std::string databaseFile() const { return databaseFile_; }

    private:
        std::string databaseFile_;

        JSONNode GlobalInfoToJSON() const;
        void GlobalInfoFromJSON(const JSONNode& node);
    };

} // namespace ser
