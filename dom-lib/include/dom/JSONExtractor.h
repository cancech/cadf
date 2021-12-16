#ifndef CAMB_NETWORK_JSON_JSONEXTRACTOR_H_
#define CAMB_NETWORK_JSON_JSONEXTRACTOR_H_

#include "dom/JSONPODValue.h"
#include "dom/JSONNode.h"

namespace camb::dom {

    /**
     * Allows for easy extraction of data from within the JSON tree. Does not allow for any form of exploration of the
     * JSON data, and it is assumed that client code making use of this already know the layout of the data.
     */
    class JSONExtractor {
        public:
            /**
             * CTOR - no memory management is performed, it is the responsibility of the caller to manage any memory allocated to the root
             *
             * @param *root const JSONValue pointer to the root of the JSON tree
             */
            JSONExtractor(const JSONValue *root);

            /**
             * DTOR
             */
            virtual ~JSONExtractor() = default;

            /**
             * Retrieve a child node from under the root. No checking is performed to determine whether the child node is a value or a subnode.
             * Note that this will only work is there is exactly a single child of the specified name available. If a child node by that name
             * does not exist, or there is more than one, an exception will be thrown.
             *
             * @throws JSONTreeException if there is a problem retrieving the desired child node
             *
             * @param &name const std::string the name of the child node
             * @return const JSONValue* pointer to the child node
             */
            virtual const JSONValue* getChild(const std::string &name) const;

            /**
             * Retrieve a child node from under the specified node. No checking is performed to determine whether the child node is a value or a subnode.
             * Note that this will only work is there is exactly a single child of the specified name available. If a child node by that name
             * does not exist, or there is more than one, an exception will be thrown.
             *
             * @throws JSONTreeException if there is a problem retrieving the desired child node
             *
             * @param *parent const JSONValue pointer to the node from which to retrieve the child
             * @param &name const std::string the name of the child node
             * @return const JSONValue* pointer to the child node
             */
            virtual const JSONValue* getChild(const JSONValue* parent, const std::string &name) const;

            /**
             * Retrieve all children from under the root. No checking is performed to determine whether the child node is a value or a subnode.
             * All children will be provided as a vector, allowing for any arbitrary number (at least one must be present or an exception will be thrown)
             *
             * @throws JSONTreeException if there is a problem retrieving the desired child node
             * @param &name const std::string the name of the children
             * @return const std::vector<JSONValue*>& reference to the vector containing the children
             */
            virtual const std::vector<JSONValue*>& getChildArray(const std::string &name) const;

            /**
             * Retrieve all children from under the specified node. No checking is performed to determine whether the child node is a value or a subnode.
             * All children will be provided as a vector, allowing for any arbitrary number (at least one must be present or an exception will be thrown)
             *
             * @throws JSONTreeException if there is a problem retrieving the desired child node
             *
             * @param *parent const JSONValue pointer to the node from which to retrieve the children
             * @param &name const std::string the name of the children
             * @return const std::vector<JSONValue*>& reference to the vector containing the children
             */
            virtual const std::vector<JSONValue*>& getChildArray(const JSONValue* parent, const std::string &name) const;

            /**
             * Get a POD value that is registered to the root node.
             * Note that there must be exactly one possible value match, otherwise an exception will be thrown.
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the value
             *
             * @param &name const std::string the name of the value
             * @return T the desired value
             */
            template<typename T>
            T getValue(const std::string &name) const {
                return getValue<T>(m_root, name);
            }

            /**
             * Get a POD value that is registered to the specified node. The process will first attempt to extract the value from itself
             * (if passing in a node that is the desired value), otherwise finds a child node of the specified name and retrieves its value.
             * Note that there must be exactly one possible value match, otherwise an exception will be thrown.
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the value
             *
             * @param *parent const JSONValue pointer to the node where to look for the value
             * @param &name const std::string the name of the value
             * @return T the desired value
             */
            template<typename T>
            T getValue(const JSONValue *parent, const std::string &name) const {
                std::vector<T> values = getValueArray<T>(parent, name);
                if (values.size() == 0)
                    throw JSONTreeException("No value found \"" + parent->getName() + "\":\"" + name + "\"");
                if (values.size() > 1)
                    throw JSONTreeException("Array found when only a single value expected \"" + parent->getName() + "\":\"" + name + "\"");

                return values[0];
            }

            /**
             * Get an array of POD values that are registered to the root node. This will allow for an arbitrary number of values to be
             * retrieved (minimum of 1)
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the values
             *
             * @param &name const std::string the name of the value
             * @return std::vector<T> the desired values
             */
            template<typename T>
            std::vector<T> getValueArray(const std::string &name) const{
                return getValueArray<T>(m_root, name);
            }

            /**
             * Get an array of POD values that are registered to the specified node. This will allow for an arbitrary number of values to be
             * retrieved (minimum of 1). The process will first attempt to extract the value from itself (if passing in a node that is the
             * desired value), otherwise finds a child node of the specified name and retrieves its value.
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the value
             *
             * @param *parent const JSONValue pointer to the node where to look for the values
             * @param &name const std::string the name of the value
             * @return std::vector<T> the desired values
             */
            template<typename T>
            std::vector<T> getValueArray(const JSONValue *parent, const std::string &name) const{
                if (parent == NULL)
                    throw JSONTreeException("Valid parent node required");

                std::vector<T> result;
                if (tryGetValueFromPOD(parent, name, result))
                    return result;
                if (tryGetValueFromNode(parent, name, result))
                    return result;

                throw JSONTreeException("Unable to extract value \"" + name + "\" from parent node \"" + parent->getName() + "\"");
            }

        private:
            // The root of the tree
            const JSONValue *m_root;

            /**
             * Try to get the POD value from the node itself. Will return false if the node is not a value node and true if the value
             * was successfully retrieved from the node.
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the value
             *
             * @param *value const JSONValue pointer to the node where to look for the value
             * @param &name const std::string the name of the value
             * @param &result std::vector where the extracted result should be stored
             * @return bool true if the value was successfully retrieved
             */
            template<typename T>
            bool tryGetValueFromPOD(const camb::dom::JSONValue *value, const std::string &name, std::vector<T>& result) const {
                // Check if this is the appropriate type of node
                const JSONPODValue *pod = dynamic_cast<const camb::dom::JSONPODValue*>(value);
                if (!pod)
                    return false;

                // Make sure that it's the desired node
                if (pod->getName() != name)
                    throw JSONTreeException("Unable to extract value \"" + name + "\" from value \"" + pod->getName() + "\"");

                // Retrieve the value
                result.push_back(pod->getValue<T>());
                return true;
            }

            /**
             * Try to get the POD value from the children of the specified node. Will return false if the node is not a node
             * that can have children.
             *
             * @template T the type of value to retrieve
             * @throws JSONTreeException if there is an issue retrieving the value
             *
             * @param *value const JSONValue pointer to the node whose children should be the desired value
             * @param &name const std::string the name of the value
             * @param &result std::vector where the extracted result should be stored
             * @return bool true if the value was successfully retrieved
             */
            template<typename T>
            bool tryGetValueFromNode(const camb::dom::JSONValue *value, const std::string &name, std::vector<T>& result) const {
                // Make sure that it's the right type of node
                const JSONNode *node = dynamic_cast<const JSONNode*>(value);
                if (!node)
                    return false;

                // Retrieve the values from the children
                for (JSONValue *v: node->getValue(name)) {
                    if (!tryGetValueFromPOD(v, name, result))
                        throw JSONTreeException("Unable to extract value \"" + name + "\" from node \"" + value->getName() + "\"");
                }
                return true;
            }
    };
}

#endif /* CAMB_NETWORK_JSON_JSONEXTRACTOR_H_ */
