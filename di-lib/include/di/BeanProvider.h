#ifndef BEANPROVIDER_H_
#define BEANPROVIDER_H_

#include <type_traits>
#include <string>

#include "di/BeanCreator.h"

namespace cadf::di {

    /*
     * The base base class for bean providers. Due to limitations of C++ this is the type that will be tracked
     * by the BeanManager, so as to allow it to not get bogged down with the template parameters.
     */
    class BaseProvider {
        public:
            // DTOR
            virtual ~BaseProvider() = default;

            /*
             * Get the type of the bean as a string. Intended to be used purely for debugging purposes.
             *
             * @returns std::string the type of the bean as a string
             */
            virtual std::string getType() = 0;
    };

    /*
     * Base class for the bean provider, that allows the BeanManager to determine what kind of bean is being provided,
     * without getting bogged down with the details of how exactly the bean is being created (i.e.: hide that template
     * parameter from the BeanManager).
     */
    template<typename T>
    class TypeProvider: public BaseProvider {

        public:
            // DTOR
            virtual ~TypeProvider() = default;

            /*
             * Get the type of the bean as a string. Intended to be used purely for debugging purposes.
             *
             * @returns std::string the type of the bean as a string
             */
            virtual std::string getType() {
                return typeid(T).name();
            }

            /*
             * Get the bean instance that this provider provides. Note that this class does not actually create
             * the bean, and instead relies on a sub class to handle the bean creation.
             */
            T getBean() {
                // The expectation is that the subclass will populate the bean instance field within the initBean method
                provideBean();

                // Bit of a roundabout way to do this, but required in order to get around the limitations of C++.
                // Retrieve the bean from the interim ValueWrapper, and make sure that the ValueWrapper is deallocated
                // to avoid leaking memory.
                T bean = m_beanWrapper->m_value;
                delete (m_beanWrapper);
                m_beanWrapper = NULL;
                return bean;
            }

        protected:
            // This is an interim storage vehicle where the bean created by the subclass is to be stored.
            ValueWrapper<T> *m_beanWrapper = NULL;

            /*
             * Provide the bean by storing it within the interim ValueWrapper beanWrapper. Note that this *must* be allocated
             * via new, as getBean will be cleaning up this memory after it has retrieved the wrapper value.
             */
            virtual void provideBean() = 0;
    };

    /*
     * Provides the beanWrapper that the Creator creates. This class primarily exists to make
     * the relationship between the Bean and Creator explicitly clear, and hide the details
     * of it from the BeanManager class.
     */
    template<typename T, class Creator>
    class BeanCreatorProvider: public TypeProvider<T> {

        private:
            // The creator for determining how to create bean instances
            Creator m_creator;

            /*
             * The creator creates the beanWrapper and store the resulting beanWrapper in the interim beanWrapper instance field container
             */
            void provideBean() {
                TypeProvider<T>::m_beanWrapper = m_creator.create();
            }
    };

    /*
     * Provides the instance that was given to the BeanInstanceProvider
     */
    template<typename T>
    class BeanInstanceProvider: public TypeProvider<T> {
        public:
            /*
             * Create the provider with the instance that is to be provided
             */
            BeanInstanceProvider(T instance) : m_instance(instance) {
            }

        private:
            // The instance passed to the provider
            T m_instance;

            /*
             * Provides the instance by passing it to interim beanWrapper instance field container
             */
            void provideBean() {
                TypeProvider<T>::m_beanWrapper = new ValueWrapper<T>(m_instance);
            }
    };

}

#endif /* BEANPROVIDER_H_ */
