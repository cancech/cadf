#ifndef CONFIG_CONTEXTTESTDEPENDENCTCONFIGS_H_
#define CONFIG_CONTEXTTESTDEPENDENCTCONFIGS_H_

#include <cassert>

#include "di/Configuration.h"
#include "DummyClass.h"

// Config which provides a single bean
CONFIGURATION(ProviderManagerTestConfig)

    BEANS(
            (BEAN, DummyClass*, "providerManagerDummySingleton")
    )

END_CONFIGURATION

// Config which requires a single resource from ProviderManagerTestConfig and provides a single bean
CONFIGURATION(ProviderConsumerManagerTestConfig)

    BEANS(
            (BEAN, DummyClass, cadf::di::FactoryBeanCreator<DummyClass>, "providerConsumerDummyFactory")
    )

    RESOURCES(
            (DummyClass*, "providerManagerDummySingleton", someVar)
    )

END_CONFIGURATION

// Config which requires two resources provided by ProviderManagerTestConfig and ProviderConsumerManagerTestConfig
CONFIGURATION(ConsumerManagerTestConfig)

    RESOURCES(
            (DummyClass*, providerManagerDummySingleton),
            (DummyClass, "providerConsumerDummyFactory", var)
    )

END_CONFIGURATION

// Config which lists the above configs as dependencies and verifies that the beans were properly loaded.
CONFIGURATION(ConfigWithDeps)

        DEPENDENCIES(ConsumerManagerTestConfig, ProviderConsumerManagerTestConfig, ProviderManagerTestConfig)

        virtual void postInit() {
            assert(var != NULL);
            assert(var->getValue() == providerConsumerDummyFactory.getValue());
        }

    RESOURCES(
            (DummyClass*, "providerManagerDummySingleton", var),
            (DummyClass, providerConsumerDummyFactory)
    )

END_CONFIGURATION

CONFIGURATION(NestedConfig1)
        DEPENDENCIES(ConfigWithDeps)
END_CONFIGURATION

CONFIGURATION(NestedConfig2)
        DEPENDENCIES(NestedConfig1)
END_CONFIGURATION

CONFIGURATION(NestedConfig3)

        DEPENDENCIES(NestedConfig2)

        virtual void postInit() {
            assert(var != NULL);
            assert(var->getValue() == providerConsumerDummyFactory.getValue());
        }

    RESOURCES(
            (DummyClass*, "providerManagerDummySingleton", var),
            (DummyClass, providerConsumerDummyFactory)
    )

END_CONFIGURATION

CONFIGURATION(NestedCycle)
        DEPENDENCIES(NestedCycle)
END_CONFIGURATION

#endif /* CONFIG_CONTEXTTESTDEPENDENCTCONFIGS_H_ */
