// Created by Andrea Arteaga, MeteoSwiss
// Email: andyspiros@gmail.com
// January 2013

#pragma once

void SerializerOutput::Init(Serializer& serializer, std::string savePointName)
{
    pSerializer_ = &serializer;
    savePoint_.set_Name(savePointName);
}

SerializerOutput& SerializerOutput::operator<< (const MetaInfo& info)
{
    savePoint_.AddMetaInfo(info);
    return *this;
}

template<typename TDataField>
SerializerOutput& SerializerOutput::operator<< (const NamedField<TDataField>& namedField)
{
    pSerializer_->Save(namedField.field(), savePoint_, namedField.name());
    return *this;
}

template<typename TDataField>
SerializerOutput& SerializerOutput::operator<< (const TDataField& field)
{
    pSerializer_->Save(field, savePoint_);
    return *this;
}


