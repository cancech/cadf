#ifndef CAMB_NETWORK_JSON_JSONBUILDER_H_
#define CAMB_NETWORK_JSON_JSONBUILDER_H_

#include "dom/JSONPODValue.h"
#include "dom/JSONRootNode.h"

namespace cadf::dom {

    class JSONParser;

    /**
     * Builder class to help with the creation of JSON Trees
     */
    class JSONBuilder {
        public:
            friend class JSONParser;

            /**
             * CTOR
             *
             * On creation the root node has allocated memory, and this memory must be managed by the caller. The builder
             * will not free this memory.
             */
            JSONBuilder();

            /**
             * DTOR
             */
            virtual ~JSONBuilder() = default;

            /**
             * Get the root of the created tree.
             *
             * This can be called at any time, however there is no need to do so until the entire tree has been created.
             * The root has memory allocated to it, and it is the client code's responsibility to manage it.
             *
             * @return const JSONValue* pointer to the root of the tree. This MUST be freed by the caller
             */
            const JSONValue* getRoot() const;

            /**
             * Create a new under the root node.
             *
             * @param &name const std::string of the new node
             *
             * @return JSONValue* pointer to the created node
             */
            virtual JSONValue* createNode(const std::string &name);

            /**
             * Create a new under an arbitrary node.
             *
             * @param *parent JSONValue pointer to the node under which the new node is to be constructed
             * @param &name const std::string of the new node
             *
             * @return JSONValue* pointer to the created node
             */
            virtual JSONValue* createNode(JSONValue *parent, const std::string &name);

            /**
             * Add a value to the root node.
             *
             * Note: if the value is to be treated as a string, then it MUST be either a std::string variable or the template <std::string> forced.
             * Otherwise it will be treated as a char[numOfElements] array rather than a string.
             *
             * @param &name const std::string the name to assign to the value
             * @param &value const T the data to assign to the value
             */
            template<typename T>
            void addValue(const std::string &name, const T &value) {
                addValue(m_root, name, value);
            }

            /**
             * Add a value to the a node.
             *
             * Note: if the value is to be treated as a string, then it MUST be either a std::string variable or the template <std::string> forced.
             * Otherwise it will be treated as a char[numOfElements] array rather than a string.
             *
             * @param *node JSONValue pointer to the node to which the value is to be added
             * @param &name const std::string the name to assign to the value
             * @param &value const T the data to assign to the value
             */
            template<typename T>
            void addValue(JSONValue *node, const std::string &name, const T &value) {
                JSONPODValue *pod = new JSONPODValue(name, value);
                m_root->addToMemory(pod);

                try {
                    toNode(node)->setValue(pod);
                } catch (std::exception &ex) {
                    delete(pod);
                    throw;
                }
            }

        private:
            // The root of the tree
            JSONRootNode *m_root;

            /**
             * Helper to shorthand the casting of JSONValue* to JSONNode*.
             *
             * @param *value JSONValue to cast
             *
             * @return JSONNode* resulting from the cast
             */
            inline JSONNode* toNode(JSONValue *value) {
                return static_cast<JSONNode*>(value);
            }
    };
}

#endif /* CAMB_NETWORK_JSON_JSONBUILDER_H_ */
