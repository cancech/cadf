#ifndef CONFIG_CONTEXTTESTBEANMISMATCHCONFIGS_H_
#define CONFIG_CONTEXTTESTBEANMISMATCHCONFIGS_H_

#include "camb/di/Configuration.h"
#include "DummyClass.h"

// Provider config which provides a single DummyClass instance
CONFIGURATION(DummyBeanProviderTestConfig)

    BEANS(
            (BEAN, DummyClass*, "BeanName")
    )

END_CONFIGURATION

// Consumer config which tries to load the bean as an int
CONFIGURATION(DummyBeanAsIntConsumerTestConfig)

    RESOURCES(
            (int, BeanName)
    )

END_CONFIGURATION

// Consumer config which tries to load the bean pointer as a scalar
CONFIGURATION(PtrAsScalarConsumerTestConfig)

    RESOURCES(
            (DummyClass, BeanName)
    )

END_CONFIGURATION

#endif /* CONFIG_CONTEXTTESTBEANMISMATCHCONFIGS_H_ */
