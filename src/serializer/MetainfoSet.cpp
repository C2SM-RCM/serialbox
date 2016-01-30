//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "MetainfoSet.h"
#include <string>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include "libjson.h"

using namespace ser;

static inline int getAnyTypeVal(const boost::any& a)
{
    const std::type_info& t = a.type();
    if (t == typeid(bool))
        return 0;
    if (t == typeid(int))
        return 1;
    if (t == typeid(float))
        return 2;
    if (t == typeid(double))
        return 3;
    if (t == typeid(std::string))
        return 4;
    return -1;
}

template<typename T>
static int compareBase(const boost::any& a1, const boost::any& a2)
{
    const T& v1 = boost::any_cast<T>(a1);
    const T& v2 = boost::any_cast<T>(a2);
    return (v1 < v2 ? -1 : (v1 > v2 ? +1 : 0));
}

/*
 * Returns  0 if contents are equal or non-comparable (in case of floating-point)
 *         -1 if a1 less-than
 *         +1 if a2 greater-than
 *
 * Different types:
 * bool < int < float < double < string
 */
static int compareAny(const boost::any& a1, const boost::any& a2)
{
    using boost::any_cast;
    const std::type_info& t = a1.type();
    if (a1.type() != a2.type())
    {
        // Case 1: different types
        int t1 = getAnyTypeVal(a1);
        int t2 = getAnyTypeVal(a2);
        return (t1 < t2 ? -1 : 1);
    }
    else
    {
        // Case 2: same type
        if(t == typeid(bool))
            return compareBase<bool>(a1, a2);

        if(t == typeid(int))
            return compareBase<int>(a1, a2);

        if(t == typeid(float))
            return compareBase<float>(a1, a2);

        if(t == typeid(double))
            return compareBase<double>(a1, a2);

        if(t == typeid(std::string))
            return compareBase<std::string>(a1, a2);

        return -1;
    }
}

static std::string writeAny(const boost::any& a)
{
    const std::type_info& t = a.type();
    std::ostringstream ss;

         if(t == typeid(bool)) ss << (boost::any_cast<bool>(a) ? "true" : "false");
    else if(t == typeid(int)) ss << boost::any_cast<int>(a);
    else if(t == typeid(float)) ss << boost::any_cast<float>(a);
    else if(t == typeid(double)) ss << boost::any_cast<double>(a);
    else if(t == typeid(std::string)) ss << "\"" << boost::any_cast<std::string>(a) << "\"";

    return ss.str();
}

static JSONNode anyNode(const std::string name, const boost::any& a)
{
    const std::type_info& t = a.type();
         if(t == typeid(bool)) return JSONNode(name, boost::any_cast<bool>(a));
    else if(t == typeid(int)) return JSONNode(name, boost::any_cast<int>(a));
    else if(t == typeid(float)) return JSONNode(name, boost::any_cast<float>(a));
    else if(t == typeid(double)) return JSONNode(name, boost::any_cast<double>(a));
    else if(t == typeid(std::string)) return JSONNode(name, boost::any_cast<std::string>(a));

    // This case should not be possible, unless there is a bug
    SerializationException exception;
    std::ostringstream ss;
    ss << "Error: generation of node " << name << " in anyNode (MetainfoSet.cpp file) is not possible"
       << " because the type " << a.type().name() << " is not supported\n"
       << "This is a bug and should be notified\n";
    exception.Init(ss.str());
    throw exception;
}

/////////////////////////////////////
// IMPLEMENTATION OF CLASS METHODS //
/////////////////////////////////////

MetainfoSet& MetainfoSet::operator=(const MetainfoSet& other)
{
    data_ = other.data_;
    return *this;
}

void MetainfoSet::Cleanup()
{
    data_.clear();
}

std::vector<std::string> MetainfoSet::keys() const
{
    std::vector<std::string> ret;
    ret.reserve(data_.size());
    for (std::map<std::string, boost::any>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
        ret.push_back(iter->first);
    return ret;
}

std::vector<int> MetainfoSet::types() const
{
    std::vector<int> ret;
    ret.reserve(data_.size());
    for (std::map<std::string, boost::any>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
    {
        if (iter->second.type() == typeid(bool))
            ret.push_back(-1);
        else if (iter->second.type() == typeid(int))
            ret.push_back(-2);
        else if (iter->second.type() == typeid(float))
            ret.push_back(-3);
        else if (iter->second.type() == typeid(double))
            ret.push_back(-4);
        else if (iter->second.type() == typeid(std::string))
            ret.push_back(boost::any_cast<std::string>(iter->second).size());
        else
        {
            std::cout << "Error: type of metainfo " << iter->first << " is not supported\n";
            std::exit(1);
        }
    }
    return ret;
}

const boost::any& MetainfoSet::AsAny(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    return iter->second;
}

bool MetainfoSet::AsBool(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    const boost::any& a = iter->second;
    const std::type_info& t = a.type();

         if(t == typeid(bool)) return boost::any_cast<bool>(a);
    else if(t == typeid(int)) return (bool)boost::any_cast<int>(a);
    else if(t == typeid(float)) return (bool)boost::any_cast<float>(a);
    else if(t == typeid(double)) return (bool)boost::any_cast<double>(a);
    else if(t == typeid(std::string))
    {
        bool v;
        std::stringstream ss;
        ss << boost::any_cast<std::string>(a);
        ss >> v;
        return v;
    }

    // Reaching this point is a bug
    std::cerr << "Bug in MetainfoSet: type of '" << key << "' not recognized\n";
    std::exit(1);
    return true;
}

int MetainfoSet::AsInt(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    const boost::any& a = iter->second;
    const std::type_info& t = a.type();

         if(t == typeid(bool)) return boost::any_cast<bool>(a);
    else if(t == typeid(int)) return boost::any_cast<int>(a);
    else if(t == typeid(float))
    {
        const float v = boost::any_cast<float>(a);
        if (v == static_cast<float>(static_cast<int>(v))) return static_cast<int>(v);
        else
        {
            SerializationException exception;
            std::ostringstream ss;
            ss << "Error: Trying to access the key " << key << " as int, but its value is " << v << "\n";
            exception.Init(ss.str());
            throw exception;
        }
    }
    else if(t == typeid(double))
    {
        const double v = boost::any_cast<double>(a);
        if (v == static_cast<double>(static_cast<int>(v))) return static_cast<int>(v);
        else
        {
            SerializationException exception;
            std::ostringstream ss;
            ss << "Error: Trying to access the key " << key << " as int, but its value is " << v << "\n";
            exception.Init(ss.str());
            throw exception;
        }
    }
    else if(t == typeid(std::string))
    {
        int v;
        std::stringstream ss;
        ss << boost::any_cast<std::string>(a);
        ss >> v;
        return v;
    }

    // Reaching this point is a bug.
    std::cerr << "Bug in MetainfoSet: type of '" << key << "' not recognized\n";
    std::exit(1);
    return 0;
}

float MetainfoSet::AsFloat(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    const boost::any& a = iter->second;
    const std::type_info& t = a.type();

         if(t == typeid(bool)) return boost::any_cast<bool>(a);
    else if(t == typeid(int)) return boost::any_cast<int>(a);
    else if(t == typeid(float)) return boost::any_cast<float>(a);
    else if(t == typeid(double)) return boost::any_cast<double>(a);
    else if(t == typeid(std::string))
    {
        float v;
        std::stringstream ss;
        ss << boost::any_cast<std::string>(a);
        ss >> v;
        return v;
    }

    // Reaching this point is a bug.
    std::cerr << "Bug in MetainfoSet: type of '" << key << "' not recognized\n";
    std::exit(1);
    return (float)0.;
}

float MetainfoSet::AsDouble(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    const boost::any& a = iter->second;
    const std::type_info& t = a.type();

         if(t == typeid(bool)) return boost::any_cast<bool>(a);
    else if(t == typeid(int)) return boost::any_cast<int>(a);
    else if(t == typeid(float)) return boost::any_cast<float>(a);
    else if(t == typeid(double)) return boost::any_cast<double>(a);
    else if(t == typeid(std::string))
    {
        float v;
        std::stringstream ss;
        ss << boost::any_cast<std::string>(a);
        ss >> v;
        return v;
    }

    // Reaching this point is a bug.
    std::cerr << "Bug in MetainfoSet: type of '" << key << "' not recognized\n";
    std::exit(1);
    return (double)0.;
}

std::string MetainfoSet::AsString(const std::string& key) const
{
    std::ostringstream ss;
    std::map<std::string, boost::any>::const_iterator iter = checkKeyExists(key);
    const boost::any& a = iter->second;
    const std::type_info& t = a.type();

         if(t == typeid(bool)) ss << std::boolalpha << boost::any_cast<bool>(a);
    else if(t == typeid(int)) ss << boost::any_cast<int>(a);
    else if(t == typeid(float)) ss << boost::any_cast<float>(a);
    else if(t == typeid(double)) ss << boost::any_cast<double>(a);
    else if(t == typeid(std::string)) ss << boost::any_cast<std::string>(a);
    else
    {
        std::cerr << "Bug in MetainfoSet: type of '" << key << "' not recognized\n";
        std::exit(1);
    }

    return ss.str();
}

std::string MetainfoSet::ToString() const
{
    std::ostringstream ss;
    ss << "[ ";

    for (std::map<std::string, boost::any>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
    {
        ss << iter->first << "=" << writeAny(iter->second) << " ";
    }

    ss << "]";
    return ss.str();
}

std::vector< JSONNode> MetainfoSet::GenerateNodes() const
{
    std::vector< JSONNode> res;
    res.reserve(data_.size());
    for (std::map<std::string, boost::any>::const_iterator iter = data_.begin(); iter != data_.end(); ++iter)
    {
        res.push_back(anyNode(iter->first, iter->second));
    }
    return res;
}

void MetainfoSet::AddNode(const JSONNode& node)
{
    JSON_NUMBER_TYPE r;
    switch (node.type())
    {
    case JSON_BOOL:
        return AddMetainfo(node.name(), node.as_bool());
    case JSON_STRING:
        return AddMetainfo(node.name(), node.as_string());
    case JSON_NUMBER:
        r = node.as_float();
        if (r == static_cast<JSON_NUMBER_TYPE>(static_cast<int>(r)))
            return AddMetainfo(node.name(), static_cast<int>(r));
        else
            return AddMetainfo(node.name(), r);
    default:
        break;
    }

    SerializationException exception;
    std::ostringstream ss;
    ss << "Error: JSON node has wrong type and could not be parsed:\n" << node.write_formatted() << "\n";
    exception.Init(ss.str());
    throw exception;
}

bool MetainfoSet::operator==(const MetainfoSet& other) const
{
    // Size of map
    if(data_.size() != other.data_.size())
        return false;

    // Same size: compare element by element
    typedef std::map<std::string, boost::any>::const_iterator iter_t;

    for (iter_t d1 = data_.begin(); d1 != data_.end(); ++d1)
    {
        // Find key in other set
        iter_t d2 = other.data_.find(d1->first);

        // If not found, return false
        if (d2 == other.data_.end())
            return false;

        // Compare values
        if (compareAny(d1->second, d2->second) != 0)
            return false;
    }

    return true;
}


bool MetainfoSet::operator<(const MetainfoSet& other) const
{
    // Size of map
    if(data_.size() < other.data_.size())
        return true;

    // Same size: compare element by element
    typedef std::map<std::string, boost::any>::const_iterator iter_t;
    const int N = data_.size();
    iter_t d1 = data_.begin();
    iter_t d2 = other.data_.begin();

    for (int i = 0; i < N; ++i, ++d1, ++d2)
    {
        // Compare keys
        if (d1->first != d2->first)
            return d1->first < d2->first;

        // Compare values
        int valcompare = compareAny(d1->second, d2->second);
        if (valcompare < 0)
            return true;
        if (valcompare > 0)
            return false;
    }

    // Both are equal or other is smaller
    return false;
}

std::map<std::string, boost::any>::const_iterator MetainfoSet::checkKeyExists(const std::string& key) const
{
    std::map<std::string, boost::any>::const_iterator iter = data_.find(key);
    if (iter == data_.end())
    {
        SerializationException exception;
        std::ostringstream ss;
        ss << "Error: Requested key " << key << " is not in set\n";
        exception.Init(ss.str());
        throw exception;
    }
    return iter;
}

