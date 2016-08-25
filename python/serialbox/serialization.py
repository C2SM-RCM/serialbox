#This file is released under terms of BSD license`
#See LICENSE.txt for more information

"""Internal serialization module that wraps the dynamic library of serialbox. 
This module is used by the Serializer module.
"""

from __future__ import print_function
import os, sys
from os.path import join as pjoin
import ctypes
import numpy as np

try:
    import numpy as np
except:
    raise Exception('The serialization module depends on numpy')

dirs = (
    os.path.dirname(os.path.realpath(__file__)),
    pjoin(os.path.dirname(os.path.realpath(__file__)), os.pardir, 'build', 'src', 'wrapper'),
    '.',
    )

# Load wrapper
wrapper = None
for d in dirs:
    library_postfix = 'so'
    from sys import platform as _platform
    if _platform == "darwin":
        library_postfix = 'dylib'

    # OS X

    libfile = pjoin(d, 'libSerialbox_Wrapper.'+library_postfix)
    try:
        wrapper_try = ctypes.cdll.LoadLibrary(libfile)
        if wrapper_try is not None:
            wrapper = wrapper_try
            break
    except Exception as e:
        #raise
        pass

if wrapper is None:
    raise ImportError("The serialization library could not be found {dirs}".format(dirs=dirs))

def type2dtype(typestr, typesize):
    conv = {
        'int1' : np.int8,
        'int2' : np.int16,
        'int4' : np.int32,
        'int8' : np.int64,
        'float4' : np.float32,
        'float324' : np.float32,
        'double8' : np.float64,
        'float648' : np.float64
        }
    try:
        return conv["{0}{1}".format(typestr,typesize)]
    except KeyError:
        raise Exception('Type \'{0}\' with {1} bytes per element not understood'.format(typestr, typesize))

# Extract wrapper functions
fs_create_serializer = wrapper.fs_create_serializer                               ; fs_create_serializer.restype = ctypes.c_void_p
fs_destroy_serializer = wrapper.fs_destroy_serializer
fs_serializer_metainfo_size = wrapper.fs_serializer_metainfo_size                 ; fs_serializer_metainfo_size.restype = ctypes.c_int
fs_serializer_metainfo_key_lengths = wrapper.fs_serializer_metainfo_key_lengths
fs_serializer_metainfo_get_keys = wrapper.fs_serializer_metainfo_get_keys
fs_serializer_metainfo_get_types = wrapper.fs_serializer_metainfo_get_types
fs_get_serializer_metainfo_b = wrapper.fs_get_serializer_metainfo_b
fs_get_serializer_metainfo_i = wrapper.fs_get_serializer_metainfo_i
fs_get_serializer_metainfo_f = wrapper.fs_get_serializer_metainfo_f
fs_get_serializer_metainfo_d = wrapper.fs_get_serializer_metainfo_d
fs_get_serializer_metainfo_s = wrapper.fs_get_serializer_metainfo_s
fs_add_serializer_metainfo_b = wrapper.fs_add_serializer_metainfo_b
fs_add_serializer_metainfo_i = wrapper.fs_add_serializer_metainfo_i
fs_add_serializer_metainfo_f = wrapper.fs_add_serializer_metainfo_f
fs_add_serializer_metainfo_d = wrapper.fs_add_serializer_metainfo_d
fs_add_serializer_metainfo_s = wrapper.fs_add_serializer_metainfo_s
fs_savepoints = wrapper.fs_savepoints                                             ; fs_savepoints.restype = ctypes.c_int
fs_get_savepoint = wrapper.fs_get_savepoint                                       ; fs_get_savepoint.restype = ctypes.c_void_p
fs_fields = wrapper.fs_fields                                                     ; fs_fields.restype = ctypes.c_int
fs_fieldname_lengths = wrapper.fs_fieldname_lengths
fs_get_fieldnames = wrapper.fs_get_fieldnames
fs_get_fieldinfo = wrapper.fs_get_fieldinfo
fs_get_field_type_length = wrapper.fs_get_field_type_length                       ; fs_get_field_type_length.restype = ctypes.c_int
fs_get_field_type = wrapper.fs_get_field_type
fs_fields_at_savepoint = wrapper.fs_fields_at_savepoint                           ; fs_fields_at_savepoint.restype = ctypes.c_int
fs_fields_at_savepoint_name_lengths = wrapper.fs_fields_at_savepoint_name_lengths
fs_fields_at_savepoint_names = wrapper.fs_fields_at_savepoint_names
fs_register_field = wrapper.fs_register_field
fs_field_exists = wrapper.fs_field_exists                                         ; fs_field_exists.restype = ctypes.c_int
fs_get_element_size = wrapper.fs_get_element_size                                 ; fs_get_element_size.restype = ctypes.c_int
fs_get_field_size = wrapper.fs_get_field_size
fs_add_field_metainfo_b = wrapper.fs_add_field_metainfo_b
fs_add_field_metainfo_i = wrapper.fs_add_field_metainfo_i
fs_add_field_metainfo_f = wrapper.fs_add_field_metainfo_f
fs_add_field_metainfo_d = wrapper.fs_add_field_metainfo_d
fs_add_field_metainfo_s = wrapper.fs_add_field_metainfo_s
fs_create_savepoint = wrapper.fs_create_savepoint                                 ; fs_create_savepoint.restype = ctypes.c_void_p
fs_duplicate_savepoint = wrapper.fs_duplicate_savepoint                           ; fs_duplicate_savepoint.restype = ctypes.c_void_p
fs_destroy_savepoint = wrapper.fs_destroy_savepoint
fs_reinitialize_savepoint = wrapper.fs_reinitialize_savepoint
fs_savepoint_metainfo_size = wrapper.fs_savepoint_metainfo_size                   ; fs_savepoint_metainfo_size.restype = ctypes.c_int
fs_savepoint_name_length = wrapper.fs_savepoint_name_length                       ; fs_savepoint_name_length.restype = ctypes.c_int
fs_savepoint_get_name = wrapper.fs_savepoint_get_name
fs_savepoint_key_lengths = wrapper.fs_savepoint_key_lengths
fs_savepoint_get_keys = wrapper.fs_savepoint_get_keys
fs_savepoint_get_types = wrapper.fs_savepoint_get_types
fs_get_savepoint_metainfo_b = wrapper.fs_get_savepoint_metainfo_b
fs_get_savepoint_metainfo_i = wrapper.fs_get_savepoint_metainfo_i
fs_get_savepoint_metainfo_f = wrapper.fs_get_savepoint_metainfo_f
fs_get_savepoint_metainfo_d = wrapper.fs_get_savepoint_metainfo_d
fs_get_savepoint_metainfo_s = wrapper.fs_get_savepoint_metainfo_s
fs_add_savepoint_metainfo_b = wrapper.fs_add_savepoint_metainfo_b
fs_add_savepoint_metainfo_i = wrapper.fs_add_savepoint_metainfo_i
fs_add_savepoint_metainfo_f = wrapper.fs_add_savepoint_metainfo_f
fs_add_savepoint_metainfo_d = wrapper.fs_add_savepoint_metainfo_d
fs_add_savepoint_metainfo_s = wrapper.fs_add_savepoint_metainfo_s
fs_read_field = wrapper.fs_read_field
fs_write_field = wrapper.fs_write_field
fs_compute_strides = wrapper.fs_compute_strides
fs_field_exists_at_savepoint = wrapper.fs_field_exists_at_savepoint               ; fs_field_exists_at_savepoint.restype = ctypes.c_int


def extract_string(string):
    val = string.encode('ascii') if type(string) == str else string
    return ctypes.c_char_p(val), len(val)

booltypes = (bool, np.bool, )
inttypes = (int, np.int8, np.int16, np.int32, np.int64, np.uint8, np.uint16, np.uint32, np.uint64, )
floattypes = (np.float32, )
doubletypes = (float, np.float64, )
stringtypes = (bytes, str, )

class fieldinfo(object):
    def __init__(self, name, datatype, bytes_per_element, rank,
            isize, jsize, ksize, lsize,
            iminushalo, iplushalo, jminushalo, jplushalo,
            kminushalo, kplushalo, lminushalo, lplushalo):
        self._name = name
        self._datatype = datatype
        self._bytes_per_element = bytes_per_element
        self._rank = rank
        self._isize = isize
        self._jsize = jsize
        self._ksize = ksize
        self._lsize = lsize
        self._iminushalo = iminushalo
        self._iplushalo = iplushalo
        self._jminushalo = jminushalo
        self._jplushalo = jplushalo
        self._kminushalo = kminushalo
        self._kplushalo = kplushalo
        self._lminushalo = lminushalo
        self._lplushalo = lplushalo

    def __repr__(self):
        return '<field ' + '{0}({1}x{2}x{3}x{4})'.format(self._name, self._isize, self._jsize, self._ksize, self._lsize) + '>'

    def __str__(self):
        ret = 'field ' + self.name + '\n'
        ret += ' -- datatype : ' + self._datatype + '\n'
        ret += ' -- bytes_per_element : ' + str(self._bytes_per_element) + '\n'
        ret += ' -- rank : ' + str(self._rank) + '\n'
        ret += ' -- isize : ' + str(self._isize) + '\n'
        ret += ' -- jsize : ' + str(self._jsize) + '\n'
        ret += ' -- ksize : ' + str(self._ksize) + '\n'
        ret += ' -- lsize : ' + str(self._lsize) + '\n'
        ret += ' -- iminushalo : ' + str(self._iminushalo) + '\n'
        ret += ' -- iplushalo : ' + str(self._iplushalo) + '\n'
        ret += ' -- jminushalo : ' + str(self._jminushalo) + '\n'
        ret += ' -- jplushalo : ' + str(self._jplushalo) + '\n'
        ret += ' -- kminushalo : ' + str(self._kminushalo) + '\n'
        ret += ' -- kplushalo : ' + str(self._kplushalo) + '\n'
        ret += ' -- lminushalo : ' + str(self._lminushalo) + '\n'
        ret += ' -- lplushalo : ' + str(self._lplushalo) + '\n'
        return ret

    @property
    def name(self):
        return self._name

    @property
    def datatype(self):
        return self._datatype

    @property
    def bytes_per_element(self):
        return self._bytes_per_element

    @property
    def rank(self):
        return self._rank

    @property
    def isize(self):
        return self._isize

    @property
    def jsize(self):
        return self._jsize

    @property
    def ksize(self):
        return self._ksize

    @property
    def lsize(self):
        return self._lsize

    @property
    def iplushalo(self):
        return self._iplushalo

    @property
    def jplushalo(self):
        return self._jplushalo

    @property
    def kplushalo(self):
        return self._kplushalo

    @property
    def lplushalo(self):
        return self._lplushalo

    @property
    def iminushalo(self):
        return self._iminushalo

    @property
    def jminushalo(self):
        return self._jminushalo

    @property
    def kminushalo(self):
        return self._kminushalo

    @property
    def lminushalo(self):
        return self._lminushalo

    @property
    def shape(self):
        return (self._isize, self._jsize, self._ksize, self._lsize)


class savepoint(object):
    def __init__(self, **kwargs):
        if 'sp' in kwargs and ('name' in kwargs or 'metainfo' in kwargs):
            raise AttributeError("Giving name and pointer is not allowed")

        if 'sp' in kwargs:
            # Use existing savepoint
            self.savepoint = kwargs['sp']

        if 'name' in kwargs:
            # Extract string and create savepoint
            n, nlength = extract_string(kwargs['name'])
            self.savepoint = ctypes.c_void_p(fs_create_savepoint(n, nlength))

        if 'metainfo' in kwargs:
            for key, value in kwargs['metainfo'].items():
                self.add_metainfo(key, value)

    def __del__(self):
        fs_destroy_savepoint(self.savepoint)

    def __repr__(self):
        return '<savepoint {0}>'.format(self.name)

    def __str__(self):
        return self.name + '[ ' + ' '.join(['{0}:{1}'.format(*i) for i in self.metainfo.items()]) + ' ]'

    @property
    def name(self):
        namelength = fs_savepoint_name_length(self.savepoint)
        namebuffer = ctypes.create_string_buffer(namelength+1)
        fs_savepoint_get_name(self.savepoint, namebuffer)
        return namebuffer.value.decode()

    @property
    def metainfo_tuples(self):
        # Retrieve key lengths
        n_keys = int(fs_savepoint_metainfo_size(self.savepoint))
        key_lengths = (ctypes.c_int*n_keys)()
        fs_savepoint_key_lengths(self.savepoint, key_lengths)

        # Retrieve keys
        keys = ((ctypes.c_char_p)*n_keys)()
        keys[:] = [('\0'*key_lengths[i]).encode() for i in range(n_keys)]
        fs_savepoint_get_keys(self.savepoint, keys)

        data = []
        m = self.metainfo
        for i in range(n_keys):
            keystr = keys[i].decode()
            data.append((keystr, m[keystr]))
        return data

    @property
    def metainfo(self):
        # Retrieve key lengths
        n_keys = int(fs_savepoint_metainfo_size(self.savepoint))
        key_lengths = (ctypes.c_int*n_keys)()
        fs_savepoint_key_lengths(self.savepoint, key_lengths)

        # Retrieve keys
        keys = ((ctypes.c_char_p)*n_keys)()
        keys[:] = [('\0'*key_lengths[i]).encode() for i in range(n_keys)]
        fs_savepoint_get_keys(self.savepoint, keys)

        # Retrieve types
        types = (ctypes.c_int*n_keys)()
        fs_savepoint_get_types(self.savepoint, types)

        metainfo = {}
        for i in range(n_keys):
            keystr = keys[i].decode()
            key, keylength = extract_string(keystr)
            if (types[i] == -1):
                v = ctypes.c_bool()
                f = fs_get_savepoint_metainfo_b(self.savepoint, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -2):
                v = ctypes.c_int()
                f = fs_get_savepoint_metainfo_i(self.savepoint, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -3):
                v = ctypes.c_float()
                f = fs_get_savepoint_metainfo_f(self.savepoint, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -4):
                v = ctypes.c_double()
                f = fs_get_savepoint_metainfo_d(self.savepoint, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] >= 0):
                v = ctypes.create_string_buffer(types[i]+1)
                f = fs_get_savepoint_metainfo_s(self.savepoint, key, keylength, v)
                metainfo[keystr] = v.value.decode()

        return metainfo


    def add_metainfo(self, key, value):
        key, keylength = extract_string(key)

        if type(value) in booltypes:
            fs_add_savepoint_metainfo_b(self.savepoint, key, keylength, ctypes.c_bool(value))
        elif type(value) in inttypes:
            fs_add_savepoint_metainfo_i(self.savepoint, key, keylength, ctypes.c_int32(value))
        elif type(value) in floattypes:
            fs_add_savepoint_metainfo_f(self.savepoint, key, keylength, ctypes.c_float(value))
        elif type(value) in doubletypes:
            fs_add_savepoint_metainfo_d(self.savepoint, key, keylength, ctypes.c_double(value))
        elif type(value) in stringtypes:
            value, valuelength = extract_string(value)
            fs_add_savepoint_metainfo_s(self.savepoint, key, keylength, value, valuelength)
        else:
            raise AttributeError("Error: type of value not supported")


class serializer(object):
    def __init__(self, directory, prefix, openmode):
        # Extract open mode
        openmode = openmode.lower()
        if len(openmode) != 1 or not openmode in 'rwa':
            raise ValueError("openmode can be one of 'r', 'w' or 'a'")

        # Extract strings
        d, dlength = extract_string(directory)
        p, plength = extract_string(prefix)

        self.serializer = ctypes.c_void_p(fs_create_serializer(d, dlength, p, plength, ord(openmode)))

    def __del__(self):
        fs_destroy_serializer(self.serializer)

    @property
    def savepoints(self):
        n_sps = int(fs_savepoints(self.serializer))
        return [savepoint(sp=ctypes.c_void_p(fs_get_savepoint(self.serializer, i))) for i in range(n_sps)]

    def fields_at_savepoint(self, savepoint):
        n_fields = int(fs_fields_at_savepoint(self.serializer, savepoint.savepoint))
        name_lengths = (ctypes.c_int*n_fields)()
        fs_fields_at_savepoint_name_lengths(self.serializer, savepoint.savepoint, name_lengths)

        names = ((ctypes.c_char_p)*n_fields)()
        names[:] = [('\0'*name_lengths[i]).encode() for i in range(n_fields)]
        fs_fields_at_savepoint_names(self.serializer, savepoint.savepoint, names)

        return [n.decode() for n in names]


    def load_field(self, name, savepoint):
        fieldinfo = self._fieldinfo(name)
        field = np.ndarray(shape=fieldinfo.shape, dtype=type2dtype(fieldinfo.datatype, fieldinfo.bytes_per_element))

        # Extract strides
        strides = [ctypes.c_int(field.data.strides[i]) if i < len(field.strides) else 0 for i in range(4)]

        name, namelength = extract_string(name)
        fs_read_field(self.serializer, savepoint.savepoint, name, namelength,
                ctypes.c_void_p(field.ctypes.data), strides[0], strides[1], strides[2], strides[3])

        return field

    def save_field(self, name, field, savepoint):
        if name not in self.fieldnames:
            self.register_field(name, field)

        name, namelength = extract_string(name)

        # Extract strides
        strides = [field.strides[i] if i < len(field.strides) else 0 for i in range(4)]

        fs_write_field(self.serializer, savepoint.savepoint, name, namelength,
                field.ctypes.data, strides[0], strides[1], strides[2], strides[3])

    def register_field(self, name, field, **kwargs):
        name, namelength = extract_string(name)
        dtype, dtypelength = extract_string(field.dtype.name)
        size = [field.shape[i] if i < len(field.shape) else 1 for i in range(4)]

        # Extract halo
        iminushalo = kwargs.get('iminushalo', 0)
        iplushalo = kwargs.get('iplushalo', 0)
        jminushalo = kwargs.get('jminushalo', 0)
        jplushalo = kwargs.get('jplushalo', 0)
        kminushalo = kwargs.get('kminushalo', 0)
        kplushalo = kwargs.get('kplushalo', 0)
        lminushalo = kwargs.get('lminushalo', 0)
        lplushalo = kwargs.get('lplushalo', 0)

        # Call C routine
        fs_register_field(self.serializer, name, namelength,
                dtype, dtypelength, field.dtype.itemsize,
                size[0], size[1], size[2], size[3],
                iminushalo, iplushalo, jminushalo, jplushalo,
                kminushalo, kplushalo, lminushalo, lplushalo
                )

    @property
    def fieldnames(self):
        n_fields = fs_fields(self.serializer)

        if n_fields == 0:
            return []

        # Get name lengths
        name_lengths = (ctypes.c_int*n_fields)()
        fs_fieldname_lengths(self.serializer, name_lengths)
        name_length = max(name_lengths)+1

        # Get field names
        names = ((ctypes.c_char_p)*n_fields)()
        names[:] = [('\0'*name_length).encode() for i in range(n_fields)]
        fs_get_fieldnames(self.serializer, names)

        return [n.decode() for n in names]

    @property
    def fieldinfos(self):
        return [self._fieldinfo(f) for f in self.fieldnames]

    @property
    def metainfo(self):
        # Retrieve key lengths
        n_keys = int(fs_serializer_metainfo_size(self.serializer))
        key_lengths = (ctypes.c_int*n_keys)()
        fs_serializer_metainfo_key_lengths(self.serializer, key_lengths)

        # Retrieve keys
        keys = ((ctypes.c_char_p)*n_keys)()
        keys[:] = [('\0'*key_lengths[i]).encode() for i in range(n_keys)]
        fs_serializer_metainfo_get_keys(self.serializer, keys)

        # Retrieve types
        types = (ctypes.c_int*n_keys)()
        fs_serializer_metainfo_get_types(self.serializer, types)

        metainfo = {}
        for i in range(n_keys):
            keystr = keys[i].decode()
            key, keylength = extract_string(keystr)
            if (types[i] == -1):
                v = ctypes.c_bool()
                f = fs_get_serializer_metainfo_b(self.serializer, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -2):
                v = ctypes.c_int()
                f = fs_get_serializer_metainfo_i(self.serializer, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -3):
                v = ctypes.c_float()
                f = fs_get_serializer_metainfo_f(self.serializer, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] == -4):
                v = ctypes.c_double()
                f = fs_get_serializer_metainfo_d(self.serializer, key, keylength, ctypes.pointer(v))
                metainfo[keystr] = v.value
            elif (types[i] >= 0):
                v = ctypes.create_string_buffer(types[i]+1)
                f = fs_get_serializer_metainfo_s(self.serializer, key, keylength, v)
                metainfo[keystr] = v.value.decode()

        return metainfo

    def add_metainfo(self, key, value):
        key, keylength = extract_string(key)

        if type(value) in booltypes:
            fs_add_serializer_metainfo_b(self.serializer, key, keylength, ctypes.c_bool(value))
        elif type(value) in inttypes:
            fs_add_serializer_metainfo_i(self.serializer, key, keylength, ctypes.c_int32(value))
        elif type(value) in floattypes:
            fs_add_serializer_metainfo_f(self.serializer, key, keylength, ctypes.c_float(value))
        elif type(value) in doubletypes:
            fs_add_serializer_metainfo_d(self.serializer, key, keylength, ctypes.c_double(value))
        elif type(value) in stringtypes:
            value, valuelength = extract_string(value)
            fs_add_serializer_metainfo_s(self.serializer, key, keylength, value, valuelength)
        else:
            raise AttributeError("Error: type of value not supported")


    def add_field_metainfo(self, fieldname, key, value):
        fieldname, fieldnamelength = extract_string(fieldname)
        key, keylength = extract_string(key)

        if type(value) in booltypes:
            fs_add_field_metainfo_b(self.serializer, fieldname, fieldnamelength, key, keylength, ctypes.c_bool(value))
        elif type(value) in inttypes:
            fs_add_field_metainfo_i(self.serializer, fieldname, fieldnamelength, key, keylength, ctypes.c_int32(value))
        elif type(value) in floattypes:
            fs_add_field_metainfo_f(self.serializer, fieldname, fieldnamelength, key, keylength, ctypes.c_float(value))
        elif type(value) in doubletypes:
            fs_add_field_metainfo_d(self.serializer, fieldname, fieldnamelength, key, keylength, ctypes.c_double(value))
        elif type(value) in stringtypes:
            value, valuelength = extract_string(value)
            fs_add_field_metainfo_s(self.serializer, fieldname, fieldnamelength, key, keylength, value, valuelength)
        else:
            raise AttributeError("Error: type of value not supported")

    def _fieldinfo(self, fieldname):
        name, namelength = extract_string(fieldname)

        # Get integer values
        bytes_per_element = ctypes.c_int()
        isize = ctypes.c_int()
        jsize = ctypes.c_int()
        ksize = ctypes.c_int()
        lsize = ctypes.c_int()
        iminushalo = ctypes.c_int()
        jminushalo = ctypes.c_int()
        kminushalo = ctypes.c_int()
        lminushalo = ctypes.c_int()
        iplushalo = ctypes.c_int()
        jplushalo = ctypes.c_int()
        kplushalo = ctypes.c_int()
        lplushalo = ctypes.c_int()

        fs_get_fieldinfo(self.serializer, name, namelength,
            ctypes.pointer(bytes_per_element),
            ctypes.pointer(isize), ctypes.pointer(jsize), ctypes.pointer(ksize), ctypes.pointer(lsize),
            ctypes.pointer(iminushalo), ctypes.pointer(iplushalo), ctypes.pointer(jminushalo), ctypes.pointer(jplushalo),
            ctypes.pointer(kminushalo), ctypes.pointer(kplushalo), ctypes.pointer(lminushalo), ctypes.pointer(lplushalo)
            )

        bytes_per_element = bytes_per_element.value
        isize = isize.value
        jsize = jsize.value
        ksize = ksize.value
        lsize = lsize.value
        iminushalo = iminushalo.value
        jminushalo = jminushalo.value
        kminushalo = kminushalo.value
        lminushalo = lminushalo.value
        iplushalo = iplushalo.value
        jplushalo = jplushalo.value
        kplushalo = kplushalo.value
        lplushalo = lplushalo.value

        # Get type
        datatype_length = fs_get_field_type_length(self.serializer, name, namelength)
        datatype = ctypes.create_string_buffer(datatype_length+1)
        fs_get_field_type(self.serializer, name, namelength, datatype)

        # Get rank
        rank = (1 if isize > 1 else 0) + (1 if jsize > 1 else 0) + (1 if ksize > 1 else 0) + (1 if lsize > 1 else 0)

        return fieldinfo(fieldname, datatype.value.decode(), bytes_per_element, rank,
                         isize, jsize, ksize, lsize,
                         iminushalo, iplushalo, jminushalo, jplushalo,
                         kminushalo, kplushalo, lminushalo, lplushalo
                    )

