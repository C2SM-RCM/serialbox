#pragma once

#include "MetainfoSet.h"
#include "FieldsTable.h"
#include "OffsetTable.h"
#include "FileFormat.h"
#include "BinarySerializer.h"
#include "TypeName.h"
#include "boost/shared_ptr.hpp"
#include "KBoundary.h"
#include <string>
#include <fstream>

namespace ser {

    enum SerializerOpenMode
    {
        SerializerOpenModeRead,
        SerializerOpenModeWrite,
        SerializerOpenModeAppend
    };

    class DataFieldInfo;

    class Serializer
    {
        friend class SerializerUnittest;

    public:
        Serializer() { }
        virtual ~Serializer();

        /**
         * Enable serialization
         *
         * Serialization is enabled by default, but it can be disabled either
         * by setting the environmental variable STELLA_SERIALIZATION_DISABLE
         * to a positive value or by calling the funcion DisableSerialization,
         * With this function you enable the serialization independently of the
         * current environment.
         *
         * The serialization can be only globally enabled or disabled. There is
         * not way to enable or disable only a specific serializer.
         */
        static void EnableSerialization() { enabled_ = 1; }

        /**
         * Disable serialization
         *
         * Serialization is enabled by default, but the user can disabled it
         * either by setting the environmental variable
         * The serialization can be only globally enabled or disabled. There is
         * not way to enable or disable only a specific serializer.
         */
        static void DisableSerialization() { enabled_ = -1; }

        /**
         * Gives the open mode
         *
         * @return The mode is returned as value
         */
        SerializerOpenMode mode() const { return mode_; }

        /**
         * Gives the directory where the data are stored
         *
         * @return The directory is returned as copy
         */
        std::string directory() const { return pFileFormat_->directory(); }

        /**
         * Gives the prefix of the files
         *
         * @return The prefix is returned as copy
         */
        std::string prefix() const { return pFileFormat_->prefix(); }

        /**
         * Initializes the serializer
         *
         * This method initializes the serializer and prepares it to perform
         * input/output operations. The serializer is attached to a specific directory
         * (that must exist before the call) and to a specific prefix, with which all
         * files read and written will start. There are three modes to open a
         * serializer: read, write and append. Read will give a read-only access to
         * the serialized data; Write will erase all files of a previous run of a
         * serializer with same directory and prefix; Append wil import all
         * existing information and allow the user to add more data.
         *
         * @param directory The directory where the files will be stored (must
         *                  exist before the call)
         * @param prefix The prefix of the files
         * @param mode The read/write/append flag that set the access mode
         */
        void Init(const std::string& directory, const std::string& prefix, SerializerOpenMode mode);

        /**
         * Gives read-only access to the metainformation
         */
        const MetainfoSet& globalMetainfo() const { return globalMetainfo_; }

        /**
         * Adds a key-value metainformation pair to the serializer.
         *
         * @throw SerializationException The type of the value is not supported.
         */
        template<typename ValueType>
        void AddMetainfo(const std::string& key, ValueType value)
        {
            globalMetainfo_.AddMetainfo(key, value);
        }

        /**
         * Registers a field into the serializer
         *
         * It is possible to register a field multiple times, as long as all
         * properties of the field always match.
         *
         * @throw SerializationException A field with the same name but different
         *        properties is registered already
         *
         * @param name The name of the field
         * @param type The name of the data type (e.g. "double", "float")
         * @param bytesPerElement The size in bytes of a scalar value (e.g. 8 for doubles)
         * @param iSize The size of the first dimension
         * @param jSize The size of the second dimension
         * @param kSize The size of the third dimension
         * @param lsize The size of the fourth dimension
         * @param iMinusHalo The dimension of the halo in negative i-direction
         * @param iPlusHalo  The dimension of the halo in positive i-direction
         * @param jMinusHalo The dimension of the halo in negative j-direction
         * @param jPlusHalo  The dimension of the halo in positive j-direction
         * @param kMinusHalo The dimension of the halo in negative k-direction
         * @param kPlusHalo  The dimension of the halo in positive k-direction
         * @param lMinusHalo The dimension of the halo in negative l-direction
         * @param lPlusHalo  The dimension of the halo in positive l-direction
         *
         * @return The method returns true if the field is new,
         *         false if the field was already registered with matching information.
         */
        bool RegisterField(const std::string& name, std::string type, int bytesPerElement,
                           int iSize, int jSize, int kSize, int lSize,
                           int iMinusHalo, int iPlusHalo, int jMinusHalo, int jPlusHalo,
                           int kMinusHalo, int kPlusHalo, int lMinusHalo, int lPlusHalo
                           );

        /**
         * Gives access to field info.
         *
         * @throw SerializationException The field is not registered
         *
         * @return The info of the requested field is returned
         */
        const DataFieldInfo& FindField(const std::string& fieldname) const;

        /**
         * Adds a key-value metainformation pair to a field.
         *
         * @throw SerializationException The field is not found in table or
         *        the type is not supported.
         */
        template<typename ValueType>
        void AddFieldMetainfo(const std::string& fieldname, const std::string& key, ValueType value)
        {
            fieldsTable_.Find(fieldname).AddMetainfo(key, value);
        }

        /**
         * Initializes the field according to the information present in the serializer
         *
         * The provided field will be initialized with the provided name and with the
         * size and boundary present in the serializer.
         *
         * @throw SerializationException The field is not registered or the field type
         *        is incompatible with the ij boundary specified in the serializer.
         */
        template<typename TDataField>
        void InitializeField(const std::string& fieldname, TDataField& field,
                             bool hasStorageInI, bool hasStorageInJ) const;

        /**
         * Gives access to the list of savepoints
         *
         * @return A constant reference to the underlying savepoints vector is returned
         */
        const std::vector<Savepoint>& savepoints() const { return offsetTable_.savepoints(); }

        /**
         * Gives access to the registered fields
         *
         * This function fills and returns a vector with the names of the registered fields
         *
         * @return The vector with the names of the fields is returned
         */
        std::vector<std::string> fieldnames() const;

        /**
         * Gives all fields which are available at the given savepoint and return their names
         * in a vector. If the savepoint does not exist in the serializer returns
         * an empty vector and does not throw any exceptions.
         *
         * @return A vector with the names of the available fields is returned
         */
        std::vector<std::string> FieldsAtSavepoint(const Savepoint& savepoint) const;

        /**
         * Serializes data to file
         *
         * This method serializes the content of the given array as the field
         * with the given name at the provided savepoint.
         *
         * The method is virtual to allow for reimplementation or decoration.
         *
         * @throw SerializationException The field is not registered or it is already
         *        present at the given savepoint
         *
         * @param fieldName The name of the field
         * @param savepoint The savepoint at which the field will be saved
         * @param pData The pointer to the beginning of the data
         * @param iStride The offset in bytes from one point to the next in i direction
         * @param jStride The offset in bytes from one point to the next in j direction
         * @param kStride The offset in bytes from one point to the next in k direction
         * @param lStride The offset in bytes from one point to the next in l direction
         */
        virtual void WriteField(const std::string& fieldName, const Savepoint& savepoint,
                                const void* pData, int iStride, int jStride, int kStride, int lStride);

        /**
         * Serializes a data field
         *
         * This overload of the method works only with STELLA data types. This will
         * automatically register the field with the provided name and serialize the
         * field. If the provided name is empty the name of the data field will be used.
         *
         * CUDA fields will be synchronized to the host before being accessed. They
         * are \b not synchronized back to the device at the end.
         *
         * See the main WriteField method to see other cases of exception raising.
         *
         * @throw SerializationException The registration fails because of inconsistent
         *        registration of fields with same name and different properties or
         *        the field is already saved at the savepoint
         *
         * @param name The name of the field (if empty the name of the field object is used)
         * @param field The STELLA data type contianing the information
         * @param savepoint The savepoint at which the field will be registed.
         */
        template<typename TDataField>
        void WriteField(std::string name, const TDataField& field, const Savepoint& savepoint,
                        const int iStride, const int jStride, const int kStride);

        /**
         * Deserializes data from file
         *
         * This method reads the contents of the file corresponging to the given
         * field name to the given savepoint and places the data into the provided
         * array, which must already be allocated.
         *
         * The method is virtual to allow for reimplementation or decoration.
         *
         * @throw SerializationException The field is not registered in the serializer or
         *        it is not saved at the savepoint
         *
         * @param fieldName The name of the field
         * @param savepoint The savepoint at which the field will be read
         * @param pData The pointer to the beginning of the data
         * @param iStride The offset in bytes from one point to the next in i direction
         * @param jStride The offset in bytes from one point to the next in j direction
         * @param kStride The offset in bytes from one point to the next in k direction
         * @param lStride The offset in bytes from one point to the next in l direction
         * @param alsoPrevious Whether to search in previous savepoints that the given
         *                     until a valid entry is found
         */
        virtual void ReadField(const std::string& fieldName, const Savepoint& savepoint,
            void* pData, int iStride, int jStride, int kStride, int lStride, bool alsoPrevious = false) const;

        /**
         * Deserializes a data field
         *
         * This overload of the method works only with STELLA data types. If the provided
         * name is empty the name of the data field will be used. The field must be
         * initialized already with the correct size and boundaries. If these do not
         * match with the serialized field a SerializationException is raised.
         *
         * CUDA fields will be synchronized to the host before being accessed. They
         * are \b not synchronized back to the device at the end.
         *
         * See the main ReadField method to see cases of exception raising.
         *
         * @param name The name of the field (if empty the name of the field object is used)
         * @param field The STELLA data type where the data will be loaded
         * @param savepoint The savepoint at which the field will be registed.
         * @param alsoPrevious Whether to search in previous savepoints that the given
         *                     until a valid entry is found
         */
        template<typename TDataField>
        void ReadField(std::string name, TDataField& field, const Savepoint& savepoint,
            bool hasStorageInI, bool hasStorageInJ, const int iStride, const int jStride, const int kStride,
            bool alsoPrevious = false) const;

        /**
         * Procudes string representation of the serializer and its data structures,
         * useful for debugging.
         *
         * @return A string is returned
         */
        std::string ToString() const;

        OffsetTable const & offsetTable() const { return offsetTable_;}

        FieldsTable const & fieldsTable() const { return fieldsTable_;}

    private:
        SerializerOpenMode mode_;

        MetainfoSet globalMetainfo_;
        FieldsTable fieldsTable_;
        OffsetTable offsetTable_;

        boost::shared_ptr<FileFormat> pFileFormat_;

        BinarySerializer binarySerializer_;

        /**
         * This variable can take three values:
         *
         *  0: the variable is not yet initialized -> the serialization is enabled if
         *     the environmental variable STELLA_SERIALIZATION_DISABLE is not set to
         *     a positive value. The first serializer which is initialized has to set
         *     this value either to +1 or to -1 according to the environment
         * +1: the serialization is enabled, independently of the environment
         * -1: the serialization is disabled, independently of the environment
         *
         * The value is initialized to 0
         */
        static int enabled_;
    };

    //
    // IMPLEMENTATION OF TEMPLATE METHODS
    //
    template<typename TDataField>
    void Serializer::InitializeField(const std::string& fieldname, TDataField& field,
                                     bool hasStorageInI, bool hasStorageInJ) const
    {
        // Get info (will throw if does not exist)
        const DataFieldInfo& info = FindField(fieldname);

        const int ct_iminus = TDataField::StorageFormat::IJBoundary::IMinusOffset::value;
        const int ct_iplus = TDataField::StorageFormat::IJBoundary::IPlusOffset::value;
        const int ct_jminus = TDataField::StorageFormat::IJBoundary::JMinusOffset::value;
        const int ct_jplus = TDataField::StorageFormat::IJBoundary::JPlusOffset::value;

        const bool ok_i = (info.iMinusHaloSize() == (hasStorageInI ? -ct_iminus : 0)) && (info.iPlusHaloSize() == (hasStorageInI ? ct_iplus : 0));
        const bool ok_j = (info.jMinusHaloSize() == (hasStorageInJ ? -ct_jminus : 0)) && (info.jPlusHaloSize() == (hasStorageInJ ? ct_jplus : 0));

        // Check IJ boundary
        if (!(ok_i && ok_j))
        {
            // Throw exception
            std::ostringstream errorstr;
            errorstr << "Error: the requested field " << fieldname << " has a different ij-boundary than"
                     << " what the provided data field object supports.\n";
            SerializationException exception;
            exception.Init(errorstr.str());
            throw exception;
        }

        // Put together size and k boundary
        IJKSize calculationDomain = info.calculationDomain();
        KBoundary kBoundary;
        kBoundary.Init(info.kMinusHaloSize(), info.kPlusHaloSize());

        // Initialize field
        field.Init(fieldname, calculationDomain, kBoundary);
    }

    template<typename TDataField>
    void Serializer::WriteField(std::string name, const TDataField& field, const Savepoint& savepoint,
        const int iStride, const int jStride, const int kStride)
    {
        typedef typename TDataField::ValueType ValueType;

        if (name.empty())
            name = field.name();

        // Retrieve size, boundaries and strides
        const IJKSize& size = field.storage().allocatedSize();
        const IJKBoundary& boundary = field.boundary();
        const int bytesPerElement = sizeof(typename TDataField::ValueType);

        // Register field
        // This will check for consistency
        this->RegisterField(name, type_name<typename TDataField::ValueType>(), bytesPerElement,
                            size.iSize(), size.jSize(), size.kSize(), 1,
                            -boundary.iMinusOffset(), boundary.iPlusOffset(),
                            -boundary.jMinusOffset(), boundary.jPlusOffset(),
                            -boundary.kMinusOffset(), boundary.kPlusOffset(),
                            0, 0);

        this->WriteField(name, savepoint, field.storage().pStorageBase(), iStride, jStride, kStride, 0);
    }

    template<typename TDataField>
    void Serializer::ReadField(std::string name, TDataField& field, const Savepoint& savepoint,
        bool hasStorageInI, bool hasStorageInJ, const int iStride, const int jStride, const int kStride,
        bool alsoPrevious) const
    {
        typedef typename TDataField::ValueType ValueType;

        if (name.empty())
            name = field.name();

        // Get info of serialized field
        const DataFieldInfo& info = FindField(name);

        // Check size and boundaries
        const IJKSize& size = field.storage().allocatedSize();
        const IJKBoundary& boundary = field.boundary();

        if (    (info.iSize() != size.iSize())
                || (info.jSize() != size.jSize())
                || (info.kSize() != size.kSize())
                || (info.lSize() != 1           ))
        {
            // Throw exception
            std::ostringstream errorstr;
            errorstr << "Error: the requested field " << name << " has a different size than"
                     << " the provided data field.\n";
            errorstr << "Registerd as: " << info.iSize() << "x" << info.jSize() << "x" << info.kSize() << "x" << info.lSize() << "\n";
            errorstr << "Given       : " << size.iSize() << "x" << size.jSize() << "x" << size.kSize() << "x" << 1 << "\n";
            SerializationException exception;
            exception.Init(errorstr.str());
            throw exception;
        }

        // Boundaries are usually set to 3 even if there is storage in that direction
        // Correct this interesting yet totally nonsense behavior
        const int actual_iminus = hasStorageInI ? boundary.iMinusOffset() : 0;
        const int actual_jminus = hasStorageInJ ? boundary.jMinusOffset() : 0;
        const int actual_iplus = hasStorageInI ? boundary.iPlusOffset() : 0;
        const int actual_jplus = hasStorageInJ ? boundary.jPlusOffset() : 0;

        if (    (info.iMinusHaloSize() != -actual_iminus)
                || (info.jMinusHaloSize() != -actual_jminus)
                || (info.kMinusHaloSize() != -boundary.kMinusOffset())
                || (info.lMinusHaloSize() != -0                      )
                || (info.iPlusHaloSize() != actual_iplus)
                || (info.jPlusHaloSize() != actual_jplus)
                || (info.kPlusHaloSize() != boundary.kPlusOffset())
                || (info.lPlusHaloSize() != 0                     ))
        {
            // Throw exception
            std::ostringstream errorstr;
            errorstr << "Error: the requested field " << name << " has different boundaries than"
                     << " the provided data field.";
            SerializationException exception;
            exception.Init(errorstr.str());
            throw exception;
        }

        // Check data type
        if (info.type() != type_name<ValueType>())
        {
            // Throw exception
            std::ostringstream errorstr;
            errorstr << "Error: the requested field " << name << " has different type than"
                     << " the provided data field.";
            SerializationException exception;
            exception.Init(errorstr.str());
            throw exception;
        }

        // Get strides
        const int bytesPerElement = sizeof(ValueType);

        // This hack is used to retrieve the storage pointer for the host memory
        // in case of GPU fields
        // This will synchronize the host storage
        ValueType * data = &field(boundary.iMinusOffset(),
                                  boundary.jMinusOffset(),
                                  boundary.kMinusOffset());

        // Perform the read
        this->ReadField(name, savepoint, data, iStride, jStride, kStride, 0, alsoPrevious);
    }

} // namespace ser
