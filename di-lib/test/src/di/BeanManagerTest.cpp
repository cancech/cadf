
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>

#include "di/BeanManager.h"
#include "DummyClass.h"

struct BeanManagerTestCreator {
        cadf::di::ValueWrapper<DummyClass>* create() {
            return new cadf::di::ValueWrapper<DummyClass>(135);
        }
};

// Classes to be used to test the cycle detection capabilities
class A_depends_on_B;
class B_depends_on_A;

cadf::di::BeanManager sharedManager;

class A_depends_on_B {
        B_depends_on_A *b = sharedManager.getBean<B_depends_on_A*>("B_depends_on_A");
};

class B_depends_on_A {
        A_depends_on_B *a = sharedManager.getBean<A_depends_on_B*>("A_depends_on_B");
};

BOOST_AUTO_TEST_SUITE(BeanManager_Test_Suite)

    BOOST_AUTO_TEST_CASE(Register_bean_with_no_name) {
        cadf::di::BeanManager manager;
        BOOST_REQUIRE_THROW(manager.registerBean<int&>(""), cadf::di::InvalidBeanNameException);
    }

    BOOST_AUTO_TEST_CASE(Register_singleton_reference) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass&>("singleton_reference");

        DummyClass &bean1 = manager.getBean<DummyClass&>("singleton_reference");
        DummyClass &bean2 = manager.getBean<DummyClass&>("singleton_reference");

        BOOST_CHECK_EQUAL(&bean1, &bean2);
    }

    BOOST_AUTO_TEST_CASE(Register_singleton_pointer) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass*>("singleton_pointer");

        DummyClass *bean1 = manager.getBean<DummyClass*>("singleton_pointer");
        DummyClass *bean2 = manager.getBean<DummyClass*>("singleton_pointer");

        BOOST_CHECK_EQUAL(bean1, bean2);
        BOOST_CHECK_EQUAL(bean1->getValue(), bean2->getValue());
    }

    BOOST_AUTO_TEST_CASE(Register_factory_scalar) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass, cadf::di::FactoryBeanCreator<DummyClass>>("factory_scalar");

        DummyClass bean1 = manager.getBean<DummyClass>("factory_scalar");
        DummyClass bean2 = manager.getBean<DummyClass>("factory_scalar");

        BOOST_CHECK(&bean1 != &bean2);
    }

    BOOST_AUTO_TEST_CASE(Register_factory_pointer) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass*, cadf::di::FactoryBeanCreator<DummyClass*>>("factory_pointer");

        DummyClass *bean1 = manager.getBean<DummyClass*>("factory_pointer");
        DummyClass *bean2 = manager.getBean<DummyClass*>("factory_pointer");

        BOOST_CHECK(bean1 != bean2);
        BOOST_CHECK(bean1 != NULL);
        BOOST_CHECK(bean2 != NULL);
        delete (bean1);
        delete (bean2);
    }

    BOOST_AUTO_TEST_CASE(Register_multiples_of_same_bean) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass, BeanManagerTestCreator>("multiple_of_same_bean");
        // Verify that the original bean was actually added
        DummyClass bean = manager.getBean<DummyClass>("multiple_of_same_bean");
        BOOST_CHECK_EQUAL(135, bean.getValue());

        BOOST_REQUIRE_THROW(manager.registerBean<int*>("multiple_of_same_bean"), cadf::di::InvalidBeanNameException);

        // Verify that the original bean is still there
        DummyClass bean2 = manager.getBean<DummyClass>("multiple_of_same_bean");
        BOOST_CHECK_EQUAL(135, bean2.getValue());
    }

    BOOST_AUTO_TEST_CASE(Retreive_bean_as_wrong_type) {
        cadf::di::BeanManager manager;
        manager.registerBean<DummyClass&>("test_for_type_check");
        // Verify that the original bean was actually added
        DummyClass bean = manager.getBean<DummyClass&>("test_for_type_check");
        BOOST_CHECK_EQUAL(0, bean.getValue());

        BOOST_REQUIRE_THROW(manager.getBean<int>("test_for_type_check"), cadf::di::InvalidBeanTypeException);
    }

    BOOST_AUTO_TEST_CASE(Retreive_bean_not_yet_registered) {
        cadf::di::BeanManager manager;
        BOOST_CHECK(!manager.containsBean("this_bean_does_not_exist"));

#ifdef ENABLE_BEAN_AUTOREGISTRATION
	// Enable auto registration
	DummyClass *autoBean = manager.getBean<DummyClass*>("this_bean_does_not_exist");
	BOOST_CHECK_EQUAL(0, autoBean->getValue());
	// Update the value of the bean
	autoBean->setValue(13579);
	BOOST_CHECK_EQUAL(13579, autoBean->getValue());
	// Make sure that this is a singleton bean
	auto otherPtr = manager.getBean<DummyClass*>("this_bean_does_not_exist");
	BOOST_CHECK_EQUAL(13579, otherPtr->getValue());
	BOOST_CHECK_EQUAL(autoBean, otherPtr);
#else
        BOOST_REQUIRE_THROW(manager.getBean<boost::any>("this_bean_does_not_exist"), cadf::di::InvalidBeanNameException);
#endif
    }

    BOOST_AUTO_TEST_CASE(Bean_instance_scalar) {
        cadf::di::BeanManager manager;
        DummyClass instance(101);
        manager.registerBeanInstance<DummyClass>("instance_scalar", instance);

        DummyClass bean = manager.getBean<DummyClass>("instance_scalar");
        BOOST_CHECK_EQUAL(instance.getValue(), bean.getValue());
        BOOST_CHECK(&instance != &bean);
    }

    BOOST_AUTO_TEST_CASE(Bean_instance_reference) {
        cadf::di::BeanManager manager;
        DummyClass instance(101);
        manager.registerBeanInstance<DummyClass&>("instance_reference", instance);

        DummyClass &bean = manager.getBean<DummyClass&>("instance_reference");
        BOOST_CHECK_EQUAL(instance.getValue(), bean.getValue());
        BOOST_CHECK_EQUAL(&instance, &bean);
    }

    BOOST_AUTO_TEST_CASE(Bean_instance_pointer) {
        cadf::di::BeanManager manager;
        DummyClass *instance = new DummyClass(202);
        manager.registerBeanInstance<DummyClass*>("instance_pointer", instance);

        DummyClass *bean = manager.getBean<DummyClass*>("instance_pointer");
        BOOST_CHECK_EQUAL(instance->getValue(), bean->getValue());
        BOOST_CHECK_EQUAL(instance, bean);
        delete (instance);
    }

    BOOST_AUTO_TEST_CASE(Check_for_Cyclic_Bean_Dependencies) {
        // Register the two beans (will not get created until requested)
        sharedManager.registerBean<A_depends_on_B*>("A_depends_on_B");
        sharedManager.registerBean<B_depends_on_A*>("B_depends_on_A");

        BOOST_REQUIRE_THROW(sharedManager.getBean<A_depends_on_B*>("A_depends_on_B"), cadf::di::BeanDependencyCycleException);
        BOOST_REQUIRE_THROW(sharedManager.getBean<B_depends_on_A*>("B_depends_on_A"), cadf::di::BeanDependencyCycleException);
    }

    BOOST_AUTO_TEST_SUITE_END()
