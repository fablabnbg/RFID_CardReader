# Principle

All communication between the RFID-Cardreader (reader) and the controller is done via MQTT.

Communication follows the [Homie Convention](https://github.com/marvinroger/homie).

If the reader needs to authorize a user, it MUST send an authorisation request to the controller. 

The controller MUST answer the request with an appropriate command to the controller. 


# Homie convention

* The base topic is up the installation. It is recommended to use own base topics for the network of RFID-Cardreaders, e.g. `FLN-Machines`.
* Each device SHOULD be named according to the machine it controlls, e.g. `ZingLaser`.
* *Note: Following the homie convention includes all the default communication from Homie and its Homie-ESP8266 implementation, e.g. advertising of Nodes, frequent information about WiFi quality, uptime, OTA support etc.*


## Nodes

* There MUST be an `AuthReq` node with the property `Token`.
* There MUST be an `State` node with the property `Alert` and the *settable* property `Relais` 
* Other nodes MAY be defined, e.g. to use the device for other functions (e.g. temperature, or current measurement etc.).

## Properties 
### Token (Node Authreq)

* Datatype uint32, no unit

### Relais (Node State)
* Datatype string, no unit
* The string to set the property (controller to reader) MUST be one of the following *enumeration*:
  * `ON_Authorized`: The user has been authorized, so the relais must be switched on.
  * `ON_Override`: The relais must be switched on, although no authorization was requested (override function from server).
  * `OFF_Rejected`: The user has been rejected
  * `OFF_Override`:  The server commands the relais to be switched off.
  * `OFF_locked`: The server commands the relais to be switched off and there SHOULD be no request from the reader to switch it on again (this also SHOULD be visualised on the reader).
  * `OFF_unlocked`: Used to reset the locked state.
* The string to tell the property to the conroller MUST be on of the following *enumeration*:
  * `ON`
  * `OFF`: The relais is off. This includes local reasons to switch the relais off.
  * `ON_override`: The relais has been overridden locally.  
  * *optional* @@@TODO@@@: `OFF_timeout`: The relais has been switched off due to timeout

### Alert
* Datatype: string, no unit
* The string informs the controller of an alert:
  * `OK`: No alert
  * `Intrusion`: Intrusion contacts open
  * @@@TODO@@@: `Override` (this informationis redundant to the Relais state ON_override)
  * Other strings MAY be defined, e.g. `overcurrent`, `powerfail` etc.

## Example:
  
 The base topic is `FLN-Machines` and the device is `Coffee`.
  
 The card reader reads a card of a user that is allowed to use the machine. An authorisation request with the token `CAFEBABE` is to be sent.
 
 Direction           | MQTT Topic                          | MQTT Message
 --------------------|-------------------------------------|-----------------
 reader -> controller|FLN-Machines/Coffee/AuthRequest/Token| CAFEBABE
 controller -> reader|FLN-Machines/Coffee/State/Relais/set | ON_Authorized
 reader -> controller|FLN-Machines/Coffee/State/Relais     | ON
 
 The card reader reads a card of a user that is NOT allowed to use the machine. An authorisation request with the token `ABADCAFE` is to be sent. 

 Direction           | MQTT Topic                          | MQTT Message
 --------------------|-------------------------------------|-----------------
 reader -> controller|FLN-Machines/Coffee/AuthRequest/Token| ABADCAFE
 controller -> reader|FLN-Machines/Coffee/State/Relais/set | OFF_Rejected
 reader -> controller|FLN-Machines/Coffee/State/Relais     | OFF


  # Security
  
  ...tbd...
  
  Keywords:
    * RFID-Security out of scope of communication spec
    * WiFi with strong passwords, on Wifi for machines
    * MQTT TLS (as soon as available in Homie-ESP8266
    * MQTT user authentication
    * MQTT ACLs for topics
 
  
