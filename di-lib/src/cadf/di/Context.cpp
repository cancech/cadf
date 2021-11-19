#include "cadf/di/Context.h"

namespace cadf::di {

// DTOR
    Context::~Context() {
        // Cleanup the configs which are still waiting
        for (ConfigurationWrapperInterface *i : m_waitingConfigs)
            delete (i);

        // Clean up the configs which have already been processed
        for (BaseConfiguration *c : m_activeConfigs)
            delete (c);
    }

    /*
     * Assemble the context
     */
    void Context::assemble() {
        // So long as configs are being processed, keep going. Extremely simplistic mechanism for
        // processing the dependencies across multiple configs.
        bool configProcessed = false;
        do {
            configProcessed = false;

            // Iterate through all registered configs and try to build and initialize them
            std::vector<ConfigurationWrapperInterface*>::iterator it = m_waitingConfigs.begin();
            while (it != m_waitingConfigs.end()) {
                // Try to load the configuration
                if (loadConfig(*it)) {
                    // Success! Clean up the wrapper for the loaded config
                    configProcessed = true;
                    delete (*it);
                    it = m_waitingConfigs.erase(it);
                } else
                    ++it;

            }
        } while (configProcessed);

        // Ensure the sanity of the context
        verifyContext();
    }

    /*
     * Try to load the configuration from the wrapper.
     */
    bool Context::loadConfig(ConfigurationWrapperInterface *wrapper) {
        if (!wrapper->areResourcesSatisfied())
            return false;

        // The config has all of its resources/dependencies satisfied
        // Create the config and initialize, process it
        BaseConfiguration *config = wrapper->buildConfig();
        try {
            config->initialize();
        } catch (...) {
            // Prevent leaking memory if an exception is thrown during initialization
            delete (config);
            throw;
        }

        // Now that the configuration is created and initialized, store it
        m_activeConfigs.push_back(config);
        return true;
    }

    /*
     * Helper function to check whether a cycle is present
     *
     * @throws ConfigurationCycleException if a cycle is detected
     */
    void checkForCycle(std::vector<ConfigurationWrapperInterface*> &configs) {
        // Check to see if there is a circular dependency
        CircularDependencyChecker checker;
        for (ConfigurationWrapperInterface *i : configs)
            checker.add(i->getName(), i->getWaitingResources(), i->getBeanNames());
        if (checker.checkForCycle()) {
            throw ConfigurationCycleException(checker.getCycle());
        }
    }

    /*
     * Sanity check of the context
     */
    void Context::verifyContext() {
        if (!m_waitingConfigs.empty()) {
            // Check whether a cycle is present and preventing the assembly of the context
            checkForCycle(m_waitingConfigs);
            // If not, just throw the exception that cannot initialize
            throw ConfigurationInitializationException(m_waitingConfigs);
        }
    }

}
