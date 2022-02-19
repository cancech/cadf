#ifndef DOM_DOMCONVERTER_H_
#define DOM_DOMCONVERTER_H_

#include "dom/DomNode.h"

namespace cadf::dom {

    /**
     * Interface for converters generating string representations of DomNode trees
     * and vice-versa. To be implemented by a specific Dom type to handle the conversion
     * for the exact syntax.
     */
    class DomConverter {

        public:
            /**
             * DTOR
             */
            virtual ~DomConverter() = default;

            /**
             * Determine the size (number of characters) of the converted DomNode.
             * This includes all data contained within, including any nested and child nodes
             * of the specified node (i.e.: if this is the root, the size of the entire tree
             * will be returned, otherwise the subtree from the specified node).
             *
             * @param &node const DomNode from where to start the count
             * @return size_t the number of characters in the string representation
             */
            virtual size_t size(const DomNode &node) = 0;

            /**
             * Convert the specified node (including any nested and child nodes) to a string
             * representation.
             *
             * @param &node const DomNode to convert
             * @return std::string the result of the conversion
             */
            virtual std::string toString(const DomNode &node) = 0;

            /**
             * Generate a tree of DomNodes to reflect the data contained within the
             * specified string.
             *
             * @param &dataString const std::string containing the string representation to convert
             * @return DomNode the root of the converted DOM tree
             * @throws ParseException if there is a problem performing the conversion
             */
            virtual DomNode fromString(const std::string &dataString) = 0;
    };
}

#endif /* DOM_DOMCONVERTER_H_ */
