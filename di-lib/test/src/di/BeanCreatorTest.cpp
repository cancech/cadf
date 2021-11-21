
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "di/BeanCreator.h"

#include "DummyClass.h"

BOOST_AUTO_TEST_SUITE(BeanCreator_Test_Suite)

    BOOST_AUTO_TEST_CASE(Singleton_Scalar_Creator) {
        cadf::di::SingletonBeanCreator<DummyClass&> creator;

        cadf::di::ValueWrapper<DummyClass&> *wrapper = creator.create();
        DummyClass &dummy1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy1.getValue());

        wrapper = creator.create();
        DummyClass &dummy2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy2.getValue());

        // Verify that it is the same instance
        dummy1.setValue(5);
        BOOST_CHECK_EQUAL(5, dummy1.getValue());
        BOOST_CHECK_EQUAL(5, dummy2.getValue());
        BOOST_CHECK_EQUAL(&dummy1, &dummy2);
    }

    BOOST_AUTO_TEST_CASE(Singleton_Pointer_Creator) {
        cadf::di::SingletonBeanCreator<DummyClass*> creator;

        cadf::di::ValueWrapper<DummyClass*> *wrapper = creator.create();
        DummyClass *dummy1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy1->getValue());

        wrapper = creator.create();
        DummyClass *dummy2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy2->getValue());

        // Verify that it is the same instance
        BOOST_CHECK_EQUAL(dummy1, dummy2);
        dummy1->setValue(5);
        BOOST_CHECK_EQUAL(5, dummy1->getValue());
        BOOST_CHECK_EQUAL(5, dummy2->getValue());
    }

    BOOST_AUTO_TEST_CASE(Factory_Scalar_Creator) {
        cadf::di::FactoryBeanCreator<DummyClass> creator;

        cadf::di::ValueWrapper<DummyClass> *wrapper = creator.create();
        DummyClass dummy1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy1.getValue());

        wrapper = creator.create();
        DummyClass dummy2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy2.getValue());

        // Verify that it is the same instance
        dummy1.setValue(5);
        BOOST_CHECK_EQUAL(5, dummy1.getValue());
        BOOST_CHECK_EQUAL(0, dummy2.getValue());
    }

    BOOST_AUTO_TEST_CASE(Factory_Pointer_Creator) {
        cadf::di::FactoryBeanCreator<DummyClass*> creator;

        cadf::di::ValueWrapper<DummyClass*> *wrapper = creator.create();
        DummyClass *dummy1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy1->getValue());

        wrapper = creator.create();
        DummyClass *dummy2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, dummy2->getValue());

        // Verify that it is the same instance
        BOOST_CHECK(dummy1 != dummy2);
        dummy1->setValue(5);
        BOOST_CHECK_EQUAL(5, dummy1->getValue());
        BOOST_CHECK_EQUAL(0, dummy2->getValue());
        delete (dummy1);
        delete (dummy2);
    }

    BOOST_AUTO_TEST_CASE(Smart_Singleton_Creator) {
        cadf::di::SmartSingletonBeanCreator<DummyClass> creator;

        // Get one instance
        cadf::di::ValueWrapper<std::shared_ptr<DummyClass>> *wrapper = creator.create();
        std::shared_ptr<DummyClass> bean1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, bean1->getValue());

        // Get a second instance
        wrapper = creator.create();
        std::shared_ptr<DummyClass> bean2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, bean2->getValue());

        // Make sure that the two instances are the same
        BOOST_CHECK_EQUAL(bean1, bean2);
    }

    BOOST_AUTO_TEST_CASE(Smart_Factory_Creator) {
        cadf::di::SmartFactoryBeanCreator<DummyClass> creator;

        // Get one instance
        cadf::di::ValueWrapper<std::shared_ptr<DummyClass>> *wrapper = creator.create();
        std::shared_ptr<DummyClass> bean1 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, bean1->getValue());

        // Get a second instance
        wrapper = creator.create();
        std::shared_ptr<DummyClass> bean2 = wrapper->m_value;
        delete (wrapper);
        BOOST_CHECK_EQUAL(0, bean2->getValue());

        // Make sure that the two instances are the same
        BOOST_CHECK(bean1 != bean2);
    }

    BOOST_AUTO_TEST_SUITE_END()
