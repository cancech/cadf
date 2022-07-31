#ifndef DOM_JSONPARSER_H_
#define DOM_JSONPARSER_H_

#include "dom/DomNode.h"

#include <string>

namespace cadf::dom::json {

    class JsonParser {
        public:
            /**
             * CTOR
             *
             * @param &json const std::string containing the JSON representation to parse
             */
            JsonParser(const std::string &json);

            /**
             * Parse the JSON string and create a DOM tree for the data
             *
             * @return DomNode at the root of the DOM tree
             */
            DomNode parse();

        private:
            // The input string that is parsed
            const std::string &m_input;
            // The current index of the parse
            size_t m_currIndex;

            /**
             * Load the next key value pair from the input string.
             *
             * @return DomNode containing the next loaded element
             */
            DomNode loadNext();

            /**
             * Load an array of values
             *
             * @return DomNode containing the loaded array of elements
             */
            DomNode loadArray();

            /**
             * Load a single value
             *
             * @return DomNode containing the loaded value
             */
            DomNode loadValue();

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

#endif /* DOM_JSONPARSER_H_ */
