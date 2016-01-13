#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <map>
#include <vector>
#include <string>
#include <cstdint>

#include "Savepoint.h"

namespace ser{

    /**
    * Holds information about records.
    *
    * Objects of this class represent a table, where every entry is associated
    * to a savepoint and contains a set of records. Records are pairs
    * fieldname -> offset. For every savepoint an arbitrary number of fields
    * with the respective offset can be registered. A field can be
    * registered just once at each savepoint.
    */
    class OffsetTable
    {
    public:
        typedef std::uint64_t offset_t;
        typedef std::string checksum_t;

    private:
        typedef struct { offset_t offset; checksum_t checksum; } OffsetTableEntryValue;
        typedef std::map<std::string, OffsetTableEntryValue> OffsetTableEntry;
        typedef OffsetTableEntry::const_iterator const_iterator;

    public:
        OffsetTable() { }
        ~OffsetTable() { }

        /**
     * Clears the table
     */
        void Cleanup();

        /**
        * Adds a new savepoint into the talbe with a given ID. The savepoint is
        * appended to the table and it is checked that the provided ID matches
        * the ID of the new savepoint. A negative value for the ID means
        * that there is no requested ID.
        *
        * @throw SerializationException The savepoint if already registered or the
        *        ID does not match the one requested (if such a request exists,
        *        i.e. if the privided ID is non-negative)
        *
        * @param savepoint The new savepoint
        * @param sid The requested savepoint ID, or a negative value (default)
        *            if none is requested
        *
        * @return The ID of the new savepoint is returned
        */
        int AddNewSavepoint(const Savepoint& savepoint, int sid=-1);

        /**
        * Add a new record, consisting of a savepoint, a field name, an offset
        * and a checksum of the binary data.
        *
        * @throw SerializationException The savepoint is not registered in the table
        *
        * If the field is already registered at the provided savepoint, the entry
        * is overwritten and no exception is thrown.
        *
        * @param savepoint The savepoint to which the entry is attached (it must already be registered) or its ID
        * @param fieldName The name of the registered field
        * @param offset The offset corresponding to the entry
        * @param checksum The checksum of the data
        */
        void AddFieldRecord(const Savepoint& savepoint, const std::string& fieldName, offset_t offset, const checksum_t& checksum);
        void AddFieldRecord(int savepointID, const std::string& fieldName, offset_t offset, const checksum_t& checksum);

        /**
        * Find savepoint
        *
        * @return If the savepoint is found, its ID is returned, otherwise a
        *         negative value is returned.
        */
        int GetSavepointID(const Savepoint& savepoint) const;

        /**
        * Get offset of record.
        *
        * If the field is not recorded at the given savepoint, a negative value
        * is returned, but no exception is thrown.
        *
        * @throw SerializationException The savepoint is not registered in the table
        *        or the the specified field is not registered at the given savepoint
        *
        * @param savepoint Guess what...
        * @param fieldName Idem
        *
        * @return The offset is returned if the record is found.
        */
        offset_t GetOffset(const Savepoint& savepoint, const std::string& fieldName) const;
        offset_t GetOffset(int savepointID, const std::string& fieldName) const;

        /**
        * Checks if an entry of the field with the same checksum exists. This can be used
        * to avoid storing to the disk if the same state of a field is already stored.
        * In case the record is found, the corresponding offset will be stored in the
        * given variable, otherwise its value is not modified.
        *
        * @param fieldName The name of the field
        * @param checksum The checksum of the content
        * @param[out] offset The offset of the found record, if any, is stored here
        *
        * @return True is returned iff a record is found
        */
        bool AlreadySerialized(const std::string& fieldName, const std::string& checksum, offset_t& offset) const;

        /**
        * Produces a string represetation of the table
        *
        * @return A string is returned
        */
        std::string ToString() const;

        /**
        * Get number of savepoits
        *
        * @return The size of the table is returned
        */
        int size() const { return savepoints_.size(); }

        /**
        * Gives access to the savepoints list
        *
        * @return A constant reference to the underlying savepoint list is returned
        */
        const std::vector<Savepoint>& savepoints() const { return savepoints_; }

        /**
        * Gives all fields which are available at the given savepoint and return their names
        * in a vector. If the savepoint does not exist in the table returns
        * an empty vector and does not throw any exceptions.
        *
        * @return A vector with the names of the available fields is returned
        */
        std::vector<std::string> FieldsAtSavepoint(const Savepoint& savepoint) const;

        /**
        * Generate JSON reprentation of the table for all fields
        *
        * This method produces a JSON node containing all entries in the table.
        *
        * @return A JSON node is returned
        */
        JSONNode TableToJSON() const;

        /**
        * Generate JSON representation of table for a single field
        *
        * This method produces a JSON node containing the entries in the table that
        * refer to a specific field
        *
        * @return A JSON node is returned
        */
        JSONNode FieldToJSON(const std::string& fieldName) const;

        /**
        * Interpret JSON representation of table
        *
        * This method erases the table and interprets the provided JSON node to construct
        * the table as it was generated. All fields with their metainformation are read
        * and imported.
        *
        * @param node The node containing the offset table
        */
        void TableFromJSON(const JSONNode& node);

        /**
        * Interpret table for a single field
        *
        * This method interprets the given JSON node and fills the table with the
        * information present. The content of the table before the call is not
        * erased. New savepoints will be registered if not already registered.
        *
        * Note: it is still unclear how to implement this, so the implementation
        * is skipped for now. It will be useful to have it in the future in case
        * other file formats will be implemented.
        * The problem is that savepoints must be added in order, but by
        * deserializing a single field at a time is possible/probable that the savepoints
        * will show up in wrong order.
        *
        * @param node The node containing the offset table
        *
        *  @note This method is not yet implemented because there is a problem with
        *       the concept. We impose allow the user to provide a savepoint ID
        *       only if the IDs appear in strict consecutive order. This might not
        *       happen in this case, since there might be leaps in the savepoints
        *       where each field is registered, and reading one field aat a time
        *       will result in non-consecutive order of reistratoin of savepoints.
        */
        //void FieldFromJSON(const JSONNode& node);

    private:

        // For each entry in the offset table there is corresponding savepoint
        std::vector<Savepoint> savepoints_;

        // This is the true offset table: it is a vector containing entries.
        // For each entry, there is a map "fieldname -> list of records"
        std::vector<OffsetTableEntry> entries_;

        // This is an index for speeding up access to the table:
        // given a savepoint, an entry in this index gives the
        // corresponding entries in the savepoints_ and entries_ vectors
        std::map<Savepoint, int> savepointIndex_;
    };

} // namespace ser
