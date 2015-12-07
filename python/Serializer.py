#This file is released under terms of BSD license`
#See LICENSE.txt for more information

from serialization import serializer


class Savepoint(dict):
    def __init__(self, ser, savepoint):
        self.savepoint = savepoint
        self.serializer = ser
        self.fields = ser.serializer.fields_at_savepoint(savepoint)

    @staticmethod
    def _reshape_array(array):
        shape = array.shape
        if len(shape) == 4 and shape[3] == 1:
            shape = (shape[0], shape[1], shape[2])
            return array.reshape(shape)
        return array

    @staticmethod
    def _remove_halo(array, info):
        istart = info.iminushalo
        iend = info.isize - info.iplushalo
        jstart = info.jminushalo
        jend = info.jsize - info.jplushalo
        kstart = info.kminushalo
        kend = info.ksize - info.kplushalo
        lstart = info.lminushalo
        lend = info.lsize - info.lminushalo

        return array[istart:iend, jstart:jend, kstart:kend, lstart:lend]

    def _prepare_data(self, field, opt):
        info = self.serializer.fieldinfos[field]
        array = self.serializer.serializer.load_field(field, self.savepoint)

        if opt is None:
            return Savepoint._reshape_array(array)

        if opt == "inner" or opt == "i":
            array = Savepoint._remove_halo(array, info)
            return Savepoint._reshape_array(array)

        raise NameError("Option is not supported: {}, type: {}".format(str(opt), str(type(opt))))

    def keys(self):
        return self.fields

    def items(self):
        for key in self.keys():
            yield (key, self.__getitem__(key))

    def __getitem__(self, arg):
        field = arg
        opt = None
        # Allow multiple arguments savepoint["v_in", "inner"]
        if isinstance(arg, tuple):
            field = arg[0]
            opt = arg[1]
        if field not in self.fields:
            raise KeyError

        return self._prepare_data(field, opt)

    def __repr__(self):
        return "{{ {names} }}".format(names=", ".join(["'{}'".format(a) for a in self.fields]))

# Nested dictionary set
# http://stackoverflow.com/q/13687924/592024
def _nested_set(dic, keys, value):
    for key in keys[:-1]:
        dic = dic.setdefault(key, {})
    dic[keys[-1]] = value

class Serializer(dict):
    def __init__(self, directory=".", prefix="Field", openmode="r"):
        self.serializer = serializer(directory, prefix, openmode)
        fnames = self.serializer.fieldnames
        finfos = self.serializer.fieldinfos

        self.fieldinfos = {}
        data = {}
        for (name, info) in zip(fnames, finfos):
            self.fieldinfos[name] = info
        for sp in self.serializer.savepoints:
            nesting = [sp.name]
            metainfo = sp.metainfo_tuples
            for k, v in metainfo:
                nesting.append(k)
                nesting.append(v)
            s = Savepoint(self, sp)
            _nested_set(data, nesting, s)
        for k, v in data.items():
            dict.__setitem__(self, k, v)

    def __setitem__(self, key, value):
        raise NotImplementedError

    def __repr__(self):
        savepointnames = [k for k, v in self.items()]
        formattednames = ["'{}' = [...]".format(a) for a in savepointnames]
        return "{{ {names} }}".format(names=", ".join(formattednames))
