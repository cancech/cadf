# di-lib: Dependency Injection Library

A comprehensive library to facilitate Dependency Injection: the sharing of object instances (beans) across an application in an efficient and straight forward manner. All efforts have been made to minimize the amount of client code (repetitive, boiler plate, or otherwise) required in order to allow bean sharing to take place. It is loosely modelled on the approach taken by Spring for Java, with a breakdown as follows:

* Beans are tracked within a [BeanManager](include/di/BeanManager.h). This associates an instance or a creator (more or this later) to a bean name, and the bean can then be extracted via its name. In this respect the [BeanManager](include/di/BeanManager.h) is at the core of the dependency injection.
* [Configuration](include/di/Configuration.h) classes are to be used to facilitate access to the [BeanManager](include/di/BeanManager.h). Each [Configuration](include/di/Configuration.h) class can pull beans from the [BeanManager](include/di/BeanManager.h) (as resources) and provides beans back into it. Each individual [Configuration](include/di/Configuration.h) can only be created when all of its required resources are available within the [BeanManager](include/di/BeanManager.h), as the resources are treated as private members within the [Configuration](include/di/Configuration.h) class. After instantiation the [Configuration](include/di/Configuration.h) must be initialised, during which time any/all beans it is to provide are created and passed into the [BeanManager](include/di/BeanManager.h).
* A single [BeanManager](include/di/BeanManager.h) and any number of [Configuration](include/di/Configuration.h) classes are then contained within a single [Context](include/di/Context.h). The [Context](include/di/Context.h) is responsible for assembling the [Configuration](include/di/Configuration.h)s and ultimately responsible for the life-cycle of the [Configuration](include/di/Configuration.h) and beans within.
* An application can have one or more [Context](include/di/Context.h)s, and as each Context will have its own [BeanManager](include/di/BeanManager.h), each will be independent from one another.

# BeanManager

As mentioned, the [BeanManager](include/di/BeanManager.h) is responsible for managing the specific beans within the [Context](include/di/Context.h). The [Configuration](include/di/Configuration.h)s will pull resources from it, push beans into it, and through this allow for instances of classes or types to be easily passed around the application. To push a bean into the [BeanManager](include/di/BeanManager.h) it must be registered, and registering the bean can be done in one of two ways:

* Instance
* Creator

## Instance

An instance bean is essentially a specific instance of an object or type, that is registered under a name within the [BeanManager](include/di/BeanManager.h). The nature of what this truly means is dependent on what type is registered (pointer, reference, or scalar), with the result being the same as if the type was retrieve via a "get" member function. A pointer or reference will pass the pointer or reference around and thus sharing the same instance across multiple actors, whereas a scalar will be passed via copy with all of the ramifications that that brings with it. The source [Configuration](include/di/Configuration.h) of an instance which allocated the memory, is responsible for de-allocating it upon destruction. Or some other approach taken based on what is applicable in the specific circumstance. The [BeanManager](include/di/BeanManager.h) will not manage any memory of any instance passed into it. To register an instance simply

```C++
BeanManager::registerBeanInstance<Type>("Name", someInstance);
```

Note that the compiler may be able to deduce the type on its own, however this is not a guarantee. For example, when passing a scalar instance the compiler will most likely deduce a scalar type, instead of a reference that may be more desirable. For this reason the Configuration Macros do not take the change and force the type to be specified.

## Creator

This is a more interesting means through which to manage beans. In this situation no instance is created outright, but rather a Creator is specified to indicate how the bean is to be created and managed. There are several Creators provided (see [BeanCreator.h](include/di/BeanCreator.h)):

* [cadf::di::SingletonBeanCreator<Type>](include/di/BeanCreator.h) - creates a singleton instance which is provided as either a pointer or reference (note that scalar is not supported). The creator will ensure that the singleton instance is destroyed when the creator is destroyed (it will only be destroyed when the [BeanManager](include/di/BeanManager.h) is destroyed when the [Context](include/di/Context.h) is destroyed)
* [cadf::di::FactoryBeanCreator<Type>](include/di/BeanCreator.h) - creates a new instance each time the bean is resourced, returning either a scalar or a pointer (note that reference is not supported). The creator does not track created instances, and thus will not clean up any memory upon destruction. For this reason it is recommended to use this with scalars to avoid memory leaks.
* [cadf::di::SmartSingleBeanCreator<Type, Ptr>](include/di/BeanCreator.h) - equivalent to [cadf::di::SingleBeanCreator](include/di/BeanCreator.h), except that the single pointer is contained within a smart pointer. The type of smart pointer can be specified via the Ptr template and it will default to std::shared_ptr.
* [cadf::di::SmartFactoryBeanCreator<Type, Ptr>](include/di/BeanCreator.h) - equivalent to [cadf::di::FactoryBeanCreator](include/di/BeanCreator.h), except that the created instance wrapped within a smart pointer. The type of smart pointer can be specified via the Ptr template and it will default to std::shared_ptr.

The type of creator to use is specified when the bean is registered

```C++
BeanManager::registerBean<Type, Creator>("Name");
```

Note that the for the provided creators the bean is not created until the bean is requested for the first time, meaning that if the bean is not resourced the creator will not create it.

It is possible and easy to create custom creators. There are however limitations:

* The creator must have a default constructor. No other constructor is supported or will be used by the BeanManager
* Must contain a create member function `cadf::di::ValueWrapper<Type>* create()`

The [ValueWrapper<Type>](include/di/BeanCreator.h) is employed to work around limitations of templates within C++, specifically to ensure that type references are not broken while moving through the [BeanManager](include/di/BeanManager.h). For example the following is a custom creator which will create int 123 each time it is called.

```C++
struct Example123Creator {
	corm::ValueWrapper<int> create() {
		return 123;
	}
};
```

It could then be used to create a bean as simple as 

```C++
m_beanManager->registerBean<int, Example123Creator>("123Bean");
```

or within a Configuration file

```C++
BEANS(
	(BEAN, int, Example123Creator, "123Bean")
)
```


### BEAN Autoregistration

As the description of the Bean Creator indicates, the provided Creators only work with beans that provide a default constructor. This limitation does allow for an extra piece of flexibility, specifically that it does not truly matter if a bean was registered prior to asking for it so long as it can be created by one of these provided Creators. If the bean in question can be created without any extra information, it is possible for it to be registered and created at the time of asking for it. This functionality is disabled by default, however this form of auto-registration can be enabled at compile time via `ENABLE_BEAN_AUTOREGISTRATION`. When this flag is defined, when the BeanManager is asked to retrieve a bean which has not been registered yet, it will be registered at that time with a [cadf::di::SingletonBeanCreator](include/di/BeanCreator.h).

Note that `ENABLE_BEAN_AUTOREGISTRATION` is Header Only, meaning that the library itself need not be recompiled in order to change this behaviour.
