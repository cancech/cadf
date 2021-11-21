#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <vector>

#include "di/BeanManager.h"
#include "di/ConfigurationMacro.h"
#include "di/exception/InvalidBeanException.h"

namespace cadf::di {

    class ConfigurationWrapperInterface;
    template<class Config>
    class ConfigurationWrapper;
    class Context;

    /*
     * Base configuration which is to be extended for the purpose of creating a new configuration.
     */
    class BaseConfiguration {

        public:
            // CTOR
            BaseConfiguration(BeanManager *manager);
            // DTOR
            virtual ~BaseConfiguration() = default;

            /*
             * Get the name of the configuration. This is expected to be overridden by the subclass.
             *
             * @return std::string the name of the configuration
             */
            static std::string getName();

            /*
             * Get a list of all dependent configurations of this configuration.
             *
             * @param context pointer to the Context where the dependent configurations should be
             *                  registered
             */
            static void registerDependentConfigurations(Context *context) {
            }

            /*
             * Get a list of all resources that the configuration requires. This is expected to be overridden by the
             * subclass, so that each configuration can properly report its own resources.
             *
             * @return std::vector of std::string names of the resources that the configuration requires
             */
            static const std::vector<std::string>& getResourceNames();

            /*
             * Get a list of the names of all beans that the configuration will provide. This is expected to be overridden
             * by the subclass, so that each configuration can properly report its own beans.
             */
            static const std::vector<std::string>& getBeanNames();

            /*
             * Initialize the configuration, triggers the postInit and provideBeans methods (in that
             * order).
             */
            void initialize();

        protected:
            // The bean manager that holds the beans for this context
            BeanManager *m_beanManager;

            /*
             * Called after the configuration has been created, to allow steps to be taken once all required
             * resources become available.
             */
            virtual void postInit();

            /*
             * Called after postContruct completes, to allow the configuration to provide any/all beans.
             */
            virtual void provideBeans();

    };

    /*
     * Interface through which the ConfigurationManager can store and access the ConfigurationWrapper created
     * for each user defined configuration.
     */
    class ConfigurationWrapperInterface {

        public:
            // DTOR
            virtual ~ConfigurationWrapperInterface() = default;

            /*
             * Get the name of the configuration associated with the wrapper.
             *
             * @return std::string the name of the configuration
             */
            virtual std::string getName() const = 0;

            /*
             * Check whether or not all required resources are available.
             *
             * @return true if there are no more resources the configuration is waiting on
             */
            virtual bool areResourcesSatisfied() = 0;

            /*
             * Build the configuration
             *
             * @return pointer to the configuration that this class is wrapped around
             *
             * @throw ConfigurationMissingResourcesException if the configuration is created when resources are still missing
             */
            virtual BaseConfiguration* buildConfig() = 0;

            /*
             * Get the vector of resources that are remaining to be fulfilled.
             *
             * @return std::vector of std::string resource names that are not yet available
             */
            virtual const std::vector<std::string>& getWaitingResources() const = 0;

            /*
             * Get the names of all beans that the configuration will provide when its resources are fulfilled.
             *
             * @return std::vector of std::string bean names that will be provided.
             */
            virtual const std::vector<std::string>& getBeanNames() const = 0;
    };

    /*
     * Helper class which wraps around a Configuration class to facilitate managing the availability of
     * the resources for the wrapper. It tracks which resources the configuration is still waiting to be
     * fulfilled and will then allow creating the Configuration class.
     */
    template<class Config>
    class ConfigurationWrapper: public ConfigurationWrapperInterface {

        public:

            // CTOR
            ConfigurationWrapper(BeanManager *manager) : m_beanManager(manager) {
                // Automatically register resources to make sure that the waitingResources is appropriately populated
                registerResources();
            }

            /*
             * Get the name of the configuration that is wrapped.
             */
            virtual std::string getName() const {
                return Config::getName();
            }

            /*
             * Check whether or not all required resources are available.
             *
             * @return true if there are no more resources the configuration is waiting on
             */
            virtual bool areResourcesSatisfied() {
                m_waitingResources.erase(std::remove_if(m_waitingResources.begin(), m_waitingResources.end(), [this](std::string s) {
                    return m_beanManager->containsBean(s);
                }), m_waitingResources.end());
                return m_waitingResources.empty();
            }

            /*
             * Build the configuration
             *
             * @return pointer to the configuration that this class is wrapped around
             *
             * @throw ConfigurationMissingResourcesException if the configuration is created when resources are still missing
             */
            virtual BaseConfiguration* buildConfig() {
                if (!areResourcesSatisfied())
                    throw ConfigurationMissingResourcesException(getName(), m_waitingResources);
                return new Config(m_beanManager);
            }

            /*
             * Get the vector of waiting resources.
             */
            virtual const std::vector<std::string>& getWaitingResources() const {
                return m_waitingResources;
            }

            /*
             * Get the vector of beans that will be provided.
             */
            virtual const std::vector<std::string>& getBeanNames() const {
                return Config::getBeanNames();
            }

        private:
            // Vector of the names of resources that the configuration is still waiting on
            std::vector<std::string> m_waitingResources;
            // The bean manager that holds the beans for this context
            BeanManager *m_beanManager;

            /*
             * Register all of the resources that the configuration requires
             */
            void registerResources() {
                const std::vector<std::string> allResources = Config::getResourceNames();
                m_waitingResources.assign(allResources.begin(), allResources.end());
            }
    };

}

#endif /* CONFIGURATION_H_ */
