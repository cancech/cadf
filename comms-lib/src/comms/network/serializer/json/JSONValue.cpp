#include "comms/network/serializer/json/JSONValue.h"

namespace cadf::comms::json {
    /*
     * CTOR
     */
    JSONValue::JSONValue(const std::string &name) : m_name(name) {
    }

    /*
     * Get the name of the value
     */
    const std::string& JSONValue::getName() const {
        return m_name;
    }

    /*
     * Convert the value to a string representation (including name)
     */
    std::string JSONValue::toString() const {
        return "\"" + getName() + "\"" + ":" + valueToString();
    }

    /*
     * Default implementation that does nothing
     */
    std::string JSONValue::valueToString() const {
        return "";
    }
}
