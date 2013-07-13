amf-cpp
=======
[![Build Status](https://travis-ci.org/Ventero/amf-cpp.png?branch=master)](https://travis-ci.org/Ventero/amf-cpp)

A C++ implementation of the Action Message Format (AMF3)

# License #

This project is licensed under the MIT/X11 license. See [LICENSE](https://github.com/Ventero/amf-cpp/blob/master/LICENSE).

# Documentation #

So far, only serialization is supported, and the serialization API is still subject
to change. Proper documentation will follow once the API is stabilized.

If you want to try the serialization, simply create the AMF objects (see `types/amf*.hpp`)
and insert them into a `Serializer` object. To get the serialized data as `std::vector<u8>`,
call the `.data()` member function.

```C++
Serializer serializer;
serializer << AmfDouble(3.14159) << AmfInteger(17);
AmfVector<int> vec({ 1, 2, 3 });
serializer << vec;

std::vector<u8> data = serializer.data();
```

# Build instructions #

## Linux / Unix ##

Building this project requires a reasonably recent C++ compiler with (at least partial)
C++11 support (GCC >= 4.6 and Clang >= 3.1 should work), as well as make (probably
GNU make, this wasn't tested with any other versions).

To build the library, just run `make` from the project directory, `make 32bit`
explicitly builds a 32bit library. `make test` builds and runs the unit tests.

## Windows ##

Since this project makes heavy use of C++11 features, the Visual Studio 2013
preview is required to compile it.

To build this project, simply open `amf-cpp.sln` and build the `amf-cpp` project.
To run the unit tests, build and run the `test` project.
