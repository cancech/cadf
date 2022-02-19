#ifndef DOM_DOMNODE_H_
#define DOM_DOMNODE_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <initializer_list>

namespace cadf::dom {

    /**
     * Representation of a single value in a DOM tree. The node can be either an object (where it contains nested nodes), value (where it
     * contains a single value), or array (where it contain multiple values). The node has no concept of its own name, as that information
     * is stored with the parent. Thus, by definition, the root node has no name.
     */
    class DomNode {

        public:
            /** Helper when iterating through the children of this node */
            typedef typename std::map<std::string, DomNode>::const_iterator ChildIterator;
            /** Helper when iterating through the values of this node's value array */
            typedef typename std::vector<DomNode>::const_iterator ArrayIterator;

            /**
             * CTOR - initializing without any value (null node)
             */
            DomNode();

            /**
             * CTOR - initializing with a string convertible value
             *
             * @param val T the value to store in the node
             * @template T the type of value that is being stored
             */
            template<typename T>
            DomNode(T val): m_type(BASIC_VALUE) {
                std::stringstream ss;
                ss << val;
                m_value = ss.str();
            }

            /**
             * CTOR - initializing with an array of elements to allow from the specified vector
             *
             * @template T the type of values to be stored in the array
             * @param vals std::vector containing the elements
             */
            template<typename T>
            DomNode(const std::vector<T> &vals): m_type(ARRAY), m_value("") {
                for(T v: vals)
                    m_arrayElements.push_back(v);
            }

            /**
             * CTOR - initializing with an array of elements to allow for in-line definition
             *
             * @template T the type of values to be stored in the array
             * @param vals std::intializer_list of the elements
             */
            template<typename T>
            DomNode(std::initializer_list<T> vals): m_type(ARRAY), m_value("") {
                for(T v: vals)
                    m_arrayElements.push_back(v);
            }

            /**
             * CTOR - initializing with a C style string
             *
             * @param *val const char string value
             */
            DomNode(const char *val);

            /**
             * CTOR - initializing with a standard string
             *
             * @param &val const std::string value
             * @param treatAsString bool true (default) is the string is to be treated as a string value
             */
            DomNode(const std::string &val, bool treatAsString = true);

            /**
             * CTOR - initializing with a boolean
             * Booleans are stored internally as true/false
             *
             * @param val bool value
             */
            DomNode(bool val);

            /**
             * CTOR - initializing with a single character
             * chars are stored internally as their string character
             *
             * @param val char value
             */
            DomNode(char val);

            /**
             * DTOR
             */
            virtual ~DomNode() = default;

            /**
             * Access the child node with the given name. Children are dynamically added when they are access for the first time
             *
             * @param &name const std::string of the desired child node.
             */
            DomNode& operator[](const std::string &name);

            /**
             * Access the child node with the given name. Children are dynamically added when they are access for the first time
             *
             * @param *name const char of the desired child node.
             */
            DomNode& operator[](const char *name);

            /**
             * Access the child node with the given name. Children are dynamically added when they are access for the first time
             *
             * @param &name const std::string of the desired child node.
             */
            const DomNode& operator[](const std::string &name) const;

            /**
             * Access the child node with the given name. Children are dynamically added when they are access for the first time
             *
             * @param *name const char of the desired child node.
             */
            const DomNode& operator[](const char *name) const;

            /**
             * Get the value of the node as a string.
             *
             * @return std::string representation of the value
             */
            operator std::string() const;

            /**
             * Get the value of the node as an int.
             *
             * @return int representation of the value
             */
            operator int() const;

            /**
             * Get the value of the node as an unsigned int.
             *
             * @return unsigned int representation of the value
             */
            operator unsigned int() const;

            /**
             * Get the value of the node as a long.
             *
             * @return long representation of the value
             */
            operator long() const;

            /**
             * Get the value of the node as a double.
             *
             * @return double representation of the value
             */
            operator double() const;

            /**
             * Get the value of the node as a boolean.
             *
             * @return bool representation of the value
             */
            operator bool() const;

            /**
             * Get the value of the node as a char. If more than a single character is stored within, only the first character
             * is returned.
             *
             * @return char representation of the value (or first character)
             */
            operator char() const;

            /**
             * Get the values stored within the node's array as the desired type.
             *
             * @template T the type of data to retrieve from the internal array
             * @return std::vector<T> containing the values within the node's array
             */
            template<typename T>
            operator std::vector<T>() const {
                 std::vector<T> values;
                 for (const DomNode &n : m_arrayElements)
                     values.push_back(n);
                 return values;
            }

            /**
             * Check if the node is a leaf in the DOM tree
             *
             * @return true if the node is a leaf (has no children)
             */
            virtual bool isLeaf() const;

            /**
             * Check if the node contains a value array.
             *
             * @return true if the node contains an array of values
             */
            virtual bool isArray() const;

            /**
             * Check of the node contains string data (originally provided as a string)
             *
             * @return true if the node has a string value
             */
            virtual bool isString() const;

            /**
             * Check if the node has a null value. To be considered null, it must be a leaf to which no value was ever applied.
             * Note: an empty string is not a null value
             *
             * @return true if the node contains a null value
             */
            virtual bool isNull() const;

            /**
             * Get the number of children the node has
             *
             * @return size_t the number of children
             */
            size_t numChildren() const;

            /**
             * Get the iterator for the beginning of the children.
             *
             * for (ChildInterator it = domNode.beginChildren(); it != domNode.endChildren(); ++it) {
             *     it->first // The name of the child
             *     it->second // The child node itself
             * }
             *
             * @return the beginning ChildIterator
             */
            ChildIterator beginChildren() const;

            /**
             * Get the iterator for the end of the children
             *
             * @return the end ChildIterator
             */
            ChildIterator endChildren() const;

            /**
             * Get the number of elements in the value array
             *
             * @return size_t the number of elements in the array
             */
            size_t numArrayElements() const;

            /**
             * Get the iterator for the beginning of the array elements
             *
             * for (ArrayIterator it = domNode.beginArray(); it != domNode.endArray(); ++it) {
             *     it->first // DomNode containing the value at the current index of the array
             * }
             *
             * @return the beginning ArrayIterator
             */
            ArrayIterator beginArray() const;

            /**
             * Get the iterator for the end of the array elements
             *
             * @return the end ArrayIterator
             */
            ArrayIterator endArray() const;

        private:
            // Enumeration to indicate what type of value is stored within
            enum VAL_TYPE {
                NONE, ARRAY, STRING, BASIC_VALUE
            };

            /** The type of value stored within the node */
            VAL_TYPE m_type;
            /** The string representation of the value */
            std::string m_value;
            /** The node's children associated to their name */
            std::map<std::string, DomNode> m_children;
            /** The array of values */
            std::vector<DomNode> m_arrayElements;
    };

    /**
     * Helper function to allow for creating a new node with a single subnode value.
     *
     * @template T the type of value to assign to the subnode
     * @param &valueName const std::string the name of the value
     * @param &value T const the actual value
     */
    template<typename T>
    DomNode buildNode(const std::string &valueName, const T &value) {
        DomNode node;
        node[valueName] = value;
        return node;
    }

    /**
     * Helper function to allow inline creation of a node with an array value.
     *
     * @template T the type of value to be assigned to the array
     * @param &valueName const std::string the name of the value
     * @param vals  std::initializer_list<T> the list of values
     */
    template<typename T>
    DomNode buildNode(const std::string &valueName, std::initializer_list<T> vals) {
        DomNode node;
        node[valueName] = vals;
        return node;
    }
}

#endif /* DOM_DOMNODE_H_ */
