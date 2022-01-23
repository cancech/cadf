#ifndef CAMB_NETWORK_JSON_JSONPARSER_H_
#define CAMB_NETWORK_JSON_JSONPARSER_H_

#include "dom/JSONBuilder.h"

namespace cadf::dom {

    /**
     * Parser to convert an input JSON string into a tree for internal parsing.
     */
    class JSONParser {
        public:
            /**
             * Parses the input string and generates a JSON tree from it. The JSONEXtractor can then be used to extract data from the generated tree.
             *
             * @throws std::out_of_range or cadf::json::JSONParseException if there is an issue parsing the input JSON string into a JSON tree.
             * @param &json const std::string the JSON string to parse
             * @return const JSONValue* the root of the JSON Tree.
             */
            static const JSONValue* parse(const std::string &json);

        private:
            // The input string that is parsed
            const std::string &m_input;
            // The current index of the parse
            size_t m_currIndex;
            // Builder with which to build the JSON Tree
            JSONBuilder m_builder;

            /**
             * CTOR - private to streamline the use of the parse as much as possible. On instantiation will process the input string and generate the tree.
             *
             * @param &json const std::string the JSON string to parse
             */
            JSONParser(const std::string &json);

            /**
             * Get the root of the generated tree. This triggers the processing of the input string, and no other supporting method should be called directly.
             *
             * @throws std::out_of_range or cadf::json::JSONParseException if there is an issue parsing the input JSON string into a JSON tree.
             * @return const JSONValue* the root of the JSON Tree.
             */
            const JSONValue* get();



            /**
             * Load the next key value pair from the input string.
             *
             * @param *parent JSONValue pointer to the parent to which the pair belongs
             */
            void loadNext(JSONValue* parent);

            /**
             * Load an array of values
             *
             * @param *parent JSONValue pointer to the parent to which the array belongs
             * @param &name const std::string the name of the array
             */
            void loadArray(JSONValue *parent, const std::string &name);

            /**
             * Load a single value
             *
             * @param *parent JSONValue pointer to the parent to which the value belongs
             * @param &name const std::string the name of the value
             */
            void loadValue(JSONValue *parent, const std::string &name);

            /**
             * Load a string from the input. String being denoted in the input by being surrounded by ".
             *
             * @return std::string loaded from the input
             */
            std::string loadString();



            /**
             * Load an element from the input until a terminator is reached. The presence of the terminator is validated by the checkEnd function (i.e.: load until checkEnd returns true).
             * Whitespace is treated as a valid part of the input.
             *
             * @param &terminators const std::string array of potential terminator characters
             * @return std::string containing the retrieved element
             */
            std::string loadUntilWithSpace(const std::string &terminators);

            /**
             * Load an element from the input until a terminator is reached. The presence of the terminator is validated by the checkEnd function (i.e.: load until checkEnd returns true).
             * Whitespace is not considered parrtt of the input, so any leading and trailing whitespace of the element are ignored and no whitespace can be present within the element.
             * Therefore: <whitespace>element<whitespace><terminator> is allowed, but elem<whitespace>ent<terminator> is not.
             *
             * @throws JSONParseException if there is whitespace within the element
             * @param &terminators const std::string array of potential terminator characters
             * @return std::string containing the retrieved element
             */
            std::string loadUntilNoSpace(const std::string &terminators);

            /**
             * Check if the character is within the terminator array.
             *
             * @param c char to check
             * @param &terminators const std::string array of potential terminator characters
             */
            bool checkIfTerminator(char c, const std::string &terminators);



            /**
             * Load the next character and advance the input.
             *
             * @param skipSpace bool true if any whitespace characters should be skipped (defaults to false)
             * @return char the next character in the input
             */
            char nextChar(bool skipSpace = false);

            /**
             * Load the next non-whitespace character and advance the input. This will skip any whitespace characters until a non-whitespace character is reached.
             *
             * @return char the next character in the input
             */
            char nextCharSkipSpace();

            /**
             * Peak at the next non-whitespace character without advancing past it. If the current position of the input is a whitespace, it will advance until the first non-whitespace character
             * is seen.
             *
             * @return char the current non-whitespace character
             */
            char peakChar();

            /**
             * Move backwards to the previous character.
             */
            void prevChar();



            /**
             * Throw an exception to indicate that invalid or malformed input was detected.
             *
             * @param &expected const std::string indicating what was expected to be seen in the input at the point when the exception was thrown
             */
            void throwException(const std::string &expected);

            /**
             * Throw an exception to indicate which exact characters were expected to be seen.
             *
             * @param &expected const std::string an array of characters indicating which exact characters were expected
             */
            void throwExpectedCharException(const std::string &expected);
    };
}

#endif /* CAMB_NETWORK_JSON_JSONPARSER_H_ */
