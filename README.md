amf-cpp
=======
[![Build Status](https://travis-ci.org/Ventero/amf-cpp.png?branch=master)](https://travis-ci.org/Ventero/amf-cpp)

A C++ implementation of the Action Message Format (AMF3)

# License #

This project is licensed under the MIT/X11 license. See [LICENSE](https://github.com/Ventero/amf-cpp/blob/master/LICENSE).

# Build instructions #

## Linux / Unix ##

Building this project requires a reasonably recent C++ compiler with (at least partial)
C++11 support (GCC >= 4.6 and Clang >= 3.1 should work), as well as make (probably
GNU make, this wasn't tested with any other versions).

To build the library, just run `make` from the project directory, `make 32bit`
explicitly builds a 32bit library. `make test` builds and runs the unit tests.
