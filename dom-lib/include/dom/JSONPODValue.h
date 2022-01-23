#ifndef CAMB_NETWORK_JSON_JSONPODVALUE_H_
#define CAMB_NETWORK_JSON_JSONPODVALUE_H_

#include "dom/JSONValue.h"

namespace cadf::dom {
    /**
     * JSON Value that represents some plain old data. The data is specified and retrieved via templated means (constructor/method),
     * however it is stored internally as a string. Therefore it is converted to a string on creation, and from on retrieval.
     */
    class JSONPODValue: public JSONValue {
        public:
            /**
             * CTOR - creates a new JSON Value from the specified POD value.
             *
             * @template T - the type of data that is to be stored within (note: must support << and >> operations for conversion to/from string)
             *
             * @param &name const std::string that is to be applied to the value
             * @param &value const T the data that is to be stored within
             */
            template<typename T>
            JSONPODValue(const std::string &name, const T &value) : JSONValue(name) {
                std::ostringstream conv;
                conv << value;
                m_value = conv.str();
            }

            /**
             * DTOR
             */
            virtual ~JSONPODValue() = default;

            /**
             * Retrieve the value stored within.
             *
             * @template T - the type of data that is to be stored within (note: must support << and >> operations for conversion to/from string)
             *
             * @return T the value
             */
            template<typename T>
            T getValue() const {
                T convertedValue;
                std::istringstream(m_value) >> convertedValue;
                return convertedValue;
            }

            /**
             * Get the value stored within as a string.
             *
             * @return std::string representation of the value
             */
            virtual std::string valueToString() const;

        private:
            // The value of the data
            std::string m_value;
            // The delimiter that is to be used to indicate the start/end of the value
            std::string m_delim;
    };

    // Note various specializations for types that require special treatment
    template<>
    JSONPODValue::JSONPODValue<std::string>(const std::string &name, const std::string &value);
    template<>
    std::string JSONPODValue::getValue<std::string>() const;

    template<>
    JSONPODValue::JSONPODValue<bool>(const std::string &name, const bool &value);
    template<>
    bool JSONPODValue::getValue<bool>() const;

    template<>
    JSONPODValue::JSONPODValue<char>(const std::string &name, const char &value);
    template<>
    char JSONPODValue::getValue<char>() const;
}

#endif /* CAMB_NETWORK_JSON_JSONPODVALUE_H_ */
