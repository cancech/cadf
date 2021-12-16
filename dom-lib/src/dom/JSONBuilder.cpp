#include "dom/JSONBuilder.h"

namespace camb::dom {

    /*
     * CTOR
     */
    JSONBuilder::JSONBuilder() : m_root(new JSONRootNode()) {

    }

    /*
     * Get the root of the tree
     */
    const JSONValue* JSONBuilder::getRoot() const {
        return m_root;
    }

    /**
     * Create a new node under the root node
     */
    JSONValue* JSONBuilder::createNode(const std::string &name) {
        return createNode(m_root, name);
    }

    /*
     * Create a new node
     */
    JSONValue* JSONBuilder::createNode(JSONValue *parent, const std::string &name) {
        JSONNode *node = new JSONNode(name);
        m_root->addToMemory(node);

        try {
            toNode(parent)->setValue(node);
            return node;
        } catch (std::exception &ex) {
            delete (node);
            throw;
        }
    }
}
