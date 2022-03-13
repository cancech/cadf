# Overview
CADF - C++ Application Development Framework

A collection of libraries that form a framework for the purpose of developing C++ applications. Some can be used individually, others have dependencies on other portions of the framework. Each individual library will detail its own specifics accordingly. External dependencies are being avoided where possible, with the only expected external dependencies being boost_test and fakeit - both used to power the automated tests verifying framework capabilities.

This genesys of this framework was the realization that while there are many development frameworks and libraries (such as boost and Qt), none of them were explicitely doing exactly what I had wanted. Thus, I started working on a tailor made framework which would do exactly what I needed.

# Libraries
The libaries contained within are as follows:

* [comms-lib](comms-lib) - provides communications across local or distributed nodes
* [di-lib](di-lib) - provides dependency injection capabilities
* [dom-lib](dom-lib) - provides DOM support
* [thread-lib](thread-lib) - provides threading capabilities

Details for each library can be viewed from within each.

# Compiling
The framework is built using cmake, so after cloning to compile simply

```
cmake -B./compile
cd compile
make
make install
```

Note that in-source compiling is disallowed.

Optional cmake flags:

* `-D CMAKE_INSTALL_PREFIX=/path/to/install/location` to install to a non-default location

## Installation
`make install` will install the headers and compiled libraries to the specified (or default) target location. To allow for different versions to be installable on the same system, the following installation path is adhered to:

* install path
  *  include
      *  cadf-version
         *  di
         *  dom
         *  thread
         *  Et Cetera
  * lib
     * cadf-version
