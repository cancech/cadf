#include "dom/JSONNode.h"
#include <iostream>

namespace cadf::dom {

    /*
     * CTOR
     */
    JSONNode::JSONNode(const std::string &name) : JSONValue(name) {
    }

    /**
     * Add a value to the node
     */
    void JSONNode::setValue(JSONValue *value) {
        // Make sure that the value is legitimate
        if (value == NULL)
            throw JSONTreeException("Invalid value specified");

        std::string name = value->getName();
        if (name.empty())
            throw JSONTreeException("Values must have a valid name specified (is this a root node?)");

        // Append it the existing values
        m_values[name].push_back(value);
    }

    /*
     * Retrieve the values for a name
     */
    const std::vector<JSONValue*>& JSONNode::getValue(const std::string &name) const {
        // Make sure that the value exists
        auto iter = m_values.find(name);
        if (iter == m_values.end())
            throw JSONTreeException("No value \"" + name + "\" exists within \"" + getName() + "\"");

        return iter->second;
    }

    /*
     * Converting the whole node just simply incorporates the delimiters
     */
    std::string JSONNode::valueToString() const {
        return "{" + allValuesToString() + "}";
    }

    /*
     * Converts each included value in order
     */
    std::string JSONNode::allValuesToString() const {
        std::string result = "";
        size_t remaining = m_values.size() - 1;

        for (std::pair<std::string, const std::vector<JSONValue*>&> p : m_values) {
            result += valuesToString(p.first, p.second);
            if (remaining-- > 0)
                result += ",";
        }

        return result;
    }

    /*
     * Performs the conversion depending on how many values for a given type there are
     */
    std::string JSONNode::valuesToString(const std::string &name, const std::vector<JSONValue*> &vals) const {
        // Make sure that there is at least something present
        if (vals.size() == 0)
            throw JSONTreeException("Invalid parameter \"" + name + "\", it has no values assigned");

        // If there is only a single entry, then proces it "normally"
        if (vals.size() == 1)
            return vals[0]->toString();

        return arrayValueToString(name, vals);
    }

    /*
     * Convert all entries into an array
     */
    std::string JSONNode::arrayValueToString(const std::string &name, const std::vector<JSONValue*> vals) const {
        std::string result = "\"" + name + "\":[";
        size_t remaining = vals.size() - 1;

        for (size_t j = 0; j < vals.size(); j++) {
            // Use "valueToString()" to avoid including the name of the value, as that is only printed once for the whole array
            result += vals[j]->valueToString();
            if (remaining-- > 0)
                result += ",";
        }

        return result + "]";
    }
}
