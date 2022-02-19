#include "dom/DomNode.h"

namespace cadf::dom {

    /*
     * Initializes with no stored value
     */
    DomNode::DomNode(): m_type(NONE), m_value("") {
    }

    /*
     * Initialize with a standard string
     */
    DomNode::DomNode(const std::string &val, bool treatAsString): m_value(val) {
        if(treatAsString)
            m_type = STRING;
        else
            m_type = BASIC_VALUE;
    }

    /*
     * Initialize with a C style string
     */
    DomNode::DomNode(const char *val): DomNode(std::string(val)) {
    }

    /*
     * Initialize with a boolean
     */
    DomNode::DomNode(bool val): m_type(BASIC_VALUE) {
       m_value = val ? "true": "false";
    }

    /*
     * Initialize with a char
     */
    DomNode::DomNode(char val): m_type(STRING) {
        std::stringstream ss;
        ss << val;
        m_value = ss.str();
    }

    /*
     * Access a child node
     */
    DomNode& DomNode::operator[](const std::string &name) {
        return m_children[name];
    }

    /*
     * Access a child node
     */
    DomNode& DomNode::operator[](const char *name) {
        return m_children[name];
    }

    /*
     * Access a child node
     */
    const DomNode& DomNode::operator[](const std::string &name) const {
        return m_children.at(name);
    }

    /*
     * Access a child node
     */
    const DomNode& DomNode::operator[](const char *name) const {
        return m_children.at(name);
    }

    /*
     * Get the value as a string
     */
    DomNode::operator std::string() const {
        return m_value;
    }

    /*
     * Get the value as an int
     */
    DomNode::operator int() const {
        return atoi(m_value.c_str());
    }

    /*
     * Get the value as an unsigned int
     */
    DomNode::operator unsigned int() const {
        return (unsigned int) operator int();
    }

    /*
     * Get the value as an long
     */
    DomNode::operator long() const {
        return atol(m_value.c_str());
    }

    /*
     * Get the value as a double
     */
    DomNode::operator double() const {
        return atof(m_value.c_str());
    }

    /*
     * Get the value as a boolean
     */
    DomNode::operator bool() const {
        return m_value == "true";
    }

    /*
     * Get the value as a char
     */
    DomNode::operator char() const {
        if (m_value.empty())
            return '\0';

        return m_value.at(0);
    }

    /*
     * Check if this is a leaf node
     */
    bool DomNode::isLeaf() const {
        return numChildren() == 0;
    }

    /*
     * Check if the node is an array
     */
    bool DomNode::isArray() const {
        return m_type == ARRAY;
    }

    /*
     * Check if the node is a string
     */
    bool DomNode::isString() const {
        return m_type == STRING;
    }

    /*
     * Check if the node is null
     */
    bool DomNode::isNull() const {
        return isLeaf() && !isArray() && !isString() && m_value.size() == 0;
    }

    /*
     * Get the number of children
     */
    size_t DomNode::numChildren() const {
        return m_children.size();
    }

    /*
     * Get the beginning child iterator
     */
    DomNode::ChildIterator DomNode::beginChildren() const {
        return m_children.cbegin();
    }

    /*
     * Get the end child iterator
     */
    DomNode::ChildIterator DomNode::endChildren() const {
        return m_children.cend();
    }

    /*
     * Get the number of elements in the value array
     */
    size_t DomNode::numArrayElements() const {
        return m_arrayElements.size();
    }

    /*
     * Get the beginning array iterator
     */
    DomNode::ArrayIterator DomNode::beginArray() const {
        return m_arrayElements.cbegin();
    }

    /*
     * Get the end array iterator
     */
    DomNode::ArrayIterator DomNode::endArray() const {
        return m_arrayElements.cend();
    }
}
