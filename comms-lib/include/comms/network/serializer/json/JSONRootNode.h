#ifndef CAMB_NETWORK_JSON_JSONROOTNODE_H_
#define CAMB_NETWORK_JSON_JSONROOTNODE_H_

#include "comms/network/serializer/json/JSONNode.h"

namespace cadf::comms::json {

    class JSONBuilder;

    /**
     * Special type of node that is designed to be placed at the root of the tree. It is conceptually the same as any other node,
     * other than placing an extra set of {} around itself. Also as a root, since the name of this node is never printed it, it
     * doesn't require/set it, and accordingly it will not allow itself to be added as a child to another node.
     */
    class JSONRootNode: public JSONNode {
        public:
            friend JSONBuilder;

            /**
             * CTOR
             */
            JSONRootNode();

            /**
             * DTOR
             */
            virtual ~JSONRootNode();

            /**
             * Custom toString() that will surround the node with {} and only print the contents of the node
             *
             * @return std::string representation of the node
             */
            virtual std::string toString() const;

        protected:
            /**
             * Add a value to the memory store, that will be cleaned up along with the root node.
             *
             * @param *value JSONValue pointer to the node that the root should clean up
             */
            void addToMemory(JSONValue *value);

        private:
            // Vector containing the additional nodes that should be deleted alongside this one
            std::vector<JSONValue*> m_memory;
    };
}

#endif /* CAMB_NETWORK_JSON_JSONROOTNODE_H_ */
