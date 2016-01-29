#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <map>
#include <string>
#include "DataFieldInfo.h"

class JSONNode;

namespace ser {

    class FieldsTable
    {
        friend class FieldsTableUnittest;

    public:
        typedef std::map<std::string, DataFieldInfo>::const_iterator const_iterator;

        FieldsTable() { }
        ~FieldsTable() { }

        /**
        * Cleans up the table
        *
        * This method removes all registered fields from the table.
        */
        void Cleanup() { data_.clear(); }

        /**
        * Register a new field
        *
        * @throw SerializationException The field exists already
        *
        * @param dataFieldInfo The container of the field information
        */
        void RegisterField(const DataFieldInfo& dataFieldInfo);

        /**
        * @return True is returned iff the field is registered in the table
        */
        bool HasField(const std::string& fieldName) const;

        /**
        * This method returns the DataFieldInfo associated with a field name
        *
        * @throw SerializationException The field has not been registered yet
        *
        * @param fieldName The name of the field whose information is requested
        *
        * @return The associated DataFieldInfo is returned if found
        */
        const DataFieldInfo& Find(const std::string& fieldName) const;

        /**
        * This method returns the DataFieldInfo associated with a field name.
        *
        * @throw SerializationException The field has not been registered yet
        *
        * @param fieldName The name of the field whose information is requested
        *
        * @return The associated DataFieldInfo is returned if found
        */
        DataFieldInfo& Find(const std::string& fieldName);

        /**
        * Allow STL-style iteration
        */
        const_iterator begin() const;
        const_iterator end() const;

        /**
        * Generate JSON description of single field
        *
        * This method creates the JSON node corresponding to a field registered
        * in the table. The generated JSON contains the data field information like
        * name, data type and size, and the metainformation attached to the field.
        *
        * @throw SerializationException The field has not been registered yet
        *
        * @param fieldName The name of the field
        *
        * @return The JSON node corresponding to the field is returned
        */
        JSONNode FieldToJSON(const std::string& fieldName) const;

        /**
        * Generate JSON description of whole table
        *
        * This method generates the JSON node containing data field information
        * for all fields registered in the table
        *
        * @return The JSON node is returned
        */
        JSONNode TableToJSON() const;

        /**
        * Interprets JSON information for a single field
        *
        * This method reads a JSON node, generates a corresponding information
        * container and registers it into the table. The metainformation is
        * imported as well.
        *
        * @throw SerializationException A field with the same name is already registered
        *
        * @param node The JSON node containing the information about the field
        *
        * @return The name of the imported field is returned
        */
        std::string FieldFromJSON(const JSONNode& node);

        /**
        * Interprets JSON information for the whole table
        *
        * This method reads a JSON node, imports all information attached to the
        * fields and registers them. This will discard all data registered in the
        * table before the call.
        *
        * @param node The JSON array node containing the table
        */
        void TableFromJSON(const JSONNode& node);

        /**
        * Gives a string representation of the table
        *
        * @return A string is returned
        */
        std::string ToString() const;

        /**
        * Give number of fields
        *
        * @return size as integer
        */
        int size() const { return data_.size(); }

    private:
        std::map<std::string, DataFieldInfo> data_;
    };

} //namespace ser

