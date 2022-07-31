#ifndef DOM_JSONCONVERTER_H_
#define DOM_JSONCONVERTER_H_

#include "dom/DomNode.h"
#include "dom/DomConverter.h"

namespace cadf::dom::json {

    /**
     * DomConverter for JSON.
     */
    class JsonConverter: public DomConverter {

        public:

            static JsonConverter* instance();

            /**
             * Determine the number of characters a JSON representation of the DomNode tree will require
             *
             * @param &root const DomNode root of the tree to calculate
             * @return size_t the number of characters in the generated JSON
             */
            virtual size_t size(const DomNode &root);

            /**
             * Convert the DomNode tree to a JSON string representation
             *
             * @param &root const DomNode root of the tree
             * @return std::string containing the JSON representation
             */
            virtual std::string toString(const DomNode &root);

            /**
             * Generate a DomNode tree based on the specified JSON string
             *
             * @param &jsonString const std::string containing the JSON representation
             * @return DomNode root of the created DomNode tree
             * @throws ParseException if there is a problem with the provided JSON representation
             */
            virtual DomNode fromString(const std::string &jsonString);

        private:
            /**
             * CTOR
             */
            JsonConverter() = default;

            /**
             * DTOR
             */
            virtual ~JsonConverter() = default;

    };
}

#endif /* DOM_JSONCONVERTER_H_ */
