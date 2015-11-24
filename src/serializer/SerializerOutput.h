// Created by Andrea Arteaga, MeteoSwiss
// Email: andyspiros@gmail.com
// January 2013

#pragma once

#include "SavePoint.h"
#include "NamedField.h"

class Serializer;

class SerializerOutput
{
public:
    /**
     * Default constructor
     */
    SerializerOutput() { }

    /**
     * Copy constructor
     */
    SerializerOutput(const SerializerOutput& other)
    {
        *this = other;
    }

    /**
     * Assignment operator
     */
    SerializerOutput& operator=(const SerializerOutput& other)
    {
        pSerializer_ = other.pSerializer_;
        savePoint_ = other.savePoint_;
        return *this;
    }
    inline void Init(Serializer& serializer, std::string savePointName);

    inline void set_SavePoint(const SavePoint& other) { savePoint_ = other; }

    inline SerializerOutput& operator<< (const MetaInfo& info);

    template<typename TDataField>
    inline SerializerOutput& operator<< (const NamedField<TDataField>& namedField);

    template<typename TDataField>
    inline SerializerOutput& operator<< (const TDataField& field);

private:
    SavePoint savePoint_;
    Serializer* pSerializer_;
};


