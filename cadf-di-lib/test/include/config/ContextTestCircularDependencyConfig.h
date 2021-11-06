#ifndef CONFIG_CONTEXTTESTCIRCULARDEPENDENCYCONFIG_H_
#define CONFIG_CONTEXTTESTCIRCULARDEPENDENCYCONFIG_H_

#include "cadf/di/Configuration.h"

// Configuration which requires a resource from CircularDep2TestConfig and provides a bean
// CircularDep2TestConfig requires
CONFIGURATION(CircularDep1TestConfig)

    BEANS(
            (BEAN, double&, "circularBean2")
    )

    RESOURCES(
            (int&, circularBean1)
    )

END_CONFIGURATION

// Configuration which requires a resource from CircularDep1TestConfig and provides a bean
// CircularDep1TestConfig requires
CONFIGURATION(CircularDep2TestConfig)

    BEANS(
            (BEAN, int&, "circularBean1")
    )

    RESOURCES(
            (double&, circularBean2)
    )

END_CONFIGURATION

#endif /* CONFIG_CONTEXTTESTCIRCULARDEPENDENCYCONFIG_H_ */
