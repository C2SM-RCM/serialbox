#pragma once
//This file is released under terms of BSD license`
//See LICENSE.txt for more information

extern "C"
{
    // Management of serializers
    void* fs_create_serializer(const char* directory, int directory_length,
                               const char* prefix, int prefix_length,
                               char openmode);
    void fs_destroy_serializer(void* serializer);
    char fs_serializer_openmode(void* serializer);
    int  fs_serializer_metainfo_size(void* savepoint);
    void fs_serializer_metainfo_key_lengths(void* savepoint, int* lengths);
    void fs_serializer_metainfo_get_keys(void* savepoint, char** lengths);
    void fs_serializer_metainfo_get_types(void* savepoint, int* types);
    void fs_get_serializer_metainfo_b(void* serializer, const char* name, int name_length,   bool* value);
    void fs_get_serializer_metainfo_i(void* serializer, const char* name, int name_length,    int* value);
    void fs_get_serializer_metainfo_f(void* serializer, const char* name, int name_length,  float* value);
    void fs_get_serializer_metainfo_d(void* serializer, const char* name, int name_length, double* value);
    void fs_get_serializer_metainfo_s(void* serializer, const char* name, int name_length, char* value);
    void fs_add_serializer_metainfo_b(void* serializer, const char* name, int name_length,    bool value);
    void fs_add_serializer_metainfo_i(void* serializer, const char* name, int name_length,     int value);
    void fs_add_serializer_metainfo_f(void* serializer, const char* name, int name_length,   float value);
    void fs_add_serializer_metainfo_d(void* serializer, const char* name, int name_length,  double value);
    void fs_add_serializer_metainfo_s(void* serializer, const char* name, int name_length, const char* value, int value_length);
    int fs_savepoints(void* serializer);
    void* fs_get_savepoint(void* serializer, int id);
    int fs_fields(void* serializer);
    void fs_fieldname_lengths(void* serializer, int* fieldname_lengths);
    void fs_get_fieldnames(void* serializer, char** fieldnames);
    void fs_get_fieldinfo(void* serializer, const char* fieldname, int fieldname_length,
                          int* bytes_per_element,
                          int* isize, int* jsize, int* ksize, int* lsize,
                          int* iminushalo, int* iplushalo, int* jminushalo, int* jplushalo,
                          int* kminushalo, int* kplushalo, int* lminushalo, int* lplushalo);
    int fs_get_field_type_length(void* serializer, const char* fieldname, int fieldname_length);
    void fs_get_field_type(void* serializer, const char* fieldname, int fieldname_length, char* datatype);

    int fs_fields_at_savepoint(void* serializer, void* savepoint);
    void fs_fields_at_savepoint_name_lengths(void* serializer, void* savepoint, int* name_lengths);
    void fs_fields_at_savepoint_names(void* serializer, void* savepoint, char** names);

    // Management of fields
    void fs_register_field(void* serializer, const char* name, int name_length,
                           const char* data_type, int data_type_length, int bytes_per_element,
                           int isize, int jsize, int ksize, int lsize,
                           int iminushalo, int iplushalo, int jminushalo, int jplushalo,
                           int kminushalo, int kplushalo, int lminushalo, int lplushalo);
    int fs_field_exists(void* serializer, const char* name, int name_length);
    int fs_get_element_size(void* serializer, const char* name, int name_length);
    void fs_get_field_size(void* serializer, const char* name, int name_length,
                           int* isize, int* jsize, int* ksize, int* lsize);

    void fs_add_field_metainfo_b(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,    bool value);
    void fs_add_field_metainfo_i(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,     int value);
    void fs_add_field_metainfo_f(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,   float value);
    void fs_add_field_metainfo_d(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length,  double value);
    void fs_add_field_metainfo_s(void* serializer, const char* fieldname, int fieldname_length, const char* name, int name_length, const char* value, int value_length);

    // Management of savepoints
    void* fs_create_savepoint(const char* name, int name_length);
    void* fs_duplicate_savepoint(void* savepoint);
    void fs_destroy_savepoint(void* savepoint);
    void fs_reinitialize_savepoint(void* savepoint, const char* name, int name_length);
    int fs_savepoint_metainfo_size(void* savepoint);
    int fs_savepoint_name_length(void* savepoint);
    void fs_savepoint_get_name(void* savepoint, char* name);
    void fs_savepoint_key_lengths(void* savepoint, int* lengths);
    void fs_savepoint_get_keys(void* savepoint, char** keys);
    void fs_savepoint_get_types(void* savepoint, int* types);
    void fs_get_savepoint_metainfo_b(void* savepoint, const char* name, int name_length,   bool* value);
    void fs_get_savepoint_metainfo_i(void* savepoint, const char* name, int name_length,    int* value);
    void fs_get_savepoint_metainfo_f(void* savepoint, const char* name, int name_length,  float* value);
    void fs_get_savepoint_metainfo_d(void* savepoint, const char* name, int name_length, double* value);
    void fs_get_savepoint_metainfo_s(void* savepoint, const char* name, int name_length, char* value);
    void fs_add_savepoint_metainfo_b(void* savepoint, const char* name, int name_length,   bool value);
    void fs_add_savepoint_metainfo_i(void* savepoint, const char* name, int name_length,    int value);
    void fs_add_savepoint_metainfo_f(void* savepoint, const char* name, int name_length,  float value);
    void fs_add_savepoint_metainfo_d(void* savepoint, const char* name, int name_length, double value);
    void fs_add_savepoint_metainfo_s(void* savepoint, const char* name, int name_length, const char* value, int value_length);

    // Read/Write
    void fs_read_field (void* serializer, void* savepoint, const char* fieldname, int name_length,
                        void* data, int istride, int jstride, int kstride, int lstride);
    void fs_write_field(void* serializer, void* savepoint, const char* fieldname, int name_length,
                        const void* data, int istride, int jstride, int kstride, int lstride);

    // Utilities
    void fs_enable_serialization();
    void fs_disable_serialization();
    void fs_print_debuginfo(void* serializer);
    void fs_check_size(void* serializer, const char* fieldname, int namelength, int* isize, int* jsize, int* ksize, int* lsize);
    void fs_compute_strides(void* serializer, const char* fieldname, int namelength,
                            const void* base_ptr, const void* iplus1, const void* jplus1, const void* kplus1, const void* lplus1,
                            int* istride, int* jstride, int* kstride, int* lstride);
    int fs_field_exists_at_savepoint(void* serializer, void* savepoint, const char* fieldname, int fieldname_length);
}
