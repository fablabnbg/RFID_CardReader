#pragma once

#include <Automaton.h>
#include <Atm_esp8266.h>
#include <Adafruit_MCP23017.h>

#include "Atm_led_mcp.h"

class ATM_RFIDRelay: public Machine {

 public:
	enum { NO_CONNECTION_OFF, NO_CONNECTION_BRIDGED, CONNECTED_OFF, CONNECTED_ON, CONNECTED_CIP, CONNECTED_BRIDGED, CONNECTED_REJECTED, NO_CONNECTION_ON }; // STATES
    enum { EVT_EV_OFF, EVT_TIMER_REJ_NOTIFY, EVT_TIMER_MAXON, EVT_EV_AUTHREJ, EVT_EV_AUTHCONF, EVT_EV_AUTHREQ, EVT_EV_CONNLOST, EVT_EV_CONNECTED, ELSE }; // EVENTS
  ATM_RFIDRelay() :
	  Machine(),
	  gpio(),
	  led_rxtx(gpio),
  	  led_conn(gpio),
	  bit_switch(gpio),
	  buzzer(gpio)
  {};
  ATM_RFIDRelay& begin( void );
  ATM_RFIDRelay& trace( Stream & stream );
  ATM_RFIDRelay& trigger( int event );
  int state( void );
  ATM_RFIDRelay& ev_off( void );
  ATM_RFIDRelay& ev_authrej( void );
  ATM_RFIDRelay& ev_authconf( void );
  ATM_RFIDRelay& ev_authreq( void );
  ATM_RFIDRelay& ev_connlost( void );
  ATM_RFIDRelay& ev_connected( void );

  void triggerRequest(uint16_t id);

 private:
  enum { ENT_NO_CONNECTION_OFF, ENT_NO_CONNECTION_BRIDGED, ENT_CONNECTED_OFF, ENT_CONNECTED_ON, ENT_CONNECTED_CIP, EXT_CONNECTED_CIP, ENT_CONNECTED_BRIDGED, ENT_CONNECTED_REJECTED, ENT_NO_CONNECTION_ON };
  int event( int id );
  void action( int id );

  atm_timer_millis timer_max_on, timer_rej_notify;
  Adafruit_MCP23017 gpio;
  Atm_esp8266_httpc_simple httpClient;

  Atm_led_mcp led_rxtx;
  Atm_led_mcp led_conn;
  Atm_led_mcp bit_switch;
  Atm_led_mcp buzzer;

  Atm_button switch_off;

  void triggerResponse();
};




/*
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="ATM_RFIDRelay">
    <states>
      <NO_CONNECTION_OFF index="0" on_enter="ENT_NO_CONNECTION_OFF">
        <EVT_EV_CONNECTED>CONNECTED_OFF</EVT_EV_CONNECTED>
      </NO_CONNECTION_OFF>
      <NO_CONNECTION_BRIDGED index="1" on_enter="ENT_NO_CONNECTION_BRIDGED">
        <EVT_EV_CONNECTED>CONNECTED_BRIDGED</EVT_EV_CONNECTED>
      </NO_CONNECTION_BRIDGED>
      <CONNECTED_OFF index="2" on_enter="ENT_CONNECTED_OFF">
        <EVT_EV_AUTHREQ>CONNECTED_CIP</EVT_EV_AUTHREQ>
        <EVT_EV_CONNLOST>NO_CONNECTION_OFF</EVT_EV_CONNLOST>
      </CONNECTED_OFF>
      <CONNECTED_ON index="3" on_enter="ENT_CONNECTED_ON">
        <EVT_TIMER_MAXON>CONNECTED_OFF</EVT_TIMER_MAXON>
        <EVT_EV_AUTHREQ>CONNECTED_CIP</EVT_EV_AUTHREQ>
        <EVT_EV_CONNLOST>CONNECTED_ON</EVT_EV_CONNLOST>
      </CONNECTED_ON>
      <CONNECTED_CIP index="4" on_enter="ENT_CONNECTED_CIP">
        <EVT_EV_AUTHREJ>CONNECTED_REJECTED</EVT_EV_AUTHREJ>
        <EVT_EV_AUTHCONF>CONNECTED_ON</EVT_EV_AUTHCONF>
        <EVT_EV_CONNLOST>NO_CONNECTION_OFF</EVT_EV_CONNLOST>
      </CONNECTED_CIP>
      <CONNECTED_BRIDGED index="5" on_enter="ENT_CONNECTED_BRIDGED" on_loop="LP_CONNECTED_BRIDGED">
        <EVT_EV_CONNLOST>NO_CONNECTION_BRIDGED</EVT_EV_CONNLOST>
      </CONNECTED_BRIDGED>
      <CONNECTED_REJECTED index="6" on_enter="ENT_CONNECTED_REJECTED">
        <EVT_TIMER_REJ_NOTIFY>CONNECTED_OFF</EVT_TIMER_REJ_NOTIFY>
        <EVT_EV_CONNLOST>NO_CONNECTION_OFF</EVT_EV_CONNLOST>
      </CONNECTED_REJECTED>
      <NO_CONNECTION_ON index="7" on_enter="ENT_NO_CONNECTION_ON" on_loop="LP_NO_CONNECTION_ON">
        <EVT_TIMER_MAXON>NO_CONNECTION_OFF</EVT_TIMER_MAXON>
      </NO_CONNECTION_ON>
    </states>
    <events>
      <EVT_TIMER_REJ_NOTIFY index="0" access="MIXED"/>
      <EVT_TIMER_MAXON index="1" access="MIXED"/>
      <EVT_EV_AUTHREJ index="2" access="MIXED"/>
      <EVT_EV_AUTHCONF index="3" access="MIXED"/>
      <EVT_EV_AUTHREQ index="4" access="MIXED"/>
      <EVT_EV_CONNLOST index="5" access="MIXED"/>
      <EVT_EV_CONNECTED index="6" access="MIXED"/>
    </events>
    <connectors>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/
