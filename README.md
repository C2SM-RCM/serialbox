# SerialBox

<img src="doc/logo.png" width="200" height="150" />

# Introduction

The SerialBox is a set of tools around serialization / deserialization of data. 
The box is used in several projects for building validation frameworks against reference runs.
This is useful in the scope of rewrite of large codes, or when porting codes to multiple computing architectures. 
As an example, porting scientific codes to graphical processing units, that require continuous validation against the existing x86 code.

# Structure

* src/ : It the main core of the SerialBox, written in C++, which provides
  * C++ functionality that can be used to serialize data from a reference run. 
  * Interfaces for deserializing into memory data structures
  * Control the metadata of the fields being serialized/deserialized. 
  * src/utils: logger and other C++ utilities
  * wrapper: a C wrapper that allows to interoperate the core C++ functionality from other languages like Fortran
* doc/ : latex documentation, tutorials and presentations
* python/ : contains various python tools based on the SerialBox format, namely
  * pp_ser: a tiny DSL/parser that facilitates inserting SerialBox statements into your Fortran Code. 
  * Visualizer: a python tool that helps visualizing serialized data
* tools/ : various tools used to validation, dumping, converting or comparing of multiple serialized runs, etc. 
* fortran/ : fortran interfaces to the SerialBox C++ functionality
 
# Building

simply ```mkdir build; cd build; cmake ../; make [install]```
for building the documentation ```make doc```
