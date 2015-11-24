#pragma once

#include <string>
#include "MetainfoSet.h"
#include "libjson.h"

namespace ser {

    class Savepoint
    {
    public:
        Savepoint() { }
        ~Savepoint() { }

        /**
         * Copy constructor
         *
         * After the construction the new savepoint object will represent the same
         * savepoint as the original one. There won't be any kind of link between
         * the original object and the new one: a deep copy is performed.
         *
         * @param other The original savepoint to copy from
         */
        Savepoint(const Savepoint& other)
        {
            *this = other;
        }

        /**
         * Assignment operator
         *
         * See copy constructor.
         */
        Savepoint& operator=(const Savepoint& other)
        {
            name_ = other.name_;
            metainfo_ = other.metainfo_;

            return *this;
        }

        /**
         * Initialize the savepoint
         *
         * This method prepares the savepoint for usage and gives a name, which is
         * the only required information for the savepoint to be usable.
         * Metainformation can be added after the initialization has been performed.
         *
         * A savepoint can be initialized multiple times. In this case, every
         * initialization removes all metainformation and sets a new name.
         *
         * @param name The name of the savepoint
         */
        void Init(const std::string& name);

        /**
         * Gives access to the name
         *
         * @return The name of the savepoint is returned
         */
        const std::string& name() const { return name_; }

        /**
         * Gives read-only access to the metainfo
         */
        const MetainfoSet& metainfo() const { return metainfo_; }

        /**
         * Add metainformation to the savepoint
         *
         * After this call a new key-value pair is registered as metainformation
         * in the savepoint. The order in which the metainformation is added
         * is irrelevant.
         *
         * Supported types are booleans, integers, floating points and string.
         *
         * @throw SerializationException The key exists already
         *
         * @param key The key of the new metainformation
         * @param value The value of the new metainformaiton
         */
        template<typename ValueType>
        void AddMetainfo(const std::string& key, const ValueType& value)
        {
            metainfo_.AddMetainfo(key, value);
        }

        /**
         * Gives the JSON representation of the savepoint
         *
         * @param id An ID information to attach. If not provided, it is not added.
         *
         * @return The JSON node containing the savepoint information is returned
         */
        JSONNode ToJSON(int id=-1) const;

        /**
         * Interprets the JSON node and sets the savepoint accordignly
         *
         * This method reads the provided JSON node and sets the savepoint's name
         * and metainformation according to the data provided. It is not necessary
         * to initialize the object before calling this routine. If the object is
         * already initiaized the name and metainformation will be discarded and
         * overwritten.
         *
         * The requirements for the JSON node are the following:
         *   - it contains exactly one child node named "name" with a string value
         *   - all other children are of one of the supported types
         *     (boolean, integer, floating point, string)
         *
         * @param node The JSON node that will be interpreted
         */
        void FromJSON(const JSONNode& node);

        /**
         * Gives a string representation of the savepoint. Useful for debugging.
         */
        std::string ToString() const;

        /**
         * Comparison operators
         *
         * These are used to allow the existance of sets or maps with savepoints
         */
        bool operator==(const Savepoint& other) const;
        bool operator!=(const Savepoint& other) const;
        bool operator<(const Savepoint& other) const;

    private:
        std::string name_;
        MetainfoSet metainfo_;
    };

    inline std::ostream& operator<<(std::ostream& out, const Savepoint& sp)
    {
        return out << sp.ToString();
    }

} // namespace ser
