#include "dom/JSONParser.h"
#include "dom/JSONException.h"

#include <sstream>
#include <cctype>

namespace camb::dom {
    /*
     * Create the parser and retrieve the root
     */
    const JSONValue* JSONParser::parse(const std::string &json) {
        JSONParser parser(json);
        return parser.get();
    }

    /*
     * CTOR
     */
    JSONParser::JSONParser(const std::string &json): m_input(json), m_currIndex(0) {
    }

    /*
     * Get the root of the created tree.
     */
    const JSONValue* JSONParser::get() {
        if (nextCharSkipSpace() != '{')
            throwExpectedCharException("{");
        loadNext(m_builder.m_root);
        return m_builder.getRoot();
    }

    /*
     * Load the next value pair from the input.
     */
    void JSONParser::loadNext(JSONValue* parent) {
        do {
            // The pair must start with a string (name:value)
            std::string name = loadString();

            // There must be a ":" separating the name and value
            if (nextCharSkipSpace() !=  ':')
                return;

            // Load the actual value
            loadValue(parent, name);

            // Continue until no more values ("," denotes another value is coming up)
        } while(nextCharSkipSpace() == ',');
        // Since the above will "consume" the last seen char, return so that we can verify the input properly
        prevChar();

        // Make sure that it is terminated with "}"
        if (nextCharSkipSpace() != '}')
            throwExpectedCharException("}");
    }

    /*
     * Load an array of values.
     */
    void JSONParser::loadArray(JSONValue *parent, const std::string &name) {
        // Same as loadNext except that it is just a comma separated list of values all under the previously determined name
        do {
            loadValue(parent, name);
        } while(nextCharSkipSpace() == ',');
        prevChar();

        // Array terminator is "]"
        if (nextCharSkipSpace() != ']')
            throwExpectedCharException("]");
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
    void JSONParser::loadValue(JSONValue *parent, const std::string &name) {
        switch(peakChar()) {
            case '[':
                nextChar();
                loadArray(parent, name);
                break;
            case '{':
                nextChar();
                loadNext(m_builder.createNode(parent, name));
                break;
            case ']':
            case '}':
            case ':':
            case ',':
                // Invalid start for a value
                throwException("value");
                break;
            case '"':
                m_builder.addValue(parent, name, loadString());
                break;
            default:
                m_builder.addValue(parent, name, loadUntilNoSpace(",}]").c_str());
                prevChar();
                break;
        }
    }

    /*
     * Load a string from the input
     */
    std::string JSONParser::loadString() {
        if (nextCharSkipSpace() != '"')
            throwExpectedCharException("\"");

        return loadUntilWithSpace("\"");
    }

    /*
     * Load until a specified terminator has been reached
     */
    std::string JSONParser::loadUntilWithSpace(const std::string &terminators) {
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
    std::string JSONParser::loadUntilNoSpace(const std::string &terminators) {
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
    bool JSONParser::checkIfTerminator(char c, const std::string &terminators) {
        for (char term: terminators) {
            if (term == c)
                return true;
        }
        return false;
    }

    /*
     * Get the next char and advance
     */
    char JSONParser::nextChar(bool skipSpace) {
        if (skipSpace)
            return nextCharSkipSpace();

        return m_input.at(m_currIndex++);
    }

    /*
     * Get the next char, skipping any whitespace
     */
    char JSONParser::nextCharSkipSpace() {
        char c = ' ';
        do {
            c = nextChar(false);
        } while (isspace(c));
        return c;
    }

    /*
     * Get the next non-whitespace char without advancing past it.
     */
    char JSONParser::peakChar() {
        while(isspace(m_input.at(m_currIndex)))
            m_currIndex++;
        return m_input.at(m_currIndex);
    }

    /*
     * Return to the previous char.
     */
    void JSONParser::prevChar() {
        m_currIndex--;
    }

    /*
     * Throw an exception.
     */
    void JSONParser::throwException(const std::string &expected) {
        throw JSONParseException(expected, m_currIndex);
    }

    /*
     * Convert the array of chars to a more print friendly representation
     */
    void JSONParser::throwExpectedCharException(const std::string &expected) {
        std::stringstream ss;
        for (size_t i = 0; i < expected.size(); i++) {
            ss << "'" << expected[i] << "'";
            if (i < expected.size() - 1)
                ss << ", ";
        }
        throwException(ss.str());
    }
}
