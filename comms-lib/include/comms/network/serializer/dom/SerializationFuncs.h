#ifndef CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_
#define CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_

#include <sstream>

#include "comms/network/Buffer.h"
#include "dom/DomNode.h"

namespace cadf::comms::dom {

    /**
     * Build a DOM tree representation of the specified data. No default implementation is provided.
     * A concrete implementation must be provided for each data type (i.e.: struct) that is to be
     * contained within a message.
     *
     * @template class T the type of data that is to be converted
     * @param &data const T reference to the specific instance to be converted
     * @return cadf::dom::DomNode the root of the tree representing the data
     */
    template<class T>
    cadf::dom::DomNode buildTree(const T &data);

    /**
     * Load the data that is stored within the tree. No default implementation is provided.
     * A concrete implementation must be provided for each data type (i.e.: struct) that is to be
     * contained within a message.
     *
     * @template class T that is to be loaded from the tree
     * @param &root const cadf::dom::DomNode at the root of the (sub)tree from which to load the data
     * @return T containing the data from the tree
     */
    template<class T>
    T loadFromTree(const cadf::dom::DomNode &root);
}

#endif /* CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_ */
