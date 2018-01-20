# Software for ESP8266 (Wemos D1 Mini)

## Build

The software should be built with [platformio](http://platformio.org/get-started/cli).

### Prerequisites:

* platformio is installed.
* If necessary, adapt `platformio.ini` to your needs (reset-method, flash-size)`. (Current version is preconfigured for Wemos D1 Mini)
* rename `example_config.h` to `config.h` and edit it to enter your WiFi SSID and password.

### Build on command line:
  * `pio run -t upload`
  * If you want to build, but no upload, enter `pio run`
  
### Build in IDE:
  * You can use Atom or VSCode with installed platformio extension.


## Dependencies

**All dependencies are installed with platformio,** so there is no need to install them manually.

### Automaton

[Automaton](https://github.com/tinkerspy/Automaton/wiki) is a *Reactive state machine framework for Arduino*.
The RFID Cardreader uses automaton to implement the state machine and the to detect the events that trigger state transitions.

*Note:* As the original version support only I/O signals on the internal GPIO pins, currently a [fork](https://github.com/euphi/Automaton) of Automaton is in use.

### Automaton-ESP8266
[Automaton-ESP8266](https://github.com/tinkerspy/Automaton-Esp8266/wiki) is an extension to Automaton to use the event/connector mechanism of Automaton for WIFI Connection and HTTP Client.

*Note:* As the original version has major bugs, currently a [fork](https://github.com/euphi/Automaton-Esp8266) of Automaton-ESP8266 is in use.

### MFRC522

Library to support MFRC522 RFID Reader on SPI.

###	Adafruit MCP23017 Arduino Library

Library to support MCP23017 I/O expander on I2C.

## Software Architecture

### main.cpp

main.cpp defines instances of the MFRC522 card reader and its own state-machine ATM_RFID_Relay as global variables.

#### `setup()`
The `setup()` functions initializes SPI and I2C, "begins" the state machine and starts the WiFi connection and connects the related events (connected, connection lost) to the triggers of the state machine:
```C++
	wifi.begin( ap_ssid, ap_password )
        // NOTE: LED_BUILTIN is RX (UART) on ESP-12, so this interferes with Serial communication on
        // ESP-12 modules (including most Wemos D1, because they also use ESP-12)
        .led( LED_BUILTIN, true ) // Esp8266 built in led shows wifi status
        .onChange(0, relay, ATM_RFIDRelay::EVT_EV_CONNLOST) // directly send an Event from Wifi the the relay state machine
        .onChange(1, relay, ATM_RFIDRelay::EVT_EV_CONNECTED)// "sub-slot" 0: Connection lost; 1: Connected
        .start();
```
  
#### `loop()`

The `loop()` functions cycles the automaton framework, then checks the card reader for new cards and if a new card is present, triggers the state machine with a new request including the UID of the card as parameter.

### ATM_RFIDRelay

This is the state machine class for automaton. So it's derived from Automaton's `Machine` class and adds the `enum`s for states and events.

#### ATM_RFIDRelay.h

The skeleton was created with the [online state machine editor](http://www.wolkendek.nl/atm/index.php) for Automaton.

As private members it defines timers, an instance of the MCP23017 IO expanders, a HTTPClient instance, an input button, and the outputs (LED, Switch, buzzer).

#### ATM_RFIDRelay.cpp

`ATM_RFIDRelay& ATM_RFIDRelay::begin()`
Here the state table is initialized and the sub-statemachines (timers, outputs, inputs, HTTPClient) are configured.

`int ATM_RFIDRelay::event( int id )`

This method is called from the automaton framework every cycle for each possible trigger event. If it returns true the event is triggers.
In ATM_RFIDRelay this is used for the timers only as all other events are triggered externally.

`void ATM_RFIDRelay::action(int id)`

For each state, Automaton allows to define a function for entry, loop or exit.
ATM_RFIDRelay uses this to set the outputs according to the current state.
The automaton "ATM_Led" class allows to set an output to a blink pattern, this is used for some optical or accustical feedback.




