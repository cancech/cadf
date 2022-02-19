#include "dom/JsonConverter.h"
#include "dom/JsonParser.h"

namespace cadf::dom::json {

    JsonConverter* JsonConverter::instance() {
        static JsonConverter myInstance;
        return &myInstance;
    }


    std::string getValueString(const DomNode &node) {
        if (node.isNull())
            return "null";
        std::string padding = node.isString() ? "\"" : "";
        return padding + node.operator std::string() + padding;
    }

    /*
     * Determine the size of the JSON representation of the tree
     */
    size_t JsonConverter::size(const DomNode &node) {
        size_t mySize = 0;

        if(node.isArray()) {
            mySize = 2 + node.numArrayElements() - 1;
            for(DomNode::ArrayIterator it = node.beginArray(); it != node.endArray(); ++it)
                mySize += size(*it);
        } else if (node.isLeaf()) {
            mySize = getValueString(node).size();
        } else {
            mySize = 2 + node.numChildren() - 1;
            for(DomNode::ChildIterator it = node.beginChildren(); it != node.endChildren(); ++it)
                mySize += it->first.size() + 2 + 1 + size(it->second);
        }

        return mySize;
    }

    /*
     * Convert the tree to a JSON string
     */
    std::string JsonConverter::toString(const DomNode &node) {
        std::string conv = "";

        if(node.isArray()) {
            conv = "[";
            size_t remaining = node.numArrayElements() - 1;
            for (DomNode::ArrayIterator it = node.beginArray(); it != node.endArray(); ++it) {
              conv += toString(*it);
              if (remaining > 0) {
                  remaining--;
                  conv += ",";
              }
            }
            conv += "]";
        } else if (node.isLeaf()) {
            conv = getValueString(node);
        } else {
            conv = "{";
            size_t remaining = node.numChildren() - 1;
            for (DomNode::ChildIterator it = node.beginChildren(); it != node.endChildren(); ++it) {
                conv += "\"" + it->first + "\":" + toString(it->second);
                if (remaining > 0) {
                    remaining--;
                    conv += ",";
                }
            }
            conv += "}";
        }

        return conv;
    }

    /*
     * Create a tree from the JSON string
     */
    DomNode JsonConverter::fromString(const std::string &jsonString) {
        JsonParser parser(jsonString);
        return parser.parse();
    }
}
