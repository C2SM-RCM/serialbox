# Python STELLA Serializer #

Currently only reads are supported.  

## Using the Library

### Reading

Open the serialization:

    from Serializer import *
    ser = Serializer("/scratch/olifu/pollen_case/test_pollen", "Error")

and access the data with the exported dictionary:

    data = ser['TimeIntegratorUnittest.DoRKStage-out']['LargeTimeStep'][0]['RKStageNumber'][1]['pp']

The interface allows removing the halo from the data:

    data = ser['TimeIntegratorUnittest.DoRKStage-out']['LargeTimeStep'][0]['RKStageNumber'][1]['pp', 'inner']

### Visualizing

Visualize the data:
    from Visualizer import *
    Visualizer(data, 'pp_in')

## Creating the Shared Library

The shared library is directly created with CMAKE.
