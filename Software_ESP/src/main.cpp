#include <Automaton.h>

#include "ATM_RFIDRelay.h"

// CONFIG: PIN layout
#define RST_PIN         D0  //16      // Configurable, see typical pin layout above
#define SS_PIN          D8  //15      // Configurable, see typical pin layout above
// Defaults for SPI
// MOSI  = 13	D7
// MISO  = 12	D6
// SCK   = 14	D5
#include <SPI.h>
#include <MFRC522.h>


#include <Wire.h>  // Wire is needed for I2C (TWI)
#include "Adafruit_MCP23017.h"

// To protect real passwords, config.h is not included on the github repository.
//   So if this file is not found, copy
//   example_config.h to config.h and adapt values
#include "config.h"

MFRC522 cardreader(SS_PIN, RST_PIN);  // Create MFRC522 instance


ATM_RFIDRelay relay;				  // Instance of the "main" state machine

void setup() {
	/* Initialize Serial
	 *   74880 is the baudrate of the bootloader. I set the application to the same value,
	 *   so you can see all messages at startup. As 74880 is not a common baud rate,
	 *   some terminal application may not support it. In this case, set baudrate to
	 *   a more common value, e.g. 115200.
	*/
	Serial.begin(74880);
	Serial.println("Setup");
	// flush(), so it is made sure that the text is seen
	//  This helps debugging potential exceptions that may occur shortly after.
	//  If flush() is used and if message is not on Serial, the crash occured before (== one of the constructors of global objects)
	Serial.flush();

	SPI.begin();				// Init SPI with default values
	cardreader.PCD_Init();		// Init MFRC522
	cardreader.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details

	// Enable debug info about the relay state machine on Serial
	relay.begin().trace(Serial);

	wifi.begin( ap_ssid, ap_password )
		// NOTE: LED_BUILTIN is RX (UART) on ESP-12, so this interferes with Serial communication on
		// ESP-12 modules (including most Wemos D1, because they also use ESP-12)
	    .led( LED_BUILTIN, true ) // Esp8266 built in led shows wifi status
		.onChange(0, relay, ATM_RFIDRelay::EVT_EV_CONNLOST) // directly send an Event from Wifi the the relay state machine
		.onChange(1, relay, ATM_RFIDRelay::EVT_EV_CONNECTED)// "sub-slot" 0: Connection lost; 1: Connected
	    .start();

	// Uncomment to enable debug info about the WiFi state machine on Serial
	wifi.trace(Serial);
}


void loop() {
    automaton.run();  // Run cycle of Automaton state machine

	// Look for new cards, TODO: Don't retrigger new card
	if ( ! cardreader.PICC_IsNewCardPresent()) {
		return;
	}
	Serial.println("New card present!");

	if (relay.state() == ATM_RFIDRelay::CONNECTED_CIP)
	{
		Serial.println("Request pending - ignoring card");
		return;
	}

	// Select one of the cards
	if ( ! cardreader.PICC_ReadCardSerial()) {
		return;
	}

	uint32_t uid = static_cast<uint32_t>(cardreader.uid.uidByte[0] << 24
			| cardreader.uid.uidByte[1] << 16 | cardreader.uid.uidByte[2] << 8
			| cardreader.uid.uidByte[3]);
	Serial.println(uid, HEX);
	relay.triggerRequest(uid);

	// Set card to halt so it is not detected again
	MFRC522::StatusCode rc = cardreader.PICC_HaltA();
	if (rc != MFRC522::STATUS_OK) {
		Serial.printf("Warning: Cannot set card to halt state [Error %x]", rc);
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	//	cardreader.PICC_DumpToSerial(&(cardreader.uid));
}
