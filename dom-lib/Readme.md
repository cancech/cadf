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

The same process can be followed for creating more complex trees

```C++
cadf::dom::DomNode root;
root["1"]["2"]["3"] = 3;
root["1"]["2"]["4"] = 4;
root["1"]["5"] = 5;
root["6"] = 6;
```

would translate into the following tree

```
            [root]
           /      \
         [1]      [6 = 6]
        /   \
       |     \
       |      \
      [2]     [5 = 5]
     /   \
   /       \
[3 = 3]  [4 = 4]
```

## cadf::dom::DomConverter

While a concrete implementation must be provided the [DomConverter](include/dom/DomConverter.h) provides the API through which to convert a [DomNode](include/dom/DomNode.h) tree into a `std::string` representation and back via the following methods:

* `size_t size(const cadf::dom::DomNode &node)` - get the size of the resulting `std::string`
* `std::string toString(const cadf::dom::DomNode &node)` - convert the tree from the specified root into a `std::string`
* `cadf::dom::DomNode fromString(const std::string &dataString)` - convert the specified `std::string` into a tree, returning the root of it

Meaning that it can be used as such

```C++
void converter(cadf::dom::DomConverter *coverter, const cadf::dom::DomNode &root) {
    std::string stringRep = converter->toString(root);
    cadf::dom::DomNode newRoot = converter->fromString(stringRep);
}
```

Note: while `size(const cadf::dom::DomNode &node)` will be dependent on the specific implementation of the [DomConverter](include/dom/DomConverter.h), it will most likely be a fairly expensive operation. In all likelihood, comparable in complexity to simply performing the conversion to `std::string` and checking the size of it.

## Concrete Converters

It is trivial to create custom converters by simply extending the [DomConverter](include/dom/DomConverter.h), there are some which are provided with the library.

### JSON Converter

[cadf::dom::json::JsonConverter](include/dom/json/JsonConverter.h) is provided to allow for converting a [DomNode](include/dom/DomNode.h) to/from JSON. It relies on [cadf::dom::json::JsonParser](include/dom/json/JsonParser.h) to facilitate the reading of a JSON `std::string`, but the API itself is unchanged. The only distinction being, the inclusion of a singleton to facilitate the initialization and access of the [JsonConverter](include/dom/json/JsonConverter.h).

```C++
cadf::dom::json::JsonConverter *converter = cadf::dom::json::JsonConverter::instance();
std::string stringRep = converter->toString(myRoot);
cadf::dom::DomRoot newRoot = converter->fromString(stringRep);
```
