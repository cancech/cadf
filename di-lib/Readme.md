# di-lib: Dependency Injection Library

A comprehensive library to facilitate Dependency Injection: the sharing of object instances (beans) across an application in an efficient and straight forward manner. All efforts have been made to minimize the amount of client code (repetitive, boiler plate, or otherwise) required in order to allow bean sharing to take place. It is loosely modelled on the approach taken by Spring for Java, with a breakdown as follows:

* Beans are tracked within a [BeanManager](include/di/BeanManager.h). This associates an instance or a creator (more or this later) to a bean name, and the bean can then be extracted via its name. In this respect the [BeanManager](include/di/BeanManager.h) is at the core of the dependency injection.
* [Configuration](include/di/Configuration.h) classes are to be used to facilitate access to the [BeanManager](include/di/BeanManager.h). Each [Configuration](include/di/Configuration.h) class can pull beans from the [BeanManager](include/di/BeanManager.h) (as resources) and provides beans back into it. Each individual [Configuration](include/di/Configuration.h) can only be created when all of its required resources are available within the [BeanManager](include/di/BeanManager.h), as the resources are treated as private members within the [Configuration](include/di/Configuration.h) class. After instantiation the [Configuration](include/di/Configuration.h) must be initialised, during which time any/all beans it is to provide are created and passed into the [BeanManager](include/di/BeanManager.h).
* A single [BeanManager](include/di/BeanManager.h) and any number of [Configuration](include/di/Configuration.h) classes are then contained within a single [Context](include/di/Context.h). The [Context](include/di/Context.h) is responsible for assembling the [Configurations](include/di/Configuration.h) and ultimately responsible for the life-cycle of the [Configuration](include/di/Configuration.h) and beans within.
* An application can have one or more [Contexts](include/di/Context.h), and as each Context will have its own [BeanManager](include/di/BeanManager.h), each will be independent from one another.

## Context

The [Context](include/di/Context.h) can be viewed as a sandbox within which beans can be shared by means of a [BeanManager](include/di/BeanManager.h) dedicated to the [Context](include/di/Context.h). There is no manner through which to share beans between [Contexts](include/di/Context.h) directly, therefore custom bridging code will have to be provided in order to facilitate that form of sharing.

### Creating a Context

Creating a new [Context](include/di/Context.h) is as simple as creating an instance of the [Context](include/di/Context.h) class. The [Context](include/di/Context.h) will create the [BeanManager](include/di/BeanManager.h) instance for itself, and allow the registration of [Configuration](include/di/Configuration.h) classes. Once all of the desired [Configurations](include/di/Configuration.h) are registered, the [Context](include/di/Context.h) must be assembled at which time it will attempt to create and initialise all registered [Configurations](include/di/Configuration.h). If there is an issue encountered during the assembly (primarily missing resource, type mismatch, or circular dependency between [Configurations](include/di/Configuration.h)) an exception will be thrown indicating where and why assembling the [Context](include/di/Context.h) failed.

```C++
cadf::di::Context sampleContext;
sampleContext.registerConfiguration<Config1, Config2, Config3>();
sampleContext.registerConfiguration<Config4>();
sampleContext.assemble();
```
Note that the [Configuration](include/di/Configuration.h) class instances are not passed into the [Context](include/di/Context.h) (their creation would most likely fail due to missing resources at this stage), but rather their type is passed through the template. Any number of [Configuration](include/di/Configuration.h) class types can be passed in to a single call to `registerConfiguration` (minimum one).

### Destroying a Context

The life-cycle of the context is tied to the life-cycle of the [Context](include/di/Context.h) class. Once that instance is destroyed, all of the registered [Configurations](include/di/Configuration.h) will be destroyed, and any beans which are managed by the [BeanManager](include/di/BeanManager.h) will be destroyed as well.

### Managing Beans Across Contexts

There may be times when it becomes necessary to combine beans that are either shared across different [Contexts](include/di/Context.h), or beans that live outside of any given single [Context](include/di/Context.h). To account for these situations the [Context](include/di/Context.h) class has the ability to register beans directly (inject a bean from outside the context), or to extract beans from within a [Context](include/di/Context.h) to be used outside of it. To facilitate both of these situations, the [Context](include/di/Context.h) actually extends the [BeanManager](include/di/BeanManager.h) allowing for direct access to beans already registered within the [Context](include/di/Context.h) (via `BeanManager::getBean<Type>("name")`), or to manually register one-off beans using either [Creators](include/di/BeanCreator.h) or an existing instance. Note that the bean life-cycle rules do not change for beans that are registered or retrieved in this manner. Meaning:

* Any bean retrieved from a [Context](include/di/Context.h) will be subject to the life-cycle of the [Context](include/di/Context.h) from which it came (as per the subsequent points)
* Destruction of a [Context](include/di/Context.h) will in turn destroy all registered [Creators](include/di/BeanCreator.h), with each [Creator](include/di/BeanCreator.h) determining how to manage any memory it allocates (i.e.: `cadf::di::SingletonBeanCreator` will destroy the instance, whereas `cadf::di::FactoryBeanCreator` leaves the memory management to the client)
* Any instances registered with the [Context](include/di/Context.h) are not handled at all, and it is expected that whomever created the instance will then bear the responsibility for managing its life-cycle.

## BeanManager

As mentioned, the [BeanManager](include/di/BeanManager.h) is responsible for managing the specific beans within the [Context](include/di/Context.h). The [Configurations](include/di/Configuration.h) will pull resources from it, push beans into it, and through this allow for instances of classes or types to be easily passed around the application. To push a bean into the [BeanManager](include/di/BeanManager.h) it must be registered, and registering the bean can be done in one of two ways:

* Instance
* Creator

### Instance

An instance bean is essentially a specific instance of an object or type, that is registered under a name within the [BeanManager](include/di/BeanManager.h). The nature of what this truly means is dependent on what type is registered (pointer, reference, or scalar), with the result being the same as if the type was retrieve via a "get" member function. A pointer or reference will pass the pointer or reference around and thus sharing the same instance across multiple actors, whereas a scalar will be passed via copy with all of the ramifications that that brings with it. The source [Configuration](include/di/Configuration.h) of an instance which allocated the memory, is responsible for de-allocating it upon destruction. Or some other approach taken based on what is applicable in the specific circumstance. The [BeanManager](include/di/BeanManager.h) will not manage any memory of any instance passed into it. To register an instance simply

```C++
BeanManager::registerBeanInstance<Type>("Name", someInstance);
```

Note that the compiler may be able to deduce the type on its own, however this is not a guarantee. For example, when passing a scalar instance the compiler will most likely deduce a scalar type, instead of a reference that may be more desirable. For this reason the Configuration Macros do not take the change and force the type to be specified.

### Creator

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


#### BEAN Autoregistration

As the description of the Bean Creator indicates, the provided Creators only work with beans that provide a default constructor. This limitation does allow for an extra piece of flexibility, specifically that it does not truly matter if a bean was registered prior to asking for it so long as it can be created by one of these provided Creators. If the bean in question can be created without any extra information, it is possible for it to be registered and created at the time of asking for it. This functionality is disabled by default, however this form of auto-registration can be enabled at compile time via `ENABLE_BEAN_AUTOREGISTRATION`. When this flag is defined, when the BeanManager is asked to retrieve a bean which has not been registered yet, it will be registered at that time with a [cadf::di::SingletonBeanCreator](include/di/BeanCreator.h).

Note that `ENABLE_BEAN_AUTOREGISTRATION` is Header Only, meaning that the library itself need not be recompiled in order to change this behaviour.

## Configuration

The [Configuration](include/di/Configuration.h) class can be seen as a recipe. It will require some resources (inputs) and then provide some beans (outputs)

### Configuration Lifecycle

The lifecycle of the [Configuration](include/di/Configuration.h) is managed by the [Context](include/di/Context.h) via the [BeanManager](include/di/BeanManager.h). The [Configuration](include/di/Configuration.h) reports what resources it requires from the [BeanManager](include/di/BeanManager.h) via the static `getResourceNames()`. Once all required resources become available within the [BeanManager](include/di/BeanManager.h) the [Configuration](include/di/Configuration.h) itself is instantiated and initialized. As part of the instantiation resources are to be pulled form the [BeanManager](include/di/BeanManager.h). As part of the initialization, `postInit()` is called where the necessary initialization (bean or otherwise) is to be done using the retrieved resources. Once the initialization is fully completed `provideBeans()` is called where beans provided by the [Configuration](include/di/Configuration.h) are registered with the [BeanManager](include/di/BeanManager.h).

At this point the [Configuration](include/di/Configuration.h) will lie dormant until the termination of the [Context](include/di/Context.h). As the [Context](include/di/Context.h) is destroyed, all [Configurations](include/di/Configuration.h) will be destroyed (via their destructors) and any/all allocated memory should be dealt with as appropriate.

### Defining a Configuration

In order for a [Configuration](include/di/Configuration.h) to be usable within a [Context](include/di/Context.h), it must:

* Extend the `cadf::di::Configuration` class.
* Provide a constructor that matches the exact signature of the base `cadf::di::Configuration` constructor.
* Override the getName() static member function to return a plain text name of the [Configuration](include/di/Configuration.h) class. _(1)_
* Override the getResourceNamess() static member function to return a complete list of names of all resources the [Configuration](include/di/Configuration.h) requires.
* Override the getBeanNames() static member function to return a complete list of all beans the [Configuration](include/di/Configuration.h) will provide. _(1)_

_(1) Strickly speaking these are only used for error checking and debug._

There is nothing preventing following traditional code practices, and manually extend and create the [Configuration](include/di/Configuration.h) class, however that will lead to verbose, repetative, and error prone code. To simplify the process, a series of Macros are provided to ease the process as much as possible.

#### Configuration Macros

A series of Macros are available to facility the definition and creation of [Configuration](include/di/Configuration.h) classes. It is highly recommended to make use of these to simplify the process of creating new [Configurations](include/di/Configuration.h), and to avoid repetitive or boiler plate code. This should also maximise future proofing and decrease the amount of rework required as future updates are released.

##### CONFIGURATION Macro

```C++
CONFIGURATION(ExampleConfiguration)
```
This Macro marks the start of a [Configuration](include/di/Configuration.h) class. It will provide the start of the class definition, and provide an implementation of the static getName() member function. The example above will generate the following:

```C++
class ExampleConfiguration: public cadf::di::BaseConfiguration { \
	public: \
		ConsumerManagerTestConfig(cadf::di::BeanManager* manager): BaseConfiguration(manager) {} \
		static std::string getName() { return "ExampleConfiguration"; }
```

##### DEPENDENCIES Macro

```c++
DEPENDENCIES(SubConfiguration1, SubConfiguration2, SubConfiguration3)
```

A [Configuration](include/di/Configuration.h) can have dependent [Configurations](include/di/Configuration.h), which will also be registered within the [Context](include/di/Context.h). This allows for easy module management, where a root [Configuration](include/di/Configuration.h) for a module can be defined and this will then list dependencies of any/all [Configuration](include/di/Configuration.h) required for the module to function. In this manner the whole module can be registered with a [Context](include/di/Context.h) by simply registering the core [Configuration](include/di/Configuration.h). Dependencies of dependencies will also be registered.

Note that when using this Macro at least one dependent [Configuration](include/di/Configuration.h) must be present, and it will support up to 256 dependent [Configuration](include/di/Configuration.h). If more than 256 dependent Configurations are required for a single [Configuration](include/di/Configuration.h), it must be broken into nested layers of dependencies (top most [Configuration](include/di/Configuration.h) can have up to 256 dependencies, each of which can have up to 256 dependencies, each of those can have an additional 256 dependencies,... ad nausea).

This Macro will ultimately provide an implementation of the static `getDependentConfigurations()` member function, and generate code which looks like the following:

```C++
static std::vector<corm::ConfigurationWrapperInterface*> getDependentConfigurations(cadf::di::BeanManager* manager) { \
		std::vector<cadf::di::ConfigurationWrapperInterface*> deps; \
		deps.push_back(new cadf::di::ConfigurationWrapper<SubConfiguration1>(manager)); \
		deps.push_back(new cadf::di::ConfigurationWrapper<SubConfiguration2>(manager)); \
		deps.push_back(new cadf::di::ConfigurationWrapper<SubConfiguration3>(manager)); \
		return deps; \
}
```

##### BEANS Macro

```C++
  BEANS(
    (BEAN, SomeClass&, "someClassBean"),
    (BEAN_INSTANCE, CustomClass*, instance)
  )
 ```
 This Macro is available to shorthand the process of registering beans with the [BeanManager](include/di/BeanManager.h), as well as providing an implementation for the static `getBeanNames()` member function. Each bean to be registered must be contained within a set of brackets, however there is some flexibility in terms of what and how the beans are registered. The first parameter for each bean is the Bean Type:
 
 * BEAN indicates a bean which is to be managed via a [Bean Creator](include/di/BeanCreator.h)
 * BEAN_INSTANCE indicates registering a member within the [Configuration](include/di/Configuration.h) as an instance
 
 For BEAN the remaining parameters are as follows:
 
 * Type - indicating the type which is to be registered
 * Creator - Optionally indicating the type of Creator to use for the bean. If not present it will default to `cadf::di::SingletonBeanCreator`
 * Bean Name - under what name the bean should be registered
 
 For BEAN_INSTANCE the remaining parameters are as follows:
 
 * Type - indicating the type which is to be registered
 * Bean Name - Optionally indicating what name to register the instance under. If not present, it will default to the variable name of the instance (in the above example the bean name will be "instance")
 * Instance - the specific instance which is to be registered as a bean
 
 Note that for BEAN_INSTANCE the Instance can be any instance of the desired type. This could be a reference to a member variable (as per the example), or creating the instance when registering. For example

```C++
  BEANS(
    (BEAN_INSTANCE, int, "someint", 123),
    (BEAN_INSTANCE, int, "somestring", "This is a String"),
    (BEAN_INSTANCE, SomeClass, "someclass", new SomeClass(1, 2, 3))
  )
```
are all valid, though note the possible memory leak on the someclass bean instance.

The original example of

```C++
  BEANS(
    (BEAN, SomeClass&, "someClassBean"),
    (BEAN_INSTANCE, CustomClass*, instance)
  )
```
will generate code which looks as follows

```C++
protected: \
	void provideBeans() { \
		m_beanManager->registerBean<SomeClass&>("someClassBean"); \
		m_beanManager->registerBeanInstance<CustomClass*>("instance", instance); \
	} \
public: \
	static const std::vector<std::string>& getBeanNames() { \
		static std::vector<std::string> beanNames { \
			"someClassBean", \
			"instance", \
		}; \
		return beanNames; \
	}
```

Note that up to 256 beans can be registered in a single Configuration through the means indicated here.

##### RESOURCES Macro

```C++
  RESOURCES(
    (int, intbean),
    (std::string&, "beanName", stringBean),
    (SampleClass*, sampleClassInstance)
  )
```
This Macro will shorthand the process of registering resource dependencies, as well as the static `getResourceNames()` member function. Much like the BEANS Marco each required resource must be contained within a set of brackets, and the values provided for each resource are as follows:

* Type - indicating what type of resource is expected to be retrieved from the [BeanManager](include/di/BeanManager.h)
* Bean Name - Optionally indicating the name of the bean to retrieve. If not present the bean name is derived from the variable name.
* Variable Name - name of the variable where the bean is to be stored

This will create all of the specified resources as members within the Configuration and load each of them from the [BeanManager](include/di/BeanManager.h). The example will generate the following code:

```C++
private: \
	int  intbean = m_beanManager->getBean<int>("intbean"); \
	std::string&  stringBean = m_beanManager->getBean<std::string&>("beanName"); \
	SampleClass*  sampleClassInstance = m_beanManager->getBean<SampleClass*>("sampleClassInstance"); \
public: \
	static const std::vector<std::string>& getResourceNames() { \
		static std::vector<std::string> resourceNames { \
			"intbean", \
			"beanName", \
			"sampleClassInstance" \
		}; \
		return resourceNames; \
	}
```
Note that up to 256 resources can be specified in a single Configuration in the manner as shown here.

#### END_CONFIGURATION Macro

```C++
END_CONFIGURATION
```
This Macro is present to maintain the symmetry of the Configuration definition, and it merely places an end to the class. Meaning that it just simply converts to 
```C++
};
```
### Example Configuration

The following is the complete Configuration as described above.

```c++
/*
 * Create a new configuration via the CONFIGURATION macro. The parameter passed to it (ExampleConfiguration in this case)
 * is the name of this Configuration class, and what will later be registered witin a Context.
 */
CONFIGURATION(ExampleConfiguration)

  // Include dependent Configurations of this Configuration (more details below)
  DEPENDENCIES(SubConfiguration1, SubConfiguration2, SubConfiguration3)

public:
  ~ExampleConfiguration() {
    // The Configuration is responsible for managing any memory it allocates
    delete(instance);
  }

protected:

  /*
   * postInit is called during the initialization process of the Configuration. It is here that any processing
   * of resources should take place. The processing need not result in any beans, but this is where any bean
   * instances which are to be then shared are to be initialized.
   */
  void postInit() {
    instance = new CustomClass(intBean, stringBean, sampleClassInstance);  
  }
  
  // Through the BEANS Macro indicate what beans are to be provided (more details below)
  BEANS(
    (BEAN, SomeClass&, "someClassBean"),
    (BEAN_INSTANCE, CustomClass*, instance)
  )

  // Through the RESOURCES Macro indicate what resources the configuration requires (more details below)
  RESOURCES(
    (int, intbean),
    (std::string&, stringBean),
    (SampleClass*, sampleClassInstance)
  )
  
private:
  CustomClass* instance;

END_CONFIGURATION
```

## Example

To show how this all comes together a few [Configurations](include/di/Configuration.h) files and a [Context](include/di/Context.h) will be included as a contrived example. So to start with, we need some [Configurations](include/di/Configuration.h)

Configuration1: provides some ints. Note that this does not require any resources so the RESOURCES macro is absent
```C++
// Config which requires a single resource from ProviderManagerTestConfig and provides a single bean
CONFIGURATION(Configuration1)

	BEANS(
		(BEAN, int, Example123Creator, "int1"),
		(BEAN_INSTANCE, int, "int2", 456),
		(BEAN_INSTANCE, int, "int3", 789)
	)

END_CONFIGURATION
```

Configuration2: takes ints to create a vector
```C++
// Config which requires three resources (provided by Configuration1) and provides a single bean
CONFIGURATION(Configuration2)

private:
	std::vector<int> allInts;

protected:
	void postInit() {
		// Called during initialization after resource are available
		allInts.push_back(int1);
		allInts.push_back(int2);
		allInts.push_back(int3);
	}

	BEANS(
		(BEAN_INSTANCE, std::vector<int>&, allInts)
	)

	RESOURCES(
		(int, int1),
		(int, int2),
		(int, int3)
	)

END_CONFIGURATION
```

Configuration3: specifies Configuration1 and Configuation2 as dependencies, and adds some more ints to the vector
```C++
CONFIGURATION(Configuration3)

DEPENDENCIES(Configuration1, Configuation2)

protected:
	void postInit() {
		m_allInts.push_back(13579);
		m_allInts.push_back(24680);
	}

	RESOURCES(
		(std::vector<int>&, "allInts", m_allInts)
	)
	
END_CONFIGURATION
```

Create a context which loads the above three Configurations
```C++
cadf::di::Context context;
context.registerConfiguration<Configuration3>();
context.assemble();

// Retrieve the vector from the context
context.getBean<std::vector<int>>("allInts");
```

## Under the Hood

The core architecture centres around a map (repository) in [BeanManager](include/di/BeanManager.h) where all registered beans are stored. The key to the map is the bean name (under which the bean is registered), however the value is not the bean itself. At least not directly. Instead there are some layers of abstraction in place to ensure a uniform and simple means of accessing the beans. 

### Provider Classes

There are a series of `Provider` classes (defined in [BeanProvider.h](include/di/BeanProvider.h)), which are responsible for the abstraction and mechanism of storing and accessing the beans themselves.

#### BaseProvider

The repository (bean map) contains an instance of `BaseProvider`, the top most parent of the `Provider` classes, which is a pure virtual class (interface). The purpose of `BaseProvider` is be the anchor of the concrete `Provider` class that can be stored in the repository. The concrete `Provider` classes are template classes, and multiple template classes of different types cannot be stored in the same map in C++. Or to put it a different way, the map must define which exact type of the template class it can store within itself. There is no "any" template wildcard that other languages such as Java allow. As the `BaseProvider`'s role is to act purely as a placeholder, there is little to no functionality available to it. In fact the only member function it provides the the `getType()` which is a pure virtual member function to retrieve the name of the type of the bean stored within as a plain string for error reporting and debugging purposes.

#### TypeProvider

As the first subclass of the `BaseProvider`, the `TypeProvider`'s role is to manage and account for the type of the bean. It is a template class whose type corresponds to the type of the bean. The ultimate role of the `TypeProvider` is to track the type of the bean, but to abstract where the bean comes from or how it is created. In a perfect world the `getBean()` member function would be pure virtual, however C++ does not allow for virtual template member functions. To get around this limitation `getBean()` is fully implemented in `TypeProvider` but it relies on the concrete subclass to provide the bean instance through the pure virtual `provideBean()` member function that the concrete provider must implement. `provideBean()` is then called from within `getBean()` with the expectation that it will populate the `m_beanWrapper` as a `new`, after which `getBean()` retrieves the bean itself and cleans up the temporary memory `m_beanWrapper`.

#### BeanCreatorProvider

This is a concrete Provider which relies on a template `Creator` class to determine how the beans are to be created/managed. It does virtually nothing, other than associate `TypeProvider::m_beanWrapper` with the `Creator`. The `BeanCreatorProvider` can work with any `Creator`, so long as the `Creator` matches the appropriate qualifications:

* The `Creator` has a default constructor.
* The `Creator` has a public no argument `create()` member function.
* The `Creator::create()` member function returns the bean instance as a `ValueWrapper<T>*` allocated via new.

#### BeanInstanceProvider

This is a concrete Provider which provides the specific instance it was created with. It always provides the same instance, meaning:

* Pointers are effectively singleton.
* References are effectively singleton.
* Scalars are, as per the scalar norm, effectively factories.

#### ValueWrapper

The ValueWrapper is a supporting class, whose purpose is to act as the middleman between the concrete `Provider` and the `TypeProvider`, ensuring that the exact same instance that the `Provider` or `Creator` wanted to provide is what the `TypeProvider` received. Issues were encountered with references becoming scalar at some point along the way in the translation (i.e.: rather than being a reference to the original instance it became its own instance). The `ValueWrapper` ensures that this does not happen. It is an extremely simple struct, designed to server this exact purpose only.

### BeanManager

The `BeanManager` is at the core of DI functionality, as it tracks all of the beans and allows for them to be added and retrieved. It is here that the `Providers` explained above are put into use. 

#### Bean Registration

When a bean is registered, depending on how it was registered a concrete provider will be created for it.

* `registerBean()` will create a `BeanCreatorProvider` using whichever `Creator` the client code specifies (defaulting to `SingletonBeanCreator` if no default was specified)
* `registerBeanInstance()` will create a `BeanInstanceProvider` using whichever instance was specified.

If the bean can be added (a series of checks are performed to ensure no duplicate beans and the like), the created concrete `Provider` is then added to the repository under the specified bean name.

#### Bean Retrieval

The retrieval of beans is slightly more complex, however ultimately still straight forward. Regardless of how the bean was registered, or what concrete `Provider` it has, the same mechanism for retrieval is employed. Before anything else can be done, a quick check is performed to ensure that the bean request is in fact already registered. If it isn't, if Bean Autoregistration is enabled it is registered using the default `Creator`, or an exception is thrown.

The act of retrieving the bean itself is about what one can expect, get the appropriate `Provider` from the repository, get the bean from it, and return it. The only extra comes in the form of `m_beanNameStack` which is in place to allow for cycle dependency checking. It is possible that attempting to create one bean will cause it to attempt to retrieve another bean, and so on, meaning that it is entirely possible for a cycle to exist where a bean will inadvertently be dependent on itself (i.e.: beanA requires beanB, which requires beanC, which required beanA). The cycle check will determine when this happens, throwing an exception indicating where the cycle exists.

### Configuration

Where `BeanManager` is the core, the `Configuration` is the cornerstone. The expectation is that the client code will never touch upon the `BeanManager` directly, but rather perform all bean processing by means of `Configuration` classes. These classes allow the client to retrieve beans as resources and provide new beans in turn. The `Configuration` manages any/all resources it requires as private members, meaning that the `Configuration` cannot be instantiated until all required resources are available. To facilitate with this process a `ConfigurationWrapper` is created for each `Configuration` when it is registered.

#### ConfigurationWrapper

As the name implies, the `ConfigurationWrapper` is a wrapper class for the `Configuration`. Since the `Configuration` itself cannot be initialised until after all required resources are available in the `BeanManager`, the `ConfigurationWrapper` is used to track what resources the `Configuration` is waiting on. The `Configuration` provides a list of resources it requires via the static `getResourceNames()` member function, the `ConfigurationWrapper` stores all of these names as `m_waitingResources` and during each `areResourcesSatisfied()` check determines which of the waiting resources have become available. Once `m_waitingResources` has been depleted, the `Configuration` has all of its resources fulfilled, and it can be initialised. This is the main duty of the `ConfigurationWrapper`, however it acts as an intermediary for the uninitialised `Configuration` in order debug related means. Ultimately until the `Configuration` itself has been initialised, the `ConfigurationWrapper` is the only way through which to retrieve any information about the `Configuration`. In this manner it can be considered to act as the metadata for the uninitialised `Configuration`.

#### ConfigurationWrapperInterface

The `ConfigurationWrapper` is a template class, with the template being the `Configuration` class that it is wrapping. As per what was described in the `Provider` section above, there is no "any" template wildcard, meaning that for the `Context` to track the `ConfigurationWrapper` it must know what exact type the template is. This makes dealing with `ConfigurationWrappers` in a uniform or simple manner impossible, which is where the `ConfigurationWrapperInterface` comes in. It is a pure virtual class, that provides as pure virtual member functions all of the necessary features that the `Context` requires to interact fully with the `ConfigurationWrapper`, without needing to rely on the `Configuration` class type. Thus `ConfigurationWrapperInterface` is what the `Context` will primarily see.

### Context

Where `BeanManager` is the core, and `Configuration` is the cornerstone, the `Context` is the glue that holds everything together. It is the `Context` that manages `Configurations`, and that provides the `BeanManager` that the `Configurations` use. In that respect it has a singular purpose: take in any number of `Configurations` and process them. Registering a `Configuration` will create the `ConfigurationWrapper` for it (and all of its dependent `Configurations`) and when all have been registered via `assemble()` it will attempt to initialise all registered `Configurations`.

#### Assemble Configuration

The process of assembling the `Context` is ultimately rather straight forward. It continuously iterates across all of the `Configurations` until a point is reached where no `Configuration` is loaded. This can occur in one of two situations:

* All `Configurations` has been loaded - SUCCESS!!
* All remaining `Configurations` have unfulfilled resources - FAILURE!!

If successful, this means that the `Context` is sane, and the application can proceed forward with its purpose. If there are unfulfilled resources, that means that not all `Configurations` have been able to retrieve their necessary resources, which means either a resource is missing from the `Context`, or a dependency cycle exists between one or more `Configurations`. The `verifyContext()` will determine which is it, making use of the `CircularDependencyChecker` to find any cycles.

#### CircularDependencyChecker

The `CircularDependencyChecker` is populated with the details of all of the `Configurations` that are as of yet waiting to be resolved. Namely:

* The name of the configuration (for reporting and tracking purposes)
* The list of resources it is still waiting on
* The list of beans that it provides

all of which is retrieved from the `ConfigurationWrapperInterface` of the pending `Configurations`. To determine whether a cycle is in place, the `CircularDependencyChecker` iterates across each `Configuration` and follows each of its resources. If it determines that it has previously visited the `Configuration`, a cycle exists and the details of the cycle are reported.
