#include "dom/JSONExtractor.h"

namespace camb::dom {

    /*
     * CTOR
     */
    JSONExtractor::JSONExtractor(const JSONValue *root) : m_root(root) {
    }

    /*
     * Get a single child of root
     */
    const JSONValue* JSONExtractor::getChild(const std::string &name) const {
        return getChild(m_root, name);
    }

    /*
     * Get a single child of the node
     */
    const JSONValue* JSONExtractor::getChild(const JSONValue *parent, const std::string &name) const {
        const std::vector<JSONValue*> &subnodes = getChildArray(parent, name);
        if (subnodes.size() == 0)
            throw JSONTreeException("No sub-node found \"" + parent->getName() + "\":\"" + name + "\"");
        if (subnodes.size() > 1)
            throw JSONTreeException("Array found when only a single sub-node expected \"" + parent->getName() + "\":\"" + name + "\"");

        return subnodes[0];
    }

    /*
     * Get all children of a name
     */
    const std::vector<JSONValue*>& JSONExtractor::getChildArray(const std::string &name) const {
        return getChildArray(m_root, name);
    }

    /*
     * Get all children of a name
     */
    const std::vector<JSONValue*>& JSONExtractor::getChildArray(const JSONValue *parent, const std::string &name) const {
        if (parent == NULL)
            throw JSONTreeException("Valid parent node required");

        const JSONNode *node = dynamic_cast<const camb::dom::JSONNode*>(parent);
        if (node == NULL)
            throw JSONTreeException("Cannot retrieve value from parent \"" + parent->getName() + "\" as it is not a node");

        return node->getValue(name);
    }
}
