#ifndef CIRCULARDEPENDENCYCHECKER_H_
#define CIRCULARDEPENDENCYCHECKER_H_

#include <string>
#include <vector>
#include <map>

namespace camb::di {

    /*
     * Helper class that checks to see whether or not a circular dependency is present between
     * configuration files.
     */
    class CircularDependencyChecker {

        public:

            /*
             * Add the details of a single configuration file.
             *
             * @param name std::string the name of the configuration file
             * @param resources std::vector<std::string> containing the names of all of the resources within
             * 		  the configuration to check. Note that fulfilled resources need not be included, as a
             * 		  fulfilled resource cannot be party to a cycle
             * @param beans std::vector<std::string> the names of the beans that the configuration will provide
             */
            void add(std::string name, std::vector<std::string> resources, std::vector<std::string> beans);

            /*
             * Check if there is a cycle present among the configuration files that we added.
             *
             * @return true if a cycle was detected
             */
            bool checkForCycle();

            /*
             * Get the vector which describes the cycle, indicating where the cycle takes place.
             *
             * @return std::vector<std::string> indicating the configurations and resources across which the cycle can be found
             */
            const std::vector<std::string>& getCycle();

        private:
            // Map of all beans to the configuration that provides it
            std::map<std::string, std::string> m_beanSource;
            // Map of configuration names to the resources they require
            std::map<std::string, std::vector<std::string>> m_configResources;
            // Vector acting as a stack to determine whether a cycle is present
            std::vector<std::string> m_cycle;

            /*
             * Check all of the resources belonging to the configuration file, to determine whether a cycle is present.
             *
             * @param configName std::string the name of the configuration file to check
             *
             * @return true if a cycle was detected
             */
            bool checkAllResources(std::string configName);

            /*
             * Follow the resource that the configuration requires.
             *
             * @param configName std::string the name of the configuration
             * @param resourceName std::string the name of the resource
             *
             * @return true if a cycle was detected
             */
            bool followResource(std::string configName, std::string resourceName);

            /*
             * Minimize the reported cycle to the smallest it can be. This will discard any extraneous "path" that was traveled prior
             * to detecting the cycle itself. For example 1 -> 2 -> 3 -> 4 -> 3 will minimize such that it only report 3 -> 4 -> 3.
             */
            void minimizeCycle();
    };

}

#endif /* CIRCULARDEPENDENCYCHECKER_H_ */
