# Serialbox Fortran example

This small example shows how to use Serialbox in a Fortran code using the
serialization directives.

The serialization happens in the `m_ser.f90` module. Two small programs use
this module to serialize and deserialize data.

### Build the example
```bash
cmake .
make
```

### Run the example
```bash
./main_producer
./main_consumer
./main_consumer_with_perturb
```
