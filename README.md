amf-cpp
=======
[![Build Status](https://travis-ci.org/Ventero/amf-cpp.png?branch=master)](https://travis-ci.org/Ventero/amf-cpp)

A C++ implementation of the Action Message Format (AMF3)

# License #

This project is licensed under the MIT/X11 license. See [LICENSE](https://github.com/Ventero/amf-cpp/blob/master/LICENSE).

# Documentation #

Both serialization and deserialization of AMF3 objects are implemented. However,
the API is still subject to change, proper documentation will follow once the
API is stabilized.

The general usage pattern for serialization is to first create the desired AMF
object instance (see `src/types/amf*.hpp`) and then serialize it through a
`Serializer` object, from which you can then get the serialized data as
`std::vector<uint8_t>`.

Deserialization of raw AMF3 data can be done through a `Deserializer` object.
Simply pass a pair of iterators or a `std::vector<uint8_t>` to its `.deserialize`
method, and you will receive a generic `AmfItemPtr`, which can be converted to
an AMF object of the correct type.

```C++
// Serialization:
// First, create the serializer.
amf::Serializer serializer;
// Then construct some values and serialize them.
serializer << amf::AmfDouble(3.14159) << amf::AmfInteger(17);
amf::AmfVector<int> vec({ 1, 2, 3 });
serializer << vec;

// Finally, get the serialized data.
// amf::v8 is a typedef for `std::vector<uint8_t>`
amf::v8 data = serializer.data();

// Deserialization:
// Create a deserializer.
amf::Deserializer deserializer;
// Re-use `data` as example input for deserialization.
auto it = data.cbegin();
amf::AmfDouble d = deserializer.deserialize(it, data.cend()).as<amf::AmfDouble>();
amf::AmfInteger i = deserializer.deserialize(it, data.cend()).as<amf::AmfInteger>();
amf::AmfVector<int> v = deserializer.deserialize(it, data.cend()).as<amf::AmfVector<int>>();

// If you only want to deserialize a single object, you can instead pass in the
// vector directly.
amf::AmfDouble d2 = deserializer.deserialize(data).as<amf::AmfDouble>();
```

# Build instructions #

## Linux / OS X / Unix ##

Building this project requires a reasonably recent C++ compiler with (at least partial)
C++11 support (GCC >= 4.6 and Clang >= 3.1 should work), as well as make (probably
GNU make only, this project hasn't been tested with other versions).

To build the library, just run `make` from the project directory, `make 32bit`
explicitly builds a 32bit library. `make test` builds and runs the unit tests.

## Windows ##

Since this project makes heavy use of C++11 features, Visual Studio 2013 or later
is required to compile it.

To build the library, simply open `amf-cpp.sln` and build the `amf-cpp` project.
To run the unit tests, build and run the `test` project.
