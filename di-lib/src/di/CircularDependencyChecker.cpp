#include "di/CircularDependencyChecker.h"

#include <iostream>
#include <algorithm>


namespace cadf::di {

    /*
     * Add the configuration
     */
    void CircularDependencyChecker::add(std::string name, std::vector<std::string> resources, std::vector<std::string> beans) {
        m_configResources[name] = resources;
        for (std::string s : beans)
            m_beanSource[s] = name;
    }

    /*
     * Check whether a cycle is present
     */
    bool CircularDependencyChecker::checkForCycle() {
        // Iterate over each configuration
        for (std::pair<std::string, std::vector<std::string>> p : m_configResources) {
            // Check each configuration to see whether or not it is part of a cycle.
            if (checkAllResources(p.first)) {
                minimizeCycle();
                return true;
            }
        }

        return false;
    }

    /*
     * Minimize the reported cycle the smallest it can be.
     */
    void CircularDependencyChecker::minimizeCycle() {
        auto start = std::find(m_cycle.begin(), m_cycle.end(), m_cycle[m_cycle.size() - 1]);
        m_cycle.erase(m_cycle.begin(), start);
    }

    /*
     * Get the detected cycle.
     */
    const std::vector<std::string>& CircularDependencyChecker::getCycle() {
        return m_cycle;
    }

    /*
     * Check all resources for the given configuration
     */
    bool CircularDependencyChecker::checkAllResources(std::string configName) {
        // Sanity check, make sure that this configuration has been registered
        if (!m_configResources.count(configName))
            return false;

        // Check all of the resources that the configuration requires
        std::vector<std::string> &resources = m_configResources[configName];
        for (std::string res : resources) {
            // Check if a cycle was found
            if (followResource(configName, res))
                return true;
        }

        // No cycle was found!
        return false;
    }

    /*
     * Follow the resource required by the configuration, and see whether a cycle is present.
     */
    bool CircularDependencyChecker::followResource(std::string configName, std::string resourceName) {
        // Build the display name and check if it has been seen.
        std::string compoundName = configName + "::" + resourceName;
        bool hasCycle = std::find(m_cycle.begin(), m_cycle.end(), compoundName) != m_cycle.end();
        m_cycle.push_back(compoundName);

        // Check if a cycle has been detected (the display name is already present in the vector
        if (hasCycle)
            return true;

        // Follow the source of the resource
        if (m_beanSource.count(resourceName)) {
            if (checkAllResources(m_beanSource[resourceName]))
                return true;
        }

        // No cycle detected
        m_cycle.pop_back();
        return false;
    }

}
