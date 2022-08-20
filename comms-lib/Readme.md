# comms-lib: Communications Library

Provides a means of communication between distributed nodes.

## Architecture

Each communicating entity is a [cadf::comms::Node](include/comms/node/Node.h) and they connect to each other via a shared [cadf::comms::IBus](include/comms/bus/Bus.h) rather than directly to each other.

```
__________________________________________________________
|                          Bus                           |
----------------------------------------------------------
     |             |             |                  |
___________   ___________   ___________        ___________
| Node #1 |   | Node #2 |   | Node #3 | ...... | Node #N |
-----------   -----------   -----------        -----------
```

Each [Node](include/comms/node/Node.h) is registered to the desired [Bus](include/comms/bus/Bus.h), and the [Bus](include/comms/bus/Bus.h) is responsible for message passing, routing sent messages to the desired recipients. For this purpose each [Node](include/comms/node/Node.h) has an address in the form of:

* _Type_ - a positive integer through which to identify a unique type of node in the system
* _Instance_ - a positive integer through which to identify an instance of a type, if there are multiples present

The _Type_ and _Instance_ combination of a [Node](include/comms/node/Node.h) must be unique within a [Bus](include/comms/bus/Bus.h), and these values are passed as part of the registration process. When sending a message the _Type_ and _Instance_ of the desired recipient is provided, and the [Bus](include/comms/bus/Bus.h) uses those values to route the message to the desired recipient [Node](include/comms/node/Node.h). For the purpose of destination addressing, a wildcard [cadf::comms::ConnectionConstants::BROADCAST](include/comms/Constants.h) can be employed to sent the message to all available [Nodes](include/comms/node/Node.h). This [BROADCAST](include/comms/Constants.h) wildcard can be used for both _Type_ and _Instance_, whether both at the same time or individually as desired.

### Nodes

The [Node](include/comms/node/Node.h) itself is comprised of several parts, namely:

* The [Node](include/comms/node/Node.h) itself
* The [Connection](include/comms/connection/Connection.h) which determines how the [Node](include/comms/node/Node.h) connects to the [Bus](include/comms/bus/Bus.h)
* A [Processor](include/comms/node/Processor.h) that will digest any message received by the [Node](include/comms/node/Node.h) from the [Bus](include/comms/bus/Bus.h)

In this manner the [Node](include/comms/node/Node.h) acts as the glue holding the functionality together, without any true concept as to how the connection to the bus is managed (responsibility of the [Connection](include/comms/connection/Connection.h)), nor what to do with any messages that are received (responsibility of the [Processors](include/comms/node/Processor.h)). If a message is received by the [Node](include/comms/node/Node.h) for which no [Processor](include/comms/node/Processor.h) exists, the message is discarded.

There are some specialized [Nodes](include/comms/node/Node.h) provided that are available for use.

#### Bridge Node

A [cadf::comms::BridgeNode](include/comms/node/BridgeNode.h) is a special [Node](include/comms/node/Node.h) that allows for the bridging between two [Buses](include/comms/bus/Bus.h).

```
___________   ___________   ___________        ___________
| Node #A |   | Node #B |   | Node #C | ...... | Node #Z |
-----------   -----------   -----------        -----------
     |             |             |                  |
__________________________________________________________
|                          Bus                           |
----------------------------------------------------------
                            |
                     _______________
                     | Bridge Node |
                     ---------------
                            |
__________________________________________________________
|                          Bus                           |
----------------------------------------------------------
     |             |             |                  |
___________   ___________   ___________        ___________
| Node #1 |   | Node #2 |   | Node #3 | ...... | Node #N |
-----------   -----------   -----------        -----------
```

Unlike a traditional [Node](include/comms/node/Node.h), the [Bridge Node](include/comms/node/BridgeNode.h) registers with (thus connects to) two [Buses](include/comms/bus/Bus.h) rather than just the one, and consequently it will have a _Type_ and _Instance_ on each (can be the same on both, or different, just must be a unique combination within each [Bus](include/comms/bus/Bus.h)). One [Bus](include/comms/bus/Bus.h) is identified as _External_ and the other _Internal_. In its capabity as a bridge, it will facilitate the forwarding of messages from one [Bus](include/comms/bus/Bus.h) to the other. When configuring the [Bridge Node](include/comms/node/BridgeNode.h) forwarding rules must be specified in both directions, where a rules takes the form of 

```
MessageType forwards to Type and Instance
```

Meaning when a given type of message is received, the [Bridge Node](include/comms/node/BridgeNode.h) will forward it to indicated _Type_ and _Instance_ on the other [Bus](include/comms/bus/Bus.h). Forwarding rules from the _External_ to the _Internal_ [Bus](include/comms/bus/Bus.h) are specified via `addForwardToInternalRule(std::string messageType, int nodeType, int nodeInstance)` and vice versa from _Internal_ to _External_ are specified via `addForwardToExternalRule(std::string messageType, int nodeType, int nodeInstance)`. The [BROADCAST](include/comms/Constants.h) wildcard can be employed in the forwarding rules, and if no rule is defined for a given _MessageType_ it will not be forwarded.

## Local Bus

The simplist approach toward creating this form of application, is if the [Bus](include/comms/bus/Bus.h) and all [Nodes](include/comms/node/Node.h) live within the same application (such as via plugins, where each plugin is a separate [Node](include/comms/node/Node.h)). For this purpose [cadf::comms::LocalBasicBus](include/comms/bus/LocalBasicBus.h) and [cadf::comms::LocalThreadedBus](include/comms/bus/LocalThreadedBus.h) are provided, with the distinction that [LocalThreadedBus](include/comms/bus/LocalThreadedBus.h) incorporates a [ThreadPool](../thread-lib) to perform message routing in a separate thread from the sender, while [LocalThreadedBus](include/comms/bus/LocalThreadedBus.h) performs the message routing in the same thread as the sender. To create a Local Bus, simply call the constructor of the desired class, no additional initialization required.

```C++
cadf::comms:LocalBasicBus nonThreadedBus;
cadf::comms::LocalThreadedBus threadedBus(myThreadPool);
```

To create a [Node](include/comms/node/Node.h) and register it with the local bus of choice, simply instantiate the [Node](include/comms/node/Node.h) with a [cadf::comms::LocalConnection](include/comms/connection/LocalConnection.h). As part of the [LocalConnection](include/comms/connection/LocalConnection.h) initialization, it must be registered with the desired [Bus](include/comms/bus/Bus.h). The last step to take participate in message passing on the [Bus](include/comms/bus/Bus.h) is to connect to it.

```C++
cadf::comms::LocalConnection myConnection(myType, myInstance, myMsgFactory);
myConnection.registerBus(&nonThreadedBus);
cadf::comms::Node myNode(&myConnection);
myNode.connect();
```

To remove the [Node](include/comms/node/Node.h) from the [Bus](include/comms/bus/Bus.h), simply disconnect

```C++
myNode.disconnect();
```

Destroying the [Node](include/comms/node/Node.h) will automatically trigger a disconnection as well.

## Remote Bus

With a remote bus, the [Bus](include/comms/bus/Bus.h) and [Nodes](include/comms/node/Node.h) are distributed (could be different machines, containers, or what have you) and require a network connection in order to communicate with each other. While the principle described above for the Local Bus is largely unchanged, the inclusion of network communication by necessity translates into significantly increased complexity.

### Protocol

The protocol is the mechanism through which data is serialized and deserialized to/from the network. Needless to say both sides of the communication must use the same protocol in order to be able to understand each other. A protocol is defined by extending the provided [cadf::comms::Protocol](include/comms/network/serializer/TemplateProtocol.h) through which a [cadf::comms::ISerializerFactory](include/comms/network/serializer/Serialier.h) (can extend the provided [cadf::comms::TemplateSerializerFactory](include/comms/network/serializer/TemplateProtocol.h) is created for the purpose of serializing a specific message to the desired network format, and a [cadf::comms::IDeserializer](include/comms/network/serializer/Serialier.h) to deserialize network data into program data. Some procotols are provided:

* [cadf::comms::binary::BinaryProtocol](include/comms/network/serializer/binary/Serializer.h) for copying the data directly onto the network
* [cadf::comms::domm::json::JSONProtocol](include/comms/network/serializer/dom/JsonSerializer.h) for converting the data into a JSON data structure
* [cadf::comms::local::LocalProtocol](include/comms/network/serializer/local/Serializer.h) dummy protocol for use with a Local bus to allow for the templates to be properly filled, yet which does nothing (in fact will generate exceptions is any attempt at (de)serialization is made)

The protocol to be employed is generally specified via a template parameter `<PROTOCOL>` in which case reference to the specific [cadf::comms::Protocol](include/comms/network/serializer/TemplateProtocol.h) extension that is to be used.

#### Message Definition

Messages must extend from [cadf::comms::IMessage](include/comms/message/Message.h), though more specifically [cadf::comms::AbstractMessage](include/comms/message/Message.h). [AbstractMessage](include/comms/message/Message.h) is a templated class, where the data the message is to contain is specified via the template parameter with the expectation being that it will be a `struct`. Thus to create a new message, simply extend [AbstractMessage](include/comms/message/Message.h)

```C++
struct MyData {
     int myIntValue;
     std::string myStringValue;
     MyCommonDataStruct myCommonData;
};

class MyMessage: public cadf::comms::AbstractDataMessage<MyData> {
public:
     MyMessage(): cadf::comms::AbstractDataMessage<MyData>("MyMessage", MyData()) {}
     MyMessage(const MyData &data): cadf::comms::AbstractDataMessage<MyData>("MyMessage", data) {}
     
protected:
     AbstractDataMessage<MyData> newInstance() const {
          return new MyMessage(m_data);
     }
};
```

_Note: a default constructor __must__ be provided and the __newInstance()__ method implemented. Anything beyond that is up to target design requirements._

#### Message Serialization and Deserialization

For the provided protocols each message data `struct` needs to provide serialization and deserialization overloads.

##### Binary Protocol

The [Binary Protocol](include/comms/network/serializer/binary/Serializer.h) being an excedingly simply protocol, where the data is copied in its binary format to the message, requires little in the way of "massaging". The protocol provides mechanisms for most common situations, though overrides must be provided for custom data types. The process is started from [cadf::comms::binary::MessageSerializer](include/comms/network/serializer/binary/Serializer.h) and [cadf::comms::binary::MessageDeserializer](include/comms/network/serializer/binary/Serializer.h) for serialization and deserialization (respectively). These then make use of [cadf::comms::binary::DataSerializer](include/comms/network/serializer/binary/Serializer.h) templates classes to perform the actual serialization and deserialization work. Overloads of the [DataSerializer](include/comms/network/serializer/binary/Serializer.h) are provided to account for a number of different data types, including:

* Plain Old Datatypes
* Strings
* Pointers
* Arrays
* Standard iteratables (i.e.: `vector`, `map`)

For anything else, custom [DataSerializer](include/comms/network/serializer/binary/Serializer.h) overrides must be provided, such that `cadf::comms::binary::sizeOfData()`, `cadf::comms::binary::serializeData`, and `cadf::comms::binary::deserializeData` are implemented for the data type in question.

```C++
template<>
size_t cadf::comms::binary::sizeOfData<MyData>(const MyData &data) {
     return sizeOf<int>(data.myIntValue) + sizeOf<std::string>(data.myStringValue) + 
          sizeOfData<MyCommonDataStruct>(data.myCommonData);
}

template<>
void cadf::comms::binary::serializeData<MyData>(const MyData& data, cadf::comms::OutputBuffer *buffer) {
    serializeData<int>(data.myIntValue, sizeof(int));
    serializeData<std::string>(data.myStringValue, buffer);
    serializeData<MyCommonDataStruct>(data.myCommonData, buffer);
}

template<>
MyData cadf::comms::binary::deserializeData<TestData>(cadf::comms::InputBuffer *buffer) {
    MyData data;
    data.myIntValue = deserializeData<int>(buffer);
    data.myStringValue = deserializeData<std::string>(buffer);
    data.myCommonValue = deserializeData<MyCommonDataStruct>(buffer);
    return data;
}
```

##### JSON Protocol

Serializaing into JSON for the [JSONProtocol](include/comms/network/serializer/dom/JsonSerializer.h) is an overall more complex task, requiring the use of the [dom-lib](../dom-lib) library. As such the (de)serialization process employs the [dom-lib](../dom-lib) library to perform the brunt of the heavy lifting, with the (de)serializor populating or pulling from the DOM tree. The process is started from [cadf::comms::dom::json::JsonSerializer](include/comms/network/serializer/dom/JsonSerializer.h) and [cadf::comms::dom::json::JsonDeserializer](include/comms/network/serializer/dom/JsonSerializer.h) for serialization and deserialization respectively. The brunt of the (de)serialization work is handled by two functions that must be implemented for every data type [cadf::comms:dom::buildTree()](include/comms/network/serializer/dom/SerializerFuncs.h) and [cadf::comms::dom::loadFromTree()](include/comms/network/serializer/dom/SerializerFuncs.h), where [buildTree()](include/comms/network/serializer/dom/SerializerFuncs.h) is expected to use the [dom-lib](../dom-lib) to build a DOM tree representation of the data, while conversely [loadFromTree()](include/comms/network/serializer/dom/SerializerFuncs.h) loads the data from a DOM tree representation. The rest of the (de)serialization is performed internally by using the [dom-lib](../dom-lib) to convert the DOM tree to/from a JSON string.

```C++
template<>
cadf::dom::DomNode cadf::comms::dom::buildTree<MyData>(const MyData &data) {
    cadf::dom::DomNode node = cadf::dom::buildNode("myIntValue", data.myIntValue);
    node["myStringValue"] = data.myStringValue;
    node["myCommonData"] = buildTree<MyCommonDataStruct>(data.myCommonData);
    return node;
}

template<>
MyData cadf::comms::dom::loadFromTree<MyData>(const cadf::dom::DomNode &root) {
    MyData data;
    data.myIntValue = root["myIntValue"];
    data.myStringValue = root["myStringValue"];
    data.myCommonData = loadFromTree<MyCommonDataStruct>(root["myCommonData"]);
    return data;
}
```

### Handshake

The handshake is an important mechanism of establishing a connection between two entities ([Node](include/comms/node/Node.h) and [ServerBus](include/comms/network/server/ServerBus.h) in this case), by trading a series of messages up front to ensure that proper communication can be established. On the server side this is handled via the [cadf::comms::HandshakeHandler](include/comms/network/handshake/HandshakeHandler.h), which uses a [cadf::comms::IHandshakeFactory](include/comms/network/handshake/Handshake.h) to create a [cadf::comms::IHandshake](include/comms/network/handshake/Handshake.h) which will perform the handshake itself. A provided [cadf::comms::IHandshakeCompleteListener](include/comms/network/handshake/Handshake.h) is notified when the handshake is successfully completed. For the purpose of customizability the handshake classes are all pure virtual (interfaces), with one default implementation provided in the form of the [cadf::comms::ProtocolHandshakeFactory](include/comms/network/handshake/ProtocolHandshake.h) and [cadf::comms::ProtocolHandshake](include/comms/network/handshake/ProtocolHandshake.h). These will perform a handshake in the desired protocol, where:

* Upon client connecting to the server, the server will send a [cadf::comms::HandshkeInitMessage](include/comms/network/handshake/HandshakeInitMessage.h) indicating the max version of the handshake process the server will support
* The client is expect to reply with a [cadf::comms::HandshakeResponseMessage](include/comms/network/handshake/HandshakeResponseMessage.h) of an appropriate version (currently max is V1), indicating the addressing details of the client (_Type_ and _Instance_)
* With the addressing information received, the server will register the client with the server and reply with a [cadf::comms::HandshakeCompleteMessage](include/comms/network/handshake/HandshakeCompleteMessage.h) indicating the version of the handshake process that was completed, and by sending the message to indicate that the handshake process was completed successfully
* At this point normal communication can commence between the two side.

### Server

The [Bus](include/comms/bus/Bus.h) becomes a server into which the various [Nodes](include/comms/node/Node.h) connect as clients. [cadf::comms::ServerBus](include/comms/network/server/ServerBus.h) provides an implementation of middle layer that binds a server to the [Bus](include/comms/bus/Bus.h). Creating a [ServerBus](include/comms/network/server/ServerBus.h) is a multistep process as shown in the following example:

```C++
// Define the details of the network connection for the server
cadf::comms::NetworkInfo myNetworkInfo;
myNetworkInfo.ipVersion = cadf::comms::NetworkInfo::IPv4;
myNetworkInfo.netAddress = "127.0.0.1"; // The IP Address on which to accept client connections
myNetworkInfo.port = 1234; // The port on which to accept client connections

// Create the mechanism for handling client connections
cadf::comms::ProtocolHandshakeFactory<PROTOCOL> myHandshakeFactory(myMaxMsgSize, myMsgFactory);
cadf::comms::HandshakeHandler myHandshakeHandler(myHandshakeFactory);
cadf::comms::BasicServerConnectionFactory<PROTOCOL> myConnectionFactory(myMsgFactory);
cadf::comms::ServerConnectionHandler myConnectionHandler(&myHandshakeHandler, &myConnectionFactory);

// Create the bus itself
cadf::comms::TcpServerSocket myServerSocket(myNetworkInfo, &myConnectionHandler, myQueueSize);
cadf::comms::LocalBasicBus myRoutingBus;
cadf::comms::ServerBus remoteBus(&myServerSocket, &myRoutingBus);
```

Where:

* [cadf::comms::ProtocolHandshakeFactory](include/comms/network/handshake/ProtocolHandshake.h) creates the logic for performing a handshake with a newly connected client
* [cadf::comms::HandshakeHandler](include/comms/network/handshake/HandshakeHandler.h) triggers a handshake when a client connects
* [cadf::comms::BasicServerConnectionFactory](include/comms/network/server/BasicServerConnectionFactory.h) creates an internal representation of the connected [Node](include/comms/node/Node.h) that is then registered with the [Bus](include/comms/bus/Bus.h) (equivalent to the [LocalConnection](include/comms/connection/LocalConnection.h) as shown in the Local Bus).
* [cadf::comms::ServerConnectionHandler](include/comms/network/socket/ServerConnectionHandler.h) sits between the actual server socket and the [Bus](include/comms/bus/Bus.h), handling the lifecycle of client connection (triggering the handshake on connection, creating the internal connection representation on successful handshake, and cleanup on client disconnect)
* [cadf::comms::TcpServerSocket](include/comms/network/socket/TcpServerSocket.h) the actual network socket into which the clients will connect

To start the server and allow for clients to connect, simply connect the socket

```C++
myServerSocket.connect();
```

### Client

Once the ServerBus is up and running, a [Node](include/comms/node/Node.h) can connect to it.

```C++
// Define the details of where to connect to the server
cadf::comms::NetworkInfo myNetworkInfo;
myNetworkInfo.ipVersion = cadf::comms::NetworkInfo::IPv4;
myNetworkInfo.netAddress = "127.0.0.1"; // The IP Address on which to accept client connections
myNetworkInfo.port = 1234; // The port on which to accept client connections

cadf::comms::TcpClientSocket mySocket(myNetworkInfo, myMaxMsgSize);
cadf::comms::BasicClient myClient(&mySocket);
cadf::comms::ClientConnection<PROTOCOL> myConnection(myType, myInstance, myMsgFactory, &myClient);
cadf::comms::Node myNode(&myConnection);
```

Where:

* [cadf::comms::TcpClientSocket](include/comms/network/socket/TcpClientSocket.h) the actual network socket through which the connection to the server is established
* [cadf::comms::BasicClient](include/comms/network/client/BasicClient.h) wraps around and manages the client socket
* [cadf::comms::ClientConnection](include/comms/connection/ClientConnection.h) network equivalent to the [LocalConnection](include/comms/connection/LocalConnection.h), which handles the connection (including handshaking) with the server

Beyond this, the operation and usage of the [Node](include/comms/node/Node.h) is unchanged from the Local description above.

## Message Passing

Regardless of which connection is employed (local or remote), or which protocol in the case of remote, the process of message passing and processing is the same. The only difference is when registering the messages with the [Node](include/comms/node/Node.h), however that is due to the differences in [Node](include/comms/node/Node.h) initialization and creation in each case, rather than any difference from the message handling.

### Register Messages

The [Node](include/comms/node/Node.h) is only able to understand and process (send/receive) messages that it is aware of and registered with it. The process of registering messages with the node is performed via the [cadf::comms::MessageRegistry](include/comms/message/MessageRegistry.h) and [cadf::comms::MessageFactory](include/comms/message/MessageFactory.h).

* [MessageRegistry](include/comms/message/MessageRegistry.h) - allows for the specification of which messages to register
* [MessageFactory](include/comms/message/MessageFactory.h) - used by the [Node](include/comms/node/Node.h) to create instances of the messages

So the [MessageRegistry](include/comms/message/MessageRegistry.h) defines which messages the [Node](include/comms/node/Node.h) is going to understand, and the [MessageFactory](include/comms/message/MessageFactory.h) will create the messages for it. As part of initialization the [MessageRegistry](include/comms/message/MessageRegistry.h) must register the messages with the [MessageFactory](include/comms/message/MessageFactory.h) for it to be aware of the desired messages.

```C++
/*
 * For Remote
 */
cadf::comms::MessageRegistry<cadf::comms::dom::json::JSONProtocol, MyMessage, MyOtherMessage, MyLastMessage> myRemoteMsgRegistry;
cadf::comms::MessageFactory<cadf::comms::dom::json::JSONProtocol> myRemoteMsgFactory;
myRemoteMsgRegistry.registerMessages(&myRemoteMsgFactory);

// For Server
cadf::comms::ProtocolHandshakeFactory<cadf::comms::dom::json::JSONProtocol> myHandshakeFactory(myMaxMsgSize, &myRemoteMsgFactory);
<snip>

// For Client
<snip>
cadf::comms::ClientConnection<cadf::comms::dom::json::JSONProtocol> myConnection(myType, myInstance, &myRemoteMsgFactory, &myClient);
<snip>


/*
 * For Local
 */
cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, MyMessage, MyOtherMessage, MyLastMessage> myLocalMsgRegistry;
cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol> myLocalMsgFactory;
myRemoteMsgRegistry.registerMessages(&myLocalMsgFactory);

// For Client
cadf::comms::LocalConnection myConnection(myType, myInstance, &myLocalMsgFactory);
<snip>
```

### Send Messages

Sending a message is in quite straight forward. Once the [Node](include/comms/node/Node.h) is initialized and connected, simply call `sendMessage()`.

```C++
myNode.sendMessage(msgToSend, recipientType, recipientInstance);
```

`sendMessage()` returns a boolean to indicate the success/failure of sending, it may also throw an exception to indicate why a message failed to send (i.e.: attempting to send a message that was not registered with the [Node](include/comms/node/Node.h)).

### Receive Messages

In order for a [Node](include/comms/node/Node.h) to receive a message, not only must be registered as per what is described above (that is required so that the [Node](include/comms/node/Node.h) can determine the contents of the message), but a [cadf::comms::IProcessor](include/comms/node/Processor.h) must be registered. [IProcessor](include/comms/node/Processor.h) is a pure virtual interface to side-step issues with template classes, and an implementation in the form of [cadf::comms::MessageProcessor](include/comms/node/Processor.h) is provided. Create an [IProcessor](include/comms/node/Processor.h) for the desired message type by extending [MessageProcessor](include/comms/node/Processor.h) and implementing the `processMessage()` method. `processMessage()` will be called when a message is received, and it is within that the message can be processed. Once the specific [MessageProcessor](include/comms/node/Processor.h) is created, it must be registered with the [Node](include/comms/node/Node.h).

```C++
struct MyMessageProcessor: cadf::comms::MessageProcessor<MyMessage> {

     virtual void processMessage(cost MyMessage *msg) {
          std::cout << "Message Received " << msg->getType() << std::endl;
     }
}

<snip>
MyMessageProcessor myMsgProcessor;
myNode.addProcessor(&myMessageProcessor);
</snip>
```

Once added, the [Node](include/comms/node/Node.h) will pass any messages of a given type to the [MessageProcessor](include/comms/node/Processor.h) created to process them. If no [MessageProcessor](include/comms/node/Processor.h) is added for a message of a given type, then any messages of that type are simply discarded.

To stop processing messages of a given type, simply remove the [MessageProcessor](include/comms/node/Processor.h) for messages of that type

```C++
myNode.removeProcessor(&myMessageProcessor);
```
