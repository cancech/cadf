#include "dom/json/JsonParser.h"
#include "dom/DomException.h"

namespace cadf::dom::json {

    /*
     * CTOR
     */
    JsonParser::JsonParser(const std::string &json): m_input(json), m_currIndex(0) {
    }

    /*
     * Parse the string and create a DOM tree
     */
    DomNode JsonParser::parse() {
        if (nextCharSkipSpace() != '{')
            throwExpectedCharException("{");

        return loadNext();
    }

    /*
     * Load the next value pair from the input.
     */
    DomNode JsonParser::loadNext() {
        DomNode parent;
        do {
            // The pair must start with a string (name:value)
            std::string name = loadString();

            // There must be a ":" separating the name and value
            if (nextCharSkipSpace() !=  ':')
                throwExpectedCharException(":");

            // Load the actual value
            parent[name] = loadValue();

            // Continue until no more values ("," denotes another value is coming up)
        } while(nextCharSkipSpace() == ',');
        // Since the above will "consume" the last seen char, return so that we can verify the input properly
        prevChar();

        // Make sure that it is terminated with "}"
        if (nextCharSkipSpace() != '}')
            throwExpectedCharException("}");

        return parent;
    }

    /*
     * Load an array of values.
     */
    DomNode JsonParser::loadArray() {
        // Same as loadNext except that it is just a comma separated list of values all under the previously determined name
        std::vector<DomNode> arrVals;
        do {
            arrVals.push_back(loadValue());
        } while(nextCharSkipSpace() == ',');
        prevChar();

        // Array terminator is "]"
        if (nextCharSkipSpace() != ']')
            throwExpectedCharException("]");

        return DomNode(arrVals);
    }

    /*
     * Load a single value.
     *
     * [ - denotes start of array
     * { - denotes start of subnode
     * " - denotes start of string
     *
     * Otherwise it is just a regular value (but a value *must* be present)
     */
    DomNode JsonParser::loadValue() {
        switch(peakChar()) {
            case '[':
                nextChar();
                return loadArray();
                break;
            case '{':
                nextChar();
                return loadNext();
            case ']':
            case '}':
            case ':':
            case ',':
                // Invalid start for a value
                throwException("value");
                break;
            case '"':
                return DomNode(loadString(), true);
            default:
                std::string loaded = loadUntilNoSpace(",}]");
                prevChar();
                if (loaded == "null")
                    return DomNode();
                return DomNode(loaded, false);
        }

        return DomNode();
    }

    /*
     * Load a string from the input
     */
    std::string JsonParser::loadString() {
        if (nextCharSkipSpace() != '"')
            throwExpectedCharException("\"");

        return loadUntilWithSpace("\"");
    }

    /*
     * Load until a specified terminator has been reached
     */
    std::string JsonParser::loadUntilWithSpace(const std::string &terminators) {
        std::stringstream ss;
        char c = nextChar(false);
        while (!checkIfTerminator(c, terminators)) {
            ss << c;
            c = nextChar();
        }
        return ss.str();
    }

    /*
     * Load until a specified terminator has been reached
     */
    std::string JsonParser::loadUntilNoSpace(const std::string &terminators) {
        std::stringstream ss;
        char c = nextChar(true);

        bool foundInnerSpace = false;
        while (true) {
            if (isspace(c)) {
                foundInnerSpace = true;
            } else if (checkIfTerminator(c, terminators)) {
                break;
            } else {
                if (foundInnerSpace)
                    throwExpectedCharException(terminators);
                ss << c;
            }
            c = nextChar();
        }
        return ss.str();
    }

    /*
     * Check if the character is an expected terminator
     */
    bool JsonParser::checkIfTerminator(char c, const std::string &terminators) {
        for (char term: terminators) {
            if (term == c)
                return true;
        }
        return false;
    }

    /*
     * Get the next char and advance
     */
    char JsonParser::nextChar(bool skipSpace) {
        if (skipSpace)
            return nextCharSkipSpace();

        return m_input.at(m_currIndex++);
    }

    /*
     * Get the next char, skipping any whitespace
     */
    char JsonParser::nextCharSkipSpace() {
        char c = ' ';
        do {
            c = nextChar(false);
        } while (isspace(c));
        return c;
    }

    /*
     * Get the next non-whitespace char without advancing past it.
     */
    char JsonParser::peakChar() {
        while(isspace(m_input.at(m_currIndex)))
            m_currIndex++;
        return m_input.at(m_currIndex);
    }

    /*
     * Return to the previous char.
     */
    void JsonParser::prevChar() {
        m_currIndex--;
    }

    /*
     * Throw an exception.
     */
    void JsonParser::throwException(const std::string &expected) {
        throw ParseException(expected, m_currIndex);
    }

    /*
     * Convert the array of chars to a more print friendly representation
     */
    void JsonParser::throwExpectedCharException(const std::string &expected) {
        std::stringstream ss;
        for (size_t i = 0; i < expected.size(); i++) {
            ss << "'" << expected[i] << "'";
            if (i < expected.size() - 1)
                ss << ", ";
        }
        throwException(ss.str());
    }
}
