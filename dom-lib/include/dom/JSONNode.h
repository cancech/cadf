#ifndef CAMB_NETWORK_JSON_JSONNODE_H_
#define CAMB_NETWORK_JSON_JSONNODE_H_

#include "dom/JSONValue.h"

#include <vector>
#include <map>

namespace camb::dom {

    /**
     * This is the default node in a JSON tree. This can contain any number of values organized by name. If the value is a simple "POD" then it is treated as
     * a simple value within this node. However the value can also be its own node, in which case the parameters of that node become nested within this one.
     * There can be any number of values stored under a given name. If there are more than one values for a name, they are treated as an array.
     */
    class JSONNode : public JSONValue {
        public:
            /**
             * CTOR
             *
             * @param &name const std::string the name of the node
             */
            JSONNode(const std::string &name);

            /**
             * DTOR
             */
            virtual ~JSONNode() = default;

            /**
             * Set a value to this node. It will be associated within based on its name.
             *
             * @param *value JSONValue pointer to the value that is to be added to this node.
             */
            void setValue(JSONValue *value);

            /**
             * Get all of the values that are stored within the node for a given value name
             *
             * @param &name const std::string the name of the value to retrieve
             * @return const std::vector<JSONValue*>& reference to all of the values stored for the name
             * @throws JSONTreeException if no value by the specified name exists
             */
            const std::vector<JSONValue*>& getValue(const std::string &name) const;

        protected:
            /**
             * Converts the value of the node to a string representation.
             *
             * @return std::string representing the value of the entire node
             */
            virtual std::string valueToString() const;

            /**
             * Converts all of the values stored within the node to a string representation.
             *
             * @return std::string representing all of the values within the node
             */
            virtual std::string allValuesToString() const;

        private:
            // Mapping of all values to their names
            std::map<std::string, std::vector<JSONValue*>> m_values;

            /**
             * Generate a string representation for all of the values of a given name.
             *
             * @param &name const std::string of the values
             * @param &vals const std::vector<JSONValue*> of all values assigned to that name
             */
            std::string valuesToString(const std::string &name, const std::vector<JSONValue*> &vals) const;

            /**
             * Generate a string representation of an array of values for a given name.
             *
             * @param &name const std::string of the values
             * @param &vals const std::vector<JSONValue*> of all values assigned to that name
             */
            std::string arrayValueToString(const std::string &name, const std::vector<JSONValue*> vals) const;
    };
}

#endif /* CAMB_NETWORK_JSON_JSONNODE_H_ */
