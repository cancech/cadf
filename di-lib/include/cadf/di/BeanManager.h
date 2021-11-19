#ifndef BEANMANAGER_H_
#define BEANMANAGER_H_

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <typeinfo>

#include "cadf/di/BeanProvider.h"
#include "cadf/di/exception/InvalidBeanException.h"

namespace cadf::di {

    /*
     * Manager responsible for tracking all bean providers, and retrieving the beans from when
     * on request. The bean must be first registered by a producer, in order for it to be retrieved.
     * Each bean is registered with a unique name, and only one bean can be registered using a given
     * name. Exceptions will be thrown if a duplicate registration is performed under the same name.
     *
     * An optional auto-register capability can be enabled, to allow registering beans when they are
     * requested. If this capability is enabled then:
     *        * All auto-registered beans will be singleton (the default creator for beans)
     *        * The bean which is to be "auto-registered" must have a default constructor (the bean will
     *        be created via its default constructor)
     *
     * To enable the auto-register capability, compile with the ENABLE_BEAN_AUTOREGISTRATION flag/symbol defined.
     */
    class BeanManager {

        public:
            // DTOR
            virtual ~BeanManager();

            /*
             * Register a bean with the manager. The bean will rely on the specified creator for
             * bean management/creation.
             *
             * @template Type the type of the bean which is to be registered
             * @template Creator the creator which is to be used to instantiate/manage the bean
             *           defaults to SingletonBeanCreator. The Creator must provide a method
             *           matching the signature of: ValueWrapper<T>* create(), where the returned
             *           wrapper is allocated in dynamic memory (i.e.: created via new).
             *
             * @param name std::string the name of the bean to register
             *
             * @throws InvalidBeanNameException if a bean by that name already exists, or if the bean name is empty
             */
            template<typename Type, class Creator = SingletonBeanCreator<Type>>
            void registerBean(std::string name) {
                verifyCanAddBean(name);
                m_repo[name] = new BeanCreatorProvider<Type, Creator>();
            }

            /*
             * Register a specific instance as a bean under the indicated name. The behavior of this will depend
             * based on what type the instance is. A reference or pointer will be treated as a singleton, while
             * a scalar will be as a factory. Note, that the manager is not responsible for the memory allocated
             * to the instance, meaning if it is freed (i.e.: manually, or due to reference going out of scope)
             * undefined behavior can be expected.
             *
             * @template Type the type of the bean which is to be registered
             *
             * @param name std::string the name of the bean to register
             * @param instance Type the bean instance to register
             *
             * @throws InvalidBeanNameException if a bean by that name already exists, or if the bean name is empty
             */
            template<typename Type>
            void registerBeanInstance(std::string name, Type instance) {
                verifyCanAddBean(name);
                m_repo[name] = new BeanInstanceProvider<Type>(instance);
            }

            /*
             * Get a bean from the manager that has been registered under the specified name. If auto
             * registration is enabled, if the request bean does not exist it will automatically be
             * registered. Note that any auto-registered beans will be singleton and must have a default
             * constructor.
             *
             * @template Type the type of the bean that is desired
             *
             * @param name std::string the name of the bean.
             *
             * @returns Type the desired bean
             *
             * @throws InvalidBeanNameException if no bean of the specified name is registered (and auto registration is disabled)
             * @throws InvalidBeanTypeException if the registered bean is of a different type than what is requested
             */
            template<typename Type>
            Type getBean(std::string name) {
                if (!containsBean(name)) {
#ifdef ENABLE_BEAN_AUTOREGISTRATION
			registerBean<Type>(name);
#else
                    throw InvalidBeanNameException(name, "no bean of that name available");
#endif
                }

                // check for cycles
                bool hasCycle = std::find(m_beanNameStack.begin(), m_beanNameStack.end(), name) != m_beanNameStack.end();
                m_beanNameStack.push_back(name);
                if (hasCycle)
                    throw BeanDependencyCycleException(m_beanNameStack);

                BaseProvider *baseProvider = m_repo[name];
                TypeProvider<Type> *typeProvider = dynamic_cast<TypeProvider<Type>*>(baseProvider);

                if (typeProvider) {
                    Type beanToReturn = typeProvider->getBean();
                    // Only pop the bean from the list if it has been retrieved
                    m_beanNameStack.pop_back();
                    return beanToReturn;
                }

                // Do not hold on to invalid bean names in the call stack
                m_beanNameStack.pop_back();
                throw InvalidBeanTypeException(name, typeid(Type).name(), baseProvider->getType());
            }

            /*
             * Convenience method to check if a bean of the given name is already registered.
             *
             * @param name std::string the name of the bean to check for
             *
             * @returns bool true if the bean is already registered
             */
            bool containsBean(std::string name);

        private:
            // Repository of all registered beans
            std::map<std::string, BaseProvider*> m_repo;
            // Stack for "getBean" calls when there is a chained situation. Used to detect cycles
            std::vector<std::string> m_beanNameStack;

            /*
             * Convenience method to check if a bean of the given name can be added to the manager.
             *
             * @param name std::string the name of the bean to check
             *
             * @throws InvalidBeanNameException if a bean by that name already exists, or if the bean name is empty
             */
            void verifyCanAddBean(std::string name);
    };

}

#endif /* BEANMANAGER_H_ */
