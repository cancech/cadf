# dom-lib: Document Object Model Library

Allows for working with structured file types as a tree, with each node in the tree being an element within the document, through a simple and convenient API. [cadf::dom::DomNode](include/dom/DomNode.h) is provided to act as both the tree node, and the mechanism through which to build up the tree, as is a pure virtual [cadf::dom::DomConverter](include/dom/DomConverter.h) which provides the API for serializing and deserializing the [DomNode](include/dom/DomNode.h) tree into and from a `std::string` representation.

Note: a concrete implementation of the [DomConverter](include/dom/DomConverter.h) must be provided to handle the conversion of the [DomNode](include/dom/DomNode.h) tree to a specific format (i.e.: JSON).

## cadf::dom::DomNode

To facilite simplicity, the `operator[]` is implemented to allow for building the tree in a simple and straight forward manner.

```C++
cadf::dom::DomNode root;
root["int"] = 123;
root["double"] = 1.23;
root["string"] = "123";
root["array"] = { 1, 2, 3 };
```
where the `std::string` passed into the `operator[]` call becomes the name of the subnode initialized with the provided value (right hand side of the call). Any value type that supports the `operator<<` for conversion to `std::string` can be used (internally the value is stored as a `std::string`). The above example would translate into the following tree

```
      /---------------------[root]--------------------------\
     |                 /----/    \-------\                   |
     |                |                   |                  |
[int = 123]    [double = 1.23]    [string = "123"]    [array = { 1, 2, 3 }]
```
