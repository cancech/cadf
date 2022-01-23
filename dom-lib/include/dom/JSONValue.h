#ifndef CAMB_NETWORK_JSON_JSONVALUE_H_
#define CAMB_NETWORK_JSON_JSONVALUE_H_

#include <sstream>

#include "dom/JSONException.h"

namespace cadf::dom {

    /**
     * The base class to be contained within a JSON tree. This represents a single value that appears within the tree.
     * This base class doesn't contain the value itself (subclasses provide that functionality), however it provides
     * a generic mechanism for processing the JSON tree.
     */
    class JSONValue {
        public:
            /**
             * CTOR
             *
             * @param &name const std::string the name of the value
             */
            JSONValue(const std::string &name);

            /**
             * DTOR
             */
            virtual ~JSONValue() = default;

            /**
             * Get the name of the value.
             *
             * @return std::string the name of the value
             */
            virtual const std::string& getName() const;

            /**
             * Get a string representation of the whole value. This includes both the name and the value itself, in its expected
             * formating.
             *
             * @return std::string ("name":value)
             */
            virtual std::string toString() const;

            /**
             * Get a string representation of just the value (does not include the name).
             *
             * @return std::string (value)
             */
            virtual std::string valueToString() const;

        private:
            // The name of the value
            std::string m_name;
    };

}

#endif /* CAMB_NETWORK_JSON_JSONVALUE_H_ */
