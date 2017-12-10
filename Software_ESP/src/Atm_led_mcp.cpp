/*
 * Atm_led_mcp.cpp
 *
 *  Created on: 09.12.2017
 *      Author: ian
 */

#include <Atm_led_mcp.h>

Atm_led_mcp::Atm_led_mcp(Adafruit_MCP23017& _gpio):
	Atm_led(),			// base class ctor would also be called implicitly, but better style to mention this explicitly :)
	gpio(_gpio) {
	// nothing to see here
}

void Atm_led_mcp::initLED() {
	gpio.pinMode(pin, OUTPUT);
	gpio.digitalWrite(pin, activeLow ? HIGH : LOW);
}

void Atm_led_mcp::switchOn() {
	gpio.digitalWrite(pin, !activeLow);
}

void Atm_led_mcp::switchOff() {
	gpio.digitalWrite(pin, activeLow);
}

void Atm_led_mcp::setBrightness(int value) {
	if (value == toHigh) switchOn(); else if(value==toLow) switchOff(); else
	Serial.printf("ERROR: Setting brightness on GPIO expander is not possible (pin: %d)\n", pin);
}
