//This file is released under terms of BSD license`
//See LICENSE.txt for more information

#include "SerializationWrapper.h"

#include "serializer/Serializer.h"
#include "serializer/Savepoint.h"

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <algorithm>

// Management of serializers
using namespace ser;

void* fs_create_serializer(const char* directory, int directory_length,
                           const char* prefix, int prefix_length,
                           char openmode)
{
    std::string directory_str(directory, directory_length);
    std::string prefix_str(prefix, prefix_length);

    SerializerOpenMode mode;
    switch(openmode)
    {
        case 'r':
            mode = SerializerOpenModeRead;
            break;
        case 'w':
            mode = SerializerOpenModeWrite;
            break;
        case 'a':
            mode = SerializerOpenModeAppend;
            break;
        default:
            std::cerr << "Error: openmode \"" << openmode << "\" not supported\n";
            std::exit(-1);
    }

    // Create serializer
    Serializer* pSerializer = new Serializer;
    pSerializer->Init(directory_str, prefix_str, mode);

    return reinterpret_cast<void*>(pSerializer);
}

void fs_destroy_serializer(void* serializer)
{
    Serializer* pSerializer = reinterpret_cast<Serializer*>(serializer);
    delete pSerializer;
}

char fs_serializer_openmode(void* serializer)
{
    switch (reinterpret_cast<Serializer*>(serializer)->mode())
    {
        case SerializerOpenModeRead:
            return 'r';
        case SerializerOpenModeWrite:
            return 'w';
        case SerializerOpenModeAppend:
            return 'a';
    }

    // Avoid warning
    return ' ';
}

int fs_serializer_metainfo_size(void* serializer)
{
    return reinterpret_cast<const Serializer*>(serializer)->globalMetainfo().size();
}

void fs_serializer_metainfo_key_lengths(void* serializer, int* lengths)
{
    const std::vector<std::string> keys = reinterpret_cast<Serializer*>(serializer)->globalMetainfo().keys();
    const int N = keys.size();
    for (int i = 0; i < N; ++i)
        lengths[i] = keys.at(i).size();
}

void fs_serializer_metainfo_get_keys(void* serializer, char** keys)
{
    const std::vector<std::string> mkeys = reinterpret_cast<Serializer*>(serializer)->globalMetainfo().keys();
    const int N = mkeys.size();

    for (int i = 0; i < N; ++i)
    {
        std::strcpy(keys[i], mkeys.at(i).c_str());
    }
}

void fs_serializer_metainfo_get_types(void* serializer, int* types)
{
    const std::vector<int> mtypes = reinterpret_cast<Serializer*>(serializer)->globalMetainfo().types();
    const int N = mtypes.size();
    for (int i = 0; i < N; ++i)
        types[i] = mtypes.at(i);
}

void fs_get_serializer_metainfo_b(void* serializer, const char* name, int name_length, bool* value)
{
    Serializer& sp = *reinterpret_cast<Serializer*>(serializer);
    sp.globalMetainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_serializer_metainfo_i(void* serializer, const char* name, int name_length, int* value)
{
    Serializer& sp = *reinterpret_cast<Serializer*>(serializer);
    sp.globalMetainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_serializer_metainfo_f(void* serializer, const char* name, int name_length, float* value)
{
    Serializer& sp = *reinterpret_cast<Serializer*>(serializer);
    sp.globalMetainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_serializer_metainfo_d(void* serializer, const char* name, int name_length, double* value)
{
    Serializer& sp = *reinterpret_cast<Serializer*>(serializer);
    sp.globalMetainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_serializer_metainfo_s(void* serializer, const char* name, int name_length, char* value)
{
    Serializer& sp = *reinterpret_cast<Serializer*>(serializer);
    std::string v;
    sp.globalMetainfo().ExtractValue(std::string(name, name_length), v);
    std::strcpy(value, v.c_str());
}

void fs_add_serializer_metainfo_b(void* serializer, const char* name, int name_length,   bool value)
{
    reinterpret_cast<Serializer*>(serializer)->AddMetainfo(std::string(name, name_length), value);
}

void fs_add_serializer_metainfo_i(void* serializer, const char* name, int name_length,    int value)
{
    reinterpret_cast<Serializer*>(serializer)->AddMetainfo(std::string(name, name_length), value);
}

void fs_add_serializer_metainfo_f(void* serializer, const char* name, int name_length,  float value)
{
    reinterpret_cast<Serializer*>(serializer)->AddMetainfo(std::string(name, name_length), value);
}

void fs_add_serializer_metainfo_d(void* serializer, const char* name, int name_length, double value)
{
    reinterpret_cast<Serializer*>(serializer)->AddMetainfo(std::string(name, name_length), value);
}

void fs_add_serializer_metainfo_s(void* serializer, const char* name, int name_length, const char* value, int value_length)
{
    reinterpret_cast<Serializer*>(serializer)->AddMetainfo(std::string(name, name_length), std::string(value, value_length));
}

int fs_savepoints(void* serializer)
{
    return reinterpret_cast<Serializer*>(serializer)->savepoints().size();
}

void* fs_get_savepoint(void* serializer, int spid)
{
    Savepoint* sp = new Savepoint;
    *sp = reinterpret_cast<Serializer*>(serializer)->savepoints().at(spid);
    return reinterpret_cast<void*>(sp);
}

int fs_fields(void* serializer)
{
    return reinterpret_cast<Serializer*>(serializer)->fieldnames().size();
}

void fs_fieldname_lengths(void* serializer, int* fieldname_lengths)
{
    const std::vector<std::string> fieldnames = reinterpret_cast<Serializer*>(serializer)->fieldnames();
    for (int i = 0; i < (int)fieldnames.size(); ++i)
        fieldname_lengths[i] = fieldnames.at(i).size();
}

void fs_get_fieldnames(void* serializer, char** fieldnames)
{
    const std::vector<std::string> mfieldnames = reinterpret_cast<Serializer*>(serializer)->fieldnames();
    for (int i = 0; i < (int)mfieldnames.size(); ++i)
    {
        std::strcpy(fieldnames[i], mfieldnames.at(i).c_str());
    }
}

void fs_get_fieldinfo(void* serializer, const char* fieldname, int fieldname_length,
                      int* bytes_per_element,
                      int* isize, int* jsize, int* ksize, int* lsize,
                      int* iminushalo, int* iplushalo, int* jminushalo, int* jplushalo,
                      int* kminushalo, int* kplushalo, int* lminushalo, int* lplushalo)
{
    const DataFieldInfo& info = reinterpret_cast<Serializer*>(serializer)->FindField(std::string(fieldname, fieldname_length));

    *bytes_per_element = info.bytesPerElement();
    *isize = info.iSize();
    *jsize = info.jSize();
    *ksize = info.kSize();
    *lsize = info.lSize();
    *iminushalo = info.iMinusHaloSize();
    *iplushalo = info.iPlusHaloSize();
    *jminushalo = info.jMinusHaloSize();
    *jplushalo = info.jPlusHaloSize();
    *kminushalo = info.kMinusHaloSize();
    *kplushalo = info.kPlusHaloSize();
    *lminushalo = info.lMinusHaloSize();
    *lplushalo = info.lPlusHaloSize();
}

int fs_get_field_type_length(void* serializer, const char* fieldname, int fieldname_length)
{
    return reinterpret_cast<Serializer*>(serializer)->FindField(std::string(fieldname, fieldname_length)).type().size();
}

void fs_get_field_type(void* serializer, const char* fieldname, int fieldname_length, char* datatype)
{
    std::strcpy(datatype,
                reinterpret_cast<Serializer*>(serializer)->FindField(std::string(fieldname, fieldname_length)).type().c_str());
}

int fs_fields_at_savepoint(void* serializer, void* savepoint)
{
    return reinterpret_cast<Serializer*>(serializer)->FieldsAtSavepoint(*reinterpret_cast<Savepoint*>(savepoint)).size();
}

void fs_fields_at_savepoint_name_lengths(void* serializer, void* savepoint, int* name_lengths)
{
    const std::vector<std::string> names = reinterpret_cast<Serializer*>(serializer)->FieldsAtSavepoint(
        *reinterpret_cast<Savepoint*>(savepoint));

    for (int i = 0; i < (int)names.size(); ++i)
        name_lengths[i] = names[i].size();
}

void fs_fields_at_savepoint_names(void* serializer, void* savepoint, char** names)
{
    const std::vector<std::string> mnames = reinterpret_cast<Serializer*>(serializer)->FieldsAtSavepoint(
        *reinterpret_cast<Savepoint*>(savepoint));

    for (int i = 0; i < (int)mnames.size(); ++i)
        std::strcpy(names[i], mnames[i].c_str());
}

void fs_register_field(void* serializer, const char* name, int name_length,
                       const char* data_type, int data_type_length, int bytes_per_element,
                       int isize, int jsize, int ksize, int lsize,
                       int iminushalo, int iplushalo, int jminushalo, int jplushalo,
                       int kminushalo, int kplushalo, int lminushalo, int lplushalo)
{
    reinterpret_cast<Serializer*>(serializer)->RegisterField(
                         std::string(name, name_length), std::string(data_type, data_type_length),
                         bytes_per_element, isize, jsize, ksize, lsize,
                         iminushalo, iplushalo, jminushalo, jplushalo,
                         kminushalo, kplushalo, lminushalo, lplushalo);
}

int fs_field_exists(void* serializer, const char* fieldname, int fieldname_length)
{
    try
    {
        reinterpret_cast<Serializer*>(serializer)->FindField(std::string(fieldname, fieldname_length));
        return 1;
    }
    catch(SerializationException&)
    {
        return 0;
    }
}

int fs_get_element_size(void* serializer, const char* name, int name_length)
{
    const DataFieldInfo& fieldinfo = reinterpret_cast<const Serializer*>(serializer)->FindField(std::string(name, name_length));
    return fieldinfo.bytesPerElement();
}

void fs_get_field_size(void* serializer, const char* name, int name_length,
                       int* isize, int* jsize, int* ksize, int* lsize)
{
    const DataFieldInfo& fieldinfo = reinterpret_cast<const Serializer*>(serializer)->FindField(std::string(name, name_length));
    *isize = fieldinfo.iSize();
    *jsize = fieldinfo.jSize();
    *ksize = fieldinfo.kSize();
    *lsize = fieldinfo.lSize();
}

void fs_add_field_metainfo_b(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,   bool value)
{
    reinterpret_cast<Serializer*>(serializer)->AddFieldMetainfo(std::string(fieldname, fieldname_length), std::string(name, name_length), value);
}

void fs_add_field_metainfo_i(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,    int value)
{
    reinterpret_cast<Serializer*>(serializer)->AddFieldMetainfo(std::string(fieldname, fieldname_length), std::string(name, name_length), value);
}

void fs_add_field_metainfo_f(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,  float value)
{
    reinterpret_cast<Serializer*>(serializer)->AddFieldMetainfo(std::string(fieldname, fieldname_length), std::string(name, name_length), value);
}

void fs_add_field_metainfo_d(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length, double value)
{
    reinterpret_cast<Serializer*>(serializer)->AddFieldMetainfo(std::string(fieldname, fieldname_length), std::string(name, name_length), value);
}

void fs_add_field_metainfo_s(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length, const char* value, int value_length)
{
    reinterpret_cast<Serializer*>(serializer)->AddFieldMetainfo(std::string(fieldname, fieldname_length), std::string(name, name_length), std::string(value, value_length));
}

void* fs_create_savepoint(const char* name, int name_length)
{
    Savepoint* pSavepoint = new Savepoint;
    pSavepoint->Init(std::string(name, name_length));
    return reinterpret_cast<void*>(pSavepoint);
}

void* fs_duplicate_savepoint(void* savepoint)
{
    Savepoint* pSavepoint = new Savepoint;
    *pSavepoint = *reinterpret_cast<Savepoint*>(savepoint);
    return reinterpret_cast<void*>(pSavepoint);
}

void fs_destroy_savepoint(void* savepoint)
{
    Savepoint* pSavepoint = reinterpret_cast<Savepoint*>(savepoint);
    delete pSavepoint;
}

void fs_reinitialize_savepoint(void* savepoint, const char* name, int name_length)
{
    reinterpret_cast<Savepoint*>(savepoint)->Init(std::string(name, name_length));
}

int fs_savepoint_name_length(void* savepoint)
{
    return reinterpret_cast<Savepoint*>(savepoint)->name().length();
}

void fs_savepoint_get_name(void* savepoint, char* name)
{
    std::strcpy(name, reinterpret_cast<Savepoint*>(savepoint)->name().c_str());
}

void fs_savepoint_key_lengths(void* savepoint, int* lengths)
{
    const std::vector<std::string> keys = reinterpret_cast<Savepoint*>(savepoint)->metainfo().keys();
    const int N = keys.size();
    for (int i = 0; i < N; ++i)
        lengths[i] = keys.at(i).size();
}

void fs_savepoint_get_keys(void* savepoint, char** keys)
{
    const std::vector<std::string> mkeys = reinterpret_cast<Savepoint*>(savepoint)->metainfo().keys();
    const int N = mkeys.size();

    for (int i = 0; i < N; ++i)
    {
        std::strcpy(keys[i], mkeys.at(i).c_str());
    }
}

void fs_savepoint_get_types(void* savepoint, int* types)
{
    const std::vector<int> mtypes = reinterpret_cast<Savepoint*>(savepoint)->metainfo().types();
    const int N = mtypes.size();
    for (int i = 0; i < N; ++i)
        types[i] = mtypes.at(i);
}

void fs_add_savepoint_metainfo_b(void* savepoint, const char* name, int name_length,   bool value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.AddMetainfo(std::string(name, name_length), value);
}

void fs_add_savepoint_metainfo_i(void* savepoint, const char* name, int name_length,    int value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.AddMetainfo(std::string(name, name_length), value);
}

void fs_add_savepoint_metainfo_f(void* savepoint, const char* name, int name_length,  float value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.AddMetainfo(std::string(name, name_length), value);
}

void fs_add_savepoint_metainfo_d(void* savepoint, const char* name, int name_length, double value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.AddMetainfo(std::string(name, name_length), value);
}

void fs_add_savepoint_metainfo_s(void* savepoint, const char* name, int name_length, const char* value, int value_length)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.AddMetainfo(std::string(name, name_length), std::string(value, value_length));
}

void fs_get_savepoint_metainfo_b(void* savepoint, const char* name, int name_length, bool* value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.metainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_savepoint_metainfo_i(void* savepoint, const char* name, int name_length, int* value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.metainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_savepoint_metainfo_f(void* savepoint, const char* name, int name_length, float* value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.metainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_savepoint_metainfo_d(void* savepoint, const char* name, int name_length, double* value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    sp.metainfo().ExtractValue(std::string(name, name_length), *value);
}

void fs_get_savepoint_metainfo_s(void* savepoint, const char* name, int name_length, char* value)
{
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    std::string v;
    sp.metainfo().ExtractValue(std::string(name, name_length), v);
    std::strcpy(value, v.c_str());
}

int fs_savepoint_metainfo_size(void* savepoint)
{
    return reinterpret_cast<const Savepoint*>(savepoint)->metainfo().size();
}

void fs_read_field(void* serializer, void* savepoint, const char* fieldname, int name_length,
                   void* data, int istride, int jstride, int kstride, int lstride)
{
    Serializer& ser = *reinterpret_cast<Serializer*>(serializer);
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);

    ser.ReadField(std::string(fieldname, name_length), sp, data, istride, jstride, kstride, lstride);
}

void fs_write_field(void* serializer, void* savepoint, const char* fieldname, int name_length,
                    const void* data, int istride, int jstride, int kstride, int lstride)
{
    Serializer& ser = *reinterpret_cast<Serializer*>(serializer);
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);

    ser.WriteField(std::string(fieldname, name_length), sp, data, istride, jstride, kstride, lstride);
}

void fs_enable_serialization()
{
    Serializer::EnableSerialization();
}

void fs_disable_serialization()
{
    Serializer::DisableSerialization();
}

void fs_print_debuginfo(void* serializer)
{
    std::cout << reinterpret_cast<Serializer*>(serializer)->ToString() << std::endl;
}

void fs_check_size(void* serializer, const char* fieldname, int namelength, int* isize, int* jsize, int* ksize, int* lsize)
{
    // Take "proposed" size (i.e. size of field in Fortran/Python),
    // compares to size as registered and reorders the sizes
    const std::string fieldname_str(fieldname, namelength);

    const Serializer& ser = *reinterpret_cast<const Serializer*>(serializer);
    const DataFieldInfo& info = ser.FindField(fieldname_str);

    int actual_sizes[4] = { *isize, *jsize, *ksize, *lsize };
    int ref_sizes[4] = { info.iSize(), info.jSize(), info.kSize(), info.lSize() };

    // Check rank
    int actual_rank = (actual_sizes[0] != 0 ? 1 : 0) + (actual_sizes[1] != 0 ? 1 : 0) + (actual_sizes[2] != 0 ? 1 : 0) + (actual_sizes[3] != 0 ? 1 : 0);
    int ref_rank = (ref_sizes[0] != 0 ? 1 : 0) + (ref_sizes[1] != 0 ? 1 : 0) + (ref_sizes[2] != 0 ? 1 : 0) + (ref_sizes[3] != 0 ? 1 : 0);

    if (actual_rank != ref_rank)
    {
        std::cout << "Error: field " << fieldname_str << " passed with rank " << actual_rank
                  << ", but was registered with rank " << ref_rank << "\n";
        std::exit(1);
    }

    // Reorder
    bool has_rank[4];
    for (int i = 0; i < 4; ++i)
        has_rank[i] = ref_sizes[i] > 1;

    for (int i = 0; i < 4; ++i)
    {
        if (actual_sizes[i] == ref_sizes[i])
            continue;

        if (ref_sizes[i] == 1)
        {
            for (int j = 3; j > i; --j)
                actual_sizes[j] = actual_sizes[j-1];
            actual_sizes[i] = 1;
            continue;
        }

        std::cout << "Error: sizes of field " << fieldname_str << " passed with sizes ("
            << actual_sizes[0] << "x"<< actual_sizes[1] << "x"<< actual_sizes[2] << "x"<< actual_sizes[3] << ")"
            << ", but registered with sizes ("
            << ref_sizes[0] << "x"<< ref_sizes[1] << "x"<< ref_sizes[2] << "x"<< ref_sizes[3] << ")\n";
        std::exit(1);
    }
}

void fs_compute_strides(void* serializer, const char* fieldname, int namelength,
                        const void* base_ptr, const void* iplus1, const void* jplus1, const void* kplus1, const void* lplus1,
                        int* istride, int* jstride, int* kstride, int* lstride)
{
    const Serializer& ser = *reinterpret_cast<const Serializer*>(serializer);
    const DataFieldInfo& info = ser.FindField(std::string(fieldname, namelength));

    // Check rank
    long strides[4] = {
            reinterpret_cast<const char*>(iplus1) - reinterpret_cast<const char*>(base_ptr),
            reinterpret_cast<const char*>(jplus1) - reinterpret_cast<const char*>(base_ptr),
            reinterpret_cast<const char*>(kplus1) - reinterpret_cast<const char*>(base_ptr),
            reinterpret_cast<const char*>(lplus1) - reinterpret_cast<const char*>(base_ptr)
        };
    const bool storage[4] = { info.iSize() > 1, info.jSize() > 1, info.kSize() > 1, info.lSize() > 1 };
    const int rank       = (strides[0] > 0 ? 1 : 0) + (strides[1] > 0 ? 1 : 0) + (strides[2] > 0 ? 1 : 0) + (strides[3] > 0 ? 1 : 0);
    const int shouldrank = (storage[0] > 0 ? 1 : 0) + (storage[1] > 0 ? 1 : 0) + (storage[2] > 0 ? 1 : 0) + (storage[3] > 0 ? 1 : 0);
    if (rank != shouldrank)
    {
        std::cout << "Error: field " << std::string(fieldname, namelength) << " has rank " << shouldrank
            << ", but field with rank " << rank << " was passed\n";
        std::exit(1);
    }

    // Reorder strides
    for (int i = 0; i < 4; ++i)
    {
        if (!storage[i])
        {
            // Shift strides to the left and set the current one to 0
            for (int j = 3; j > i; --j)
                strides[j] = strides[j-1];
            strides[i] = 0;
        }
    }

    *istride = strides[0];
    *jstride = strides[1];
    *kstride = strides[2];
    *lstride = strides[3];
}

int fs_field_exists_at_savepoint(void* serializer, void* savepoint, const char* fieldname, int fieldname_length)
{
    Serializer& ser = *reinterpret_cast<Serializer*>(serializer);
    Savepoint& sp = *reinterpret_cast<Savepoint*>(savepoint);
    std::string name(fieldname, fieldname_length);

    std::vector<std::string> ff = ser.FieldsAtSavepoint(sp);
    return std::find(ff.begin(), ff.end(), name) != ff.end() ? 1 : 0;
}

