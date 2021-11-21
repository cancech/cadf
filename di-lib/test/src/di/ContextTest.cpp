
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "di/Context.h"

#include "config/ContextTestConfigs.h"
#include "config/ContextTestBeanMismatchConfigs.h"
#include "config/ContextTestDependenctConfigs.h"
#include "config/ContextTestCircularDependencyConfig.h"

BOOST_AUTO_TEST_SUITE(ConfigurationManager_Test_Suite)

    BOOST_AUTO_TEST_CASE(Single_Configuration_Missing_Resources) {
        cadf::di::Context context;
        context.registerConfiguration<SingleConfigMissingResourcesTestConfig>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::ConfigurationInitializationException);
    }

    BOOST_AUTO_TEST_CASE(Multiple_Configs) {
        cadf::di::Context context;
        context.registerConfiguration<ConsumerManagerTestConfig, ProviderConsumerManagerTestConfig, ProviderManagerTestConfig>();
        context.assemble();
    }

    BOOST_AUTO_TEST_CASE(No_Config_Registered) {
        cadf::di::Context context;
        context.assemble();
    }

    BOOST_AUTO_TEST_CASE(Config_Dependency) {
        cadf::di::Context context;
        context.registerConfiguration<ConfigWithDeps>();
        context.assemble();
    }

    BOOST_AUTO_TEST_CASE(Config_Nested_Dependency) {
        cadf::di::Context context;
        context.registerConfiguration<NestedConfig3>();
        context.assemble();
    }

    BOOST_AUTO_TEST_CASE(Config_Nested_Cycle) {
        cadf::di::Context context;
        context.registerConfiguration<NestedCycle>();
        context.assemble();
    }

    BOOST_AUTO_TEST_CASE(Wrong_Bean_Type) {
        cadf::di::Context context;
        context.registerConfiguration<DummyBeanProviderTestConfig>();
        context.registerConfiguration<DummyBeanAsIntConsumerTestConfig>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::InvalidBeanTypeException);
    }

    BOOST_AUTO_TEST_CASE(Wrong_Bean_Ptr) {
        cadf::di::Context context;
        context.registerConfiguration<DummyBeanProviderTestConfig, PtrAsScalarConsumerTestConfig>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::InvalidBeanTypeException);
    }

    BOOST_AUTO_TEST_CASE(Duplicate_Resources_Different_Config) {
        cadf::di::Context context;
        context.registerConfiguration<ProvideBean1Config, ProvideBean2Config>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::InvalidBeanNameException);
    }

    BOOST_AUTO_TEST_CASE(Load_Same_Config_Twice) {
        cadf::di::Context context;
        context.registerConfiguration<ProvideBean1Config>();
        context.registerConfiguration<ProvideBean1Config>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::InvalidBeanNameException);
    }

    BOOST_AUTO_TEST_CASE(Circular_Config_Dependency) {
        cadf::di::Context context;
        context.registerConfiguration<CircularDep1TestConfig, CircularDep2TestConfig>();
        BOOST_REQUIRE_THROW(context.assemble(), cadf::di::ConfigurationCycleException);
    }

    BOOST_AUTO_TEST_CASE(Beans_External_To_Context) {
        std::string str = "Something or other";
        cadf::di::Context context;
        context.registerConfiguration<SingleConfigMissingResourcesTestConfig, ProvideBean1Config>();
        context.registerBeanInstance<int>("missingIntValue", 987);
        context.registerBeanInstance<std::string&>("missingStringReference", str);
        context.assemble();

        int &intFromContext = context.getBean<int&>("DuplicateBean");
        BOOST_CHECK_EQUAL(0, intFromContext);
    }

    BOOST_AUTO_TEST_SUITE_END()
