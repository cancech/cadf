#ifndef INVALIDBEANEXCEPTION_H_
#define INVALIDBEANEXCEPTION_H_

#include <exception>
#include <vector>
#include <iostream>

namespace camb::di {

    class ConfigurationWrapperInterface;

    /*
     * Exception which is to be thrown when attempting to use an invalid name for a bean. This includes:
     *      * registering a bean with a name that already exists
     *      * retrieving a bean that has not yet been registered
     */
    struct InvalidBeanNameException: public std::runtime_error {
            InvalidBeanNameException(const std::string name, const std::string reason) : std::runtime_error("Invalid bean name \"" + name + "\": " + reason) {
            }
    };

    /*
     * Exception which is to be throw when attempting to retrieve a bean which is of a different type than
     * what is desired (i.e.: register bean is Foo, but trying to retrieve Bar).
     */
    struct InvalidBeanTypeException: public std::runtime_error {
            InvalidBeanTypeException(const std::string name, const std::string wanted, const std::string actual) :
                    std::runtime_error("Invalid bean type for \"" + name + "\": wanted " + wanted + " but was " + actual) {
            }
    };

    /*
     * Exception which is thrown when attempting to get a bean which exists within a dependency cycle (i.e.: BeanA depends on
     * BeanB, which depends on BeanA).
     */
    struct BeanDependencyCycleException: public std::runtime_error {
            BeanDependencyCycleException(const std::vector<std::string> &cycle);
    };

    /*
     * Exception which is thrown when attempting to create a configuration while it is still waiting on resources.
     */
    struct ConfigurationMissingResourcesException: public std::runtime_error {
            ConfigurationMissingResourcesException(const std::string configName, const std::vector<std::string> &missing);
    };

    /*
     * Exception which is thrown when an issue is encountered when attempting to initialize the configurations.
     */
    struct ConfigurationInitializationException: public std::runtime_error {
            ConfigurationInitializationException(const std::vector<ConfigurationWrapperInterface*> &wrappers);
    };

    /*
     * Exception which is thrown when a cycle is detected between two (or more) configuration files.
     */
    struct ConfigurationCycleException: public std::runtime_error {
            ConfigurationCycleException(const std::vector<std::string> &cycle);
    };

}

#endif /* INVALIDBEANEXCEPTION_H_ */
