#ifndef CONFIG_CONFIGURATIONTESTCONFIGS_H_
#define CONFIG_CONFIGURATIONTESTCONFIGS_H_

#include "di/Configuration.h"
#include "DummyClass.h"

// Configuration that is expected to not have its resources fulfilled.
CONFIGURATION(MissingResourcesTestConfig)

    RESOURCES(
            (int, missingIntValue),
            (std::string&, missingStringReference),
            (DummyClass*, missingDummyPointer)
    )

END_CONFIGURATION

// Configuration that required no resources nor provides any beans
CONFIGURATION(NoResourceTestConfig)

        int numTimesPostConstructCalled = 0;
        int numTimesProvideBeansCalled = 0;

        virtual void postInit() {
            numTimesPostConstructCalled++;
        }

        virtual void provideBeans() {
            numTimesProvideBeansCalled++;
        }

END_CONFIGURATION

// Configuration which requires no resources but provides some beans
CONFIGURATION(ProviderTestConfig)

        virtual void postInit() {
            someIntValue = 56789;
        }

    BEANS(
            (BEAN, DummyClass, camb::di::FactoryBeanCreator<DummyClass>, "providerDummyClassFactory"),
            (BEAN, DummyClass*, "providerDummyClassSingleton"),
            (BEAN_INSTANCE, int&, "providerSomeIntValue", someIntValue)
    )

    public:

        int* getSomeIntValuePtr() {
            return &someIntValue;
        }

    private:
        int someIntValue = 0;

END_CONFIGURATION

// Configuration that provides no beans but pulls in all of the resources provided by ProviderTestConfig
CONFIGURATION(ConsumerTestConfig)

        int* getProviderSomeIntValue() {
            return &providerSomeIntValue;
        }

        DummyClass* getProviderDummyClassSingleton() {
            return providerDummyClassSingleton;
        }

    RESOURCES(
            (DummyClass, providerDummyClassFactory),
            (DummyClass*, providerDummyClassSingleton),
            (int&, providerSomeIntValue)
    )

END_CONFIGURATION

#endif /* CONFIG_CONFIGURATIONTESTCONFIGS_H_ */
