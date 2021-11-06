#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "di/BeanProvider.h"
#include "DummyClass.h"

struct BeanProviderTestCreator {
        camb::di::ValueWrapper<DummyClass>* create() {
            return new camb::di::ValueWrapper<DummyClass>(DummyClass(789));
        }
};

BOOST_AUTO_TEST_SUITE(BeanProvider_Test_Suite)

    BOOST_AUTO_TEST_CASE(Bean_creator_provider) {
        camb::di::BeanCreatorProvider<DummyClass, BeanProviderTestCreator> provider;

        DummyClass bean = provider.getBean();
        BOOST_CHECK_EQUAL(789, bean.getValue());
        // The result need not be exactly the class name, so only need to make sure that the class name is contained/represented
        BOOST_CHECK(provider.getType().find("DummyClass") != std::string::npos);
    }

    BOOST_AUTO_TEST_CASE(Bean_singleton_provider_reference) {
        camb::di::BeanCreatorProvider<int&, camb::di::SingletonBeanCreator<int&>> provider;

        int &bean1 = provider.getBean(); // @suppress("Method cannot be resolved")
        int &bean2 = provider.getBean(); // @suppress("Method cannot be resolved")

        BOOST_CHECK_EQUAL(&bean1, &bean2);
    }

    BOOST_AUTO_TEST_CASE(Bean_singleton_provider_pointer) {
        camb::di::BeanCreatorProvider<DummyClass*, camb::di::SingletonBeanCreator<DummyClass*>> provider;

        DummyClass *bean1 = provider.getBean();
        DummyClass *bean2 = provider.getBean();

        BOOST_CHECK_EQUAL(bean1, bean2);
    }

    BOOST_AUTO_TEST_CASE(Bean_factory_provider_scalar) {
        camb::di::BeanCreatorProvider<DummyClass, camb::di::FactoryBeanCreator<DummyClass>> provider;

        DummyClass bean1 = provider.getBean();
        DummyClass bean2 = provider.getBean();

        BOOST_CHECK(&bean1 != &bean2);
    }

    BOOST_AUTO_TEST_CASE(Bean_factory_provider_pointer) {
        camb::di::BeanCreatorProvider<int*, camb::di::FactoryBeanCreator<int*>> provider;

        int *bean1 = provider.getBean();
        int *bean2 = provider.getBean();

        BOOST_CHECK(bean1 != bean2);
        delete (bean1);
        delete (bean2);
    }

    BOOST_AUTO_TEST_CASE(Bean_instance_provider) {
        DummyClass instance(987);
        camb::di::BeanInstanceProvider<DummyClass*> provider(&instance);

        DummyClass *bean = provider.getBean();
        BOOST_CHECK_EQUAL(instance.getValue(), bean->getValue());
        BOOST_CHECK_EQUAL(&instance, bean);
    }

    BOOST_AUTO_TEST_SUITE_END()
