//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "Serializer.h"

#include "CentralizedFileFormat.h"
#include "SerializationException.h"
#include "Checksum.h"
#include "utils/Logger.h"
#include <sstream>
#include <cstdlib>

using namespace ser;

int Serializer::enabled_ = 0;

void Serializer::Init(const std::string& directory, const std::string& prefix, SerializerOpenMode mode)
{
    // Set the enabled_ flag to something different than 0
    if (enabled_ == 0)
    {
        const char* envvar = std::getenv("STELLA_SERIALIZATION_DISABLED");
        enabled_ = (envvar && std::atoi(envvar) > 0) ? -1 : 1;
    }

    mode_ = mode;

    // Initialize file type, as it will also fill the tables
    boost::shared_ptr<CentralizedFileFormat> fileFormat(new CentralizedFileFormat);
    fileFormat->Init(directory, prefix, globalMetainfo_, fieldsTable_, offsetTable_);
    pFileFormat_ = fileFormat;

    // Clean the tables if mode is write
    if (mode_ == SerializerOpenModeWrite)
        pFileFormat_->CleanTables();
    else
        pFileFormat_->ImportTables();

    // Initiaize binary serializer
    binarySerializer_.Init();

    // Register basic metainfo
    if (!globalMetainfo_.HasKey("__format"))
        globalMetainfo_.AddMetainfo("__format", pFileFormat_->FormatName());
    //if (!globalMetainfo_.HasKey("__created"))
    //    globalMetainfo_.AddMetainfo("__created", /* TODO : add date (timestamp) */);
}

Serializer::~Serializer()
{
}

bool Serializer::RegisterField(const std::string& name, std::string type, int bytesPerElement,
            int iSize, int jSize, int kSize, int lSize,
            int iMinusHalo, int iPlusHalo, int jMinusHalo, int jPlusHalo,
            int kMinusHalo, int kPlusHalo, int lMinusHalo, int lPlusHalo
        )
{
    // Check that sizes are >= 1
    if (iSize < 1 || jSize < 1 || kSize < 1 || lSize < 1)
    {
        std::ostringstream errorstr;
        errorstr << "Error: field " << name << " is registered with sizes: "
            << iSize << "x" << jSize << "x" << kSize << "x" << lSize << "\n"
            << "Sizes must be larger than 0 for all dimensions\n";
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }

    // Compute rank
    int rank = (iSize != 1 ? 1: 0) + (jSize != 1 ? 1: 0)
             + (kSize != 1 ? 1: 0) + (lSize != 1 ? 1: 0);

    // Create info object
    DataFieldInfo info;
    info.Init(name, type, bytesPerElement, rank,
              iSize, jSize, kSize, lSize,
              iMinusHalo, iPlusHalo, jMinusHalo, jPlusHalo,
              kMinusHalo, kPlusHalo, lMinusHalo, lPlusHalo);

    DataFieldInfo other;
    try
    {
        other = fieldsTable_.Find(name);
    }
    catch (SerializationException&)
    {
        // Field does not exist: register and exit
        fieldsTable_.RegisterField(info);
        return true;
    }

    // Field was alredy registered: check info
    if (   type != other.type()
        || bytesPerElement != other.bytesPerElement()
        || iSize != other.iSize()
        || jSize != other.jSize()
        || kSize != other.kSize()
        || lSize != other.lSize()
        || iMinusHalo != other.iMinusHaloSize()
        || iPlusHalo  != other.iPlusHaloSize()
        || jMinusHalo != other.jMinusHaloSize()
        || jPlusHalo  != other.jPlusHaloSize()
        || kMinusHalo != other.kMinusHaloSize()
        || kPlusHalo  != other.kPlusHaloSize()
        || lMinusHalo != other.lMinusHaloSize()
        || lPlusHalo  != other.lPlusHaloSize()
       )
    {
        std::ostringstream errorstr;
        errorstr << "Error: field " << name << " was already registered into "
            << "serializer with different information";
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }

    return false;
}

const DataFieldInfo& Serializer::FindField(const std::string& fieldname) const
{
    return fieldsTable_.Find(fieldname);
}

std::vector<std::string> Serializer::fieldnames() const
{
    std::vector<std::string> ret;
    ret.reserve(fieldsTable_.size());
    for (FieldsTable::const_iterator iter = fieldsTable_.begin(); iter != fieldsTable_.end(); ++iter)
        ret.push_back(iter->first);
    return ret;
}

std::vector<std::string> Serializer::FieldsAtSavepoint(const Savepoint& savepoint) const
{
    return offsetTable_.FieldsAtSavepoint(savepoint);
}

void Serializer::WriteField(const std::string& fieldName, const Savepoint& savepoint,
        const void* pData, int iStride, int jStride, int kStride, int lStride)
{
    // Don't write if serialization is disabeld
    if (enabled_ < 0) return;

    sout << "Writing field " << fieldName << " at savepoint " << savepoint.ToString()
         << " with data at " << pData << "\n";

    // Check open mode
    if (mode_ != SerializerOpenModeWrite && mode_ != SerializerOpenModeAppend)
    {
        // Throw exception
        std::ostringstream errorstr;
        errorstr << "Error: serializer not open in write mode, but write operation requested.";
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }

    // Retrieve field info
    // (It will throw an exception if the field is not yet registered)
    const DataFieldInfo& info = fieldsTable_.Find(fieldName);

    // Search savepoint or add a new one if not present
    int savepointID = offsetTable_.GetSavepointID(savepoint);
    if (savepointID < 0)
        savepointID = offsetTable_.AddNewSavepoint(savepoint);

    // Check if field is already saved at savepoint
    try
    {
        offsetTable_.GetOffset(savepointID, fieldName);

        // Field is already there: throw exception
        std::ostringstream errorstr;
        errorstr << "Error: field " << fieldName << " was already saved into "
            << "serializer at savepoint " << savepoint.ToString();
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }
    catch (SerializationException&)
    {
        // Field is not there (or savepoint does not exist)
        // No error, continue
    }

    // Obtain buffer and checksum
    std::vector<char> binaryData;
    const std::string checksum = binarySerializer_.WriteArray(pData, info.bytesPerElement(),
            info.iSize(), info.jSize(), info.kSize(), info.lSize(),
            iStride, jStride, kStride, lStride, binaryData);

    // Control in offset table if data must be stored
    int offset = offsetTable_.AlreadySerialized(fieldName, checksum);

    if (offset < 0)
    {
        // Open file stream
        std::ofstream fs;
        offset = pFileFormat_->OpenStreamAppend(fs, fieldName, savepoint);

        // Write data and close stream
        fs.write(binaryData.data(), binaryData.size());
        fs.close();
    }

    // Update tables
    offsetTable_.AddFieldRecord(savepointID, fieldName, offset, checksum);
    pFileFormat_->WriteTables(fieldName);
}

void Serializer::ReadField(const std::string& fieldName, const Savepoint& savepoint,
        void* pData, int iStride, int jStride, int kStride, int lStride, bool alsoPrevious) const
{
    // Don't read if serialization is disabeld
    if (enabled_ < 0) return;

    sout << "Requested field " << fieldName << " at savepoint " << savepoint.ToString()
         << " into data at " << pData << "\n";

    // Retrieve field info
    // (It will throw an exception is the field is not yet registered)
    const DataFieldInfo& info = fieldsTable_.Find(fieldName);

    // Search savepoint
    int savepointID = offsetTable_.GetSavepointID(savepoint);
    if (savepointID < 0)
    {
        // Throw exception
        std::ostringstream errorstr;
        errorstr << "Error: the requested savepoint " << savepoint.ToString()
            << " does not exist in the serializer.";
        SerializationException exception;
        exception.Init(errorstr.str());
        throw exception;
    }

    // Search in previous savepoints
    if (alsoPrevious)
    {
        OffsetTable::offset_t offset;
        bool found = false;
        while(!found)
        {
            try
            {
                offsetTable_.GetOffset(savepointID, fieldName);

                // Field is found
                found = true;
                break;
            }
            catch (SerializationException&)
            {
                // Field is not found: proceed with previous savepoint
            }

            if(--savepointID < 0)
                break;
        }

        if (savepointID < 0)
        {
            std::ostringstream errorstr;
            errorstr << "Error: field " << fieldName << " is never serialized "
                     << "at or before savepoint " << savepoint << "\n";
            SerializationException exception;
            exception.Init(errorstr.str());
            throw exception;
        }

        sout << "Field found at savepoint " << savepoint << "\n";
    }

    // Open file stream
    // (This will check if the field is saved at savepoint and throw otherwise)
    std::ifstream fs;
    pFileFormat_->OpenStreamRead(fs, fieldName, offsetTable_.savepoints()[savepointID]);

    // Read data into buffer
    std::vector<char> binaryData(info.dataSize());
    fs.read(binaryData.data(), binaryData.size());

    // Read data into array
    binarySerializer_.ReadArray(pData, info.bytesPerElement(),
            info.iSize(), info.jSize(), info.kSize(), info.lSize(),
            iStride, jStride, kStride, lStride, binaryData);
}

std::string Serializer::ToString() const
{
    std::ostringstream ss;
    ss << "Serializer\n"
       << "Directory: " << pFileFormat_->directory() << "\n"
       << "Prefix: " << pFileFormat_->prefix() << "\n"
       << "Globalmetainfo: " << globalMetainfo_.ToString() << "\n"
       << fieldsTable_.ToString()
       << offsetTable_.ToString()
       << "----------------------------------------------\n";
    return ss.str();
}

