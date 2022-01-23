#include "dom/JSONPODValue.h"

namespace cadf::dom {
    /**
     * CTOR - special constructor when the value is a string - sets the delimiters to ""
     */
    template<>
    JSONPODValue::JSONPODValue<std::string>(const std::string &name, const std::string &value): JSONValue(name), m_value(value), m_delim("\"") {
    }

    /**
     * CTOR - special constructor when the value is a bool - converts the 1/0 value to true/false
     */
    template<>
    JSONPODValue::JSONPODValue<bool>(const std::string &name, const bool &value): JSONValue(name) {
        if (value)
            m_value = "true";
        else
            m_value = "false";
    }

    /**
     * CTOR - special constructor when the value is a char - converts the char to an int for storage
     */
    template<>
    JSONPODValue::JSONPODValue<char>(const std::string &name, const char &value): JSONValue(name) {
        std::ostringstream conv;
        conv << (int)value;
        m_value = conv.str();
    }

    /**
     * Special retrieval method for string - since the value is already a string, no need to perform any conversion.
     */
    template<>
    std::string JSONPODValue::getValue<std::string>() const {
        return m_value;
    }

    /**
     * Special retrieval method for bool - derive the values from the stored "true/false"
     */
    template<>
    bool JSONPODValue::getValue<bool>() const {
        return m_value == "true";
    }

    /**
     * Special retrieval method for char - convert the stored integer back to a char
     */
    template<>
    char JSONPODValue::getValue<char>() const {
        int val = getValue<int>();
        return (char) val;
    }

    /**
     * Since this represents a single value, simply return the value with any necessary delimiters ("" for string)
     */
    std::string JSONPODValue::valueToString() const {
        return m_delim + m_value + m_delim;
    }
}
