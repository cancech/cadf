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

### Specialty Nodes

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
