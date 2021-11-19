#include "cadf/di/Configuration.h"

namespace cadf::di {

// CTOR
    BaseConfiguration::BaseConfiguration(BeanManager *manager) : m_beanManager(manager) {
    }

    /*
     * Get the default name
     */
    std::string BaseConfiguration::getName() {
        return "Undefined";
    }

    /*
     * Get the default resource names
     */
    const std::vector<std::string>& BaseConfiguration::getResourceNames() {
        static std::vector<std::string> emptyVec;
        return emptyVec;
    }

    /*
     * Get the default bean names
     */
    const std::vector<std::string>& BaseConfiguration::getBeanNames() {
        static std::vector<std::string> emptyVec;
        return emptyVec;
    }

    /*
     * Initialize the configuration
     */
    void BaseConfiguration::initialize() {
        postInit();
        provideBeans();
    }

    /*
     * Default post initialization
     */
    void BaseConfiguration::postInit() {
    }

    /*
     * Default bean provision
     */
    void BaseConfiguration::provideBeans() {
    }

}
