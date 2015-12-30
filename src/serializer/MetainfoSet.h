#pragma once

//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "boost/static_assert.hpp"
#include "boost/any.hpp"
#include "boost/type_traits/is_same.hpp"

#include "Definitions.h"
#include "SerializationException.h"
#include "libjson.h"


namespace ser {

    /**
    * Objects of this class contain a set of metainformation in
    * form of key = value pair. The keys are strings, while the
    * values can be integers, booleans, floating point numbers
    * (either single or double precision) or strings.
    */
    class MetainfoSet
    {
    public:
        /**
        * Default constructor
        */
        MetainfoSet() { }

        /**
        * Copy constructor
        */
        MetainfoSet(const MetainfoSet& other) { *this = other; }

        /**
        * Assignment operator
        *
        * Discards all previously stored information and copies data from the other
        * object, performing a deep copy.
        *
        * @param other The object to copy from
        */
        MetainfoSet& operator=(const MetainfoSet& other);

        /**
        * Removes all metainfo and frees all memory
        */
        void Cleanup();

        /**
        * Gives access to all existing keys
        *
        * The order in which the keys are returned is ensured not to change
        * if no keys are added or removed.
        *
        * @return A vector containing all keys is returned
        */
        std::vector<std::string> keys() const;

        /**
        * This function is a mean to retrieve the types of the metainfo saved in the set.
        * For every metainfo saved, a number is generated. If the number is positive, the
        * corresponding metainfo is a string and the number represents the length of it.
        * The value -1 corresponds to boolean, -2 to integer, -3 to single precision and
        * -4 to double precision floating point.
        *
        *  The generated list is ensured to be in the same order as the list returned
        *  by the function keys.
        *
        *  @return A vector of integer representing the types is returned
        */
        std::vector<int> types() const;

        /**
        * Check if key exists already in set
        *
        * @return True is returned iff the key does not exist
        */
        inline bool HasKey(const std::string& key) const
        {
            return data_.count(key);
        }

        /**
        * Add a new key-value pair into the set. The key must not exist yet.
        * The supported types are int, bool, float,
        * double and std::string; a compile-time error is generated if the
        * given type is not supported.
        *
        * @throw SerializationException The key exists already
        *
        * @param key The key of the pair
        * @param value The value, whose type must be one of the supported types
        */
        template<typename ValueType>
        inline void AddMetainfo(const std::string& key, const ValueType& value)
        {
            // Check that type of meta info is supported
            BOOST_STATIC_ASSERT((   boost::is_same<ValueType, int        >::type::value
                                    || boost::is_same<ValueType, bool       >::type::value
                                    || boost::is_same<ValueType, float      >::type::value
                                    || boost::is_same<ValueType, double     >::type::value
                                    || boost::is_same<ValueType, std::string>::type::value
                                    ));

            // Check that key does not exist yet
            if(HasKey(key))
            {
                std::string msg = "Error: metainfo with key = " + key + " exists already\n";
                SerializationException exception;
                exception.Init(msg);
                throw exception;
            }

            // Set key = value
            data_[key] = value;
        }

        /**
        * Special case for handling C strings
        */
        inline void AddMetainfo(const std::string& key, const char* pStr)
        {
            AddMetainfo(key, std::string(pStr));
        }

        /**
        * This method allows to retrieve the value of a metainformation by key.
        * The type of the given variable must be the same type of the metainformation;
        * providing the wrong type results in a SerializationException to be thrown.
        * In case of success, the given reference is assigned the value of the metainformation.
        * In case of error, the content of the variable is undefined.
        *
        * @throw SerializationException The key does not exist of the type of the
        *        passed value does not match that of the metainformation
        *
        * @param key The key of the metainformation to retrieve
        * @param[Out] value The reference where the value of the metainformation
        *                   will be placed
        */
        template<typename ValueType>
        inline void ExtractValue(const std::string& key, ValueType& value) const
        {
            std::map<std::string, boost::any>::const_iterator iter = data_.find(key);

            // Check that type exists
            if (iter == data_.end())
            {
                std::string msg = "Error: metainfo with key = " + key + " does not exist in set\n";
                SerializationException exception;
                exception.Init(msg);
                throw exception;
            }

            // Check type
            try
            {
                const boost::any& anyvalue = iter->second;
                value = boost::any_cast<ValueType>(anyvalue);
            }
            catch(boost::bad_any_cast&)
            {
                std::ostringstream msg;
                msg << "Error: type mismatch with metainfo " << key << "\n";
                msg << "Stored metainfo has type " << iter->second.type().name() << "\n";
                msg << "Provided reference has type " << typeid(ValueType).name() << "\n";
                SerializationException exception;
                exception.Init(msg.str());
                throw exception;
            }
        }

        /**
        * Gives access to the internal representation of the
        * requested metainfo
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The metainfo object is returned as constant reference
        */
        const boost::any& AsAny(const std::string& key) const;

        /**
        * Extracts a value in bool representation.
        *
        * If the type is different than bool, the function does its best to
        * convert the value to bool in a meaningful way.
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        bool AsBool(const std::string& key) const;

        /**
        * Extracts a value in int representation.
        *
        * If the type is different than int, the function does its best to
        * convert the value to int in ameaningful way, and throws a
        * SerializationException if the value is not convertible to int, e.g.,
        * in case of a floating point holding a non-integer value.
        *
        * @throw SerializationException The key does not exist or the value is
        *        not convertible to integer
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        int AsInt(const std::string& key) const;

        /**
        * Extracts a value in single precision floating point representation.
        *
        * If the type is different than float, the function converts the
        * value to float.
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        float AsFloat(const std::string& key) const;

        /**
        * Extracts a value in double precision floating point representation.
        *
        * If the type is different than double, the function converts the
        * value to double.
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        float AsDouble(const std::string& key) const;

        /**
        * Extracts a value in RealType representation, where RealType is
        * either `float` or `double`.
        *
        * If the type is different than RealType, the function converts the
        * value to RealType.
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        template<typename RealType>
        RealType AsReal(const std::string& key) const
        {
            BOOST_STATIC_ASSERT(boost::is_same<RealType, float>::value
                             || boost::is_same<RealType, double>::value);

            if (boost::is_same<RealType, float>::value) return AsFloat(key);
            else if (boost::is_same<RealType, double>::value) return AsDouble(key);
        }

        /**
        * Extracts a value assuming its type is string.
        *
        * If the type is different than string, the function constructs
        * a string representation of it.
        *
        * @throw SerializationException The key does not exist
        *
        * @param key The identification of the metainfo value which is requested
        *
        * @return The value of the metainfo is returned as copy
        */
        std::string AsString(const std::string& key) const;

        /**
        * Creates a string that carries the metainfo in a human-readable form
        *
        * @return The string representing the set is returned
        */
        std::string ToString() const;

        /**
        * Number of metainfo
        *
        * @return The size of the set is returned
        */
        size_t size() const { return data_.size(); }

        /**
        * Creates and returns a vector of JSON nodes containing the key=value pairs.
         */
        std::vector< JSONNode> GenerateNodes() const;

        /**
        * Adds to the set a key=value pair from a JSON node. The node must be of the types
        * JSON_BOOL, JSON_NUMBER or JSON_STRING, otherwise a serialization exception is raised
        *
        * @param node The node containing the information
        */
        void AddNode(const JSONNode& node);

        /**
        * Comparison operator. Allows to check whether two metainfo sets have the same
        * metainformation (i.e. same keys and values)
        */
        bool operator==(const MetainfoSet& other) const;

        /**
        * Comparison operator. Allows to order objects of this type, but has no semantic meaning.
        *
        * @param other Another MetainfoSet
        *
        * @return True is returned iff the current object is considered less than the other
        */
        bool operator<(const MetainfoSet& other) const;

    private:
        std::map<std::string, boost::any> data_;

        std::map<std::string, boost::any>::const_iterator checkKeyExists(const std::string& key) const;
    };

} //namespace ser
