# Python STELLA Serializer #

This is currently a work in progress that I want to bring back into STELLA as soon as it's in a reasonable state.

Currently only reads are supported.  

## Using the Library

### Reading

Open the serialization:

    from visualization_framework import *
    ser = Serializer("/scratch/olifu/pollen_case/test_pollen", "Error")

and access the data with the exported dictionary:

    data = ser['TimeIntegratorUnittest.DoRKStage-out']['LargeTimeStep'][0]['RKStageNumber'][1]['pp']

The interface allows removing the halo from the data:

    data = ser['TimeIntegratorUnittest.DoRKStage-out']['LargeTimeStep'][0]['RKStageNumber'][1]['pp', 'inner']

### Visualizing

Visualize the data:

    Visualizer(data, 'pp_in')

## Creating the Shared Library

The shared library is directly created with CMAKE.