#pragma once

#include <string>

template<typename TDataField>
class NamedField
{
public:
    NamedField() { }

    NamedField(const NamedField<TDataField>& other)
    {
        name_ = other.name_;
        pField_ = other.pField_;
    }

    void Init(const std::string& name, const TDataField& field)
    {
        name_ = name;
        pField_ = &field;
    }

    const std::string& name () const { return name_; }
    const TDataField& field () const { return *pField_; }

private:
    std::string name_;
    const TDataField* pField_;
};

template<typename TDataField>
NamedField<TDataField> named_field(const std::string& name, const TDataField& field)
{
    NamedField<TDataField> namedField;
    namedField.Init(name, field);
    return namedField;
}

