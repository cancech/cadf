#include "../../../../include/cadf/di/exception/InvalidBeanException.h"

#include <sstream>

#include "cadf/di/Configuration.h"

namespace cadf::di {

    /*
     * Helper functions which translates a vector to a human readable string form. Note, that the type of
     * stored within the vector must support the << operator.
     *
     * @template T the type of value stored within the vector
     *
     * @param v reference to the std::vector to convert to a string
     *
     * @return std::string the string representation of the vector
     */
    template<typename T>
    std::string vectorAsString(const std::vector<T> &v) {
        std::stringstream ss;
        ss << "[";

        for (uint i = 0; i < v.size(); i++) {
            ss << v.at(i);

            if (i < (v.size() - 1))
                ss << ", ";
        }

        ss << "]";

        return ss.str();
    }

    /*
     * Constructor for the BeanDependencyCycleException that converts the cycle vector to a string
     */
    BeanDependencyCycleException::BeanDependencyCycleException(const std::vector<std::string> &cycle) : std::runtime_error("Dependency cycle detected " + vectorAsString(cycle)) {
    }

    /*
     * Constructor for the ConfigurationMissingResources that converts the vector of missing resources to a string
     */
    ConfigurationMissingResourcesException::ConfigurationMissingResourcesException(const std::string configName, const std::vector<std::string> &missing) :
            std::runtime_error("Unable to create Configuration " + configName + " due to missing resources: " + vectorAsString(missing)) {

    }

    /*
     * Constructor that converts the vector of unfulfilled ConfigurationWrapperInterface to string
     */
    ConfigurationInitializationException::ConfigurationInitializationException(const std::vector<ConfigurationWrapperInterface*> &wrappers) :
            std::runtime_error("Unable to initialize configurations: " + vectorAsString(wrappers)) {
    }

    /*
     * Constructor that converts the vector of a config cycle vector to a string
     */
    ConfigurationCycleException::ConfigurationCycleException(const std::vector<std::string> &cycle) : std::runtime_error("Configuration cycle detected " + vectorAsString(cycle)) {
    }

}

/*
 * Properly "convert" the ConfigurationWrapperInterface to string for exception reporting purposes
 */
std::ostream& operator<<(std::ostream &os, const cadf::di::ConfigurationWrapperInterface *w) {
    os << w->getName() << " is missing resources " << cadf::di::vectorAsString(w->getWaitingResources());
    return os;
}
