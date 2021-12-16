#include "dom/JSONRootNode.h"

namespace camb::dom {
    /*
     * CTOR
     */
    JSONRootNode::JSONRootNode() : JSONNode("") {
    }

    /*
     * DTOR
     */
    JSONRootNode::~JSONRootNode() {
        for (JSONValue *val : m_memory)
            delete (val);

        m_memory.clear();
    }

    /*
     * Convert the node (and hence whole tree) to a string representation of itself.
     */
    std::string JSONRootNode::toString() const {
        return "{" + JSONNode::allValuesToString() + "}";
    }

    /*
     * Set the memory store
     */
    void JSONRootNode::addToMemory(JSONValue *value) {
        m_memory.push_back(value);
    }
}
