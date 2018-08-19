#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#include "Adafruit_MCP23017.h"

#define GPB_OFFSET 8                  //offset for GPB* pins
#define CONN_LED_PIN 6                //GPA6
#define TXRX_LED_PIN 5                //GPA5
#define BUZZ_PIN 7                    //GPA7
#define RELAY_PIN GPB_OFFSET + 0      //GPB0
#define RELAY_PIN_READ GPB_OFFSET + 1 //GPB1

#define CYCLE_TIME         100
#define BUZZ_SHORT_TIME_MS 100
#define BUZZ_LONG_TIME_MS  1000 * 60 //every minute
#define LED_BLINK_TIME_MS  200

typedef enum tagLEDMode{
  LEDOff = 0,
  LEDOn,
  LEDBlink
}LEDMode;

typedef enum tagBuzzerMode{
  BuzzOff = 0,
  BuzzOn,
  BuzzConfirm,
  BuzzWaitForUser,
  BuzzAck,
  BuzzNak,
  BuzzAnnoy
}BuzzerMode;

typedef enum tagRelayMode{
  RelOff = 0,
  RelOn
}RelayMode;

class GPIOHandler{
public:
  GPIOHandler(void);
  virtual ~GPIOHandler(void);

  void LEDConn(LEDMode mode);
  void LEDTxRx(LEDMode mode);
  void Buzzer(BuzzerMode mode);
  void Relay(RelayMode mode);
  bool RelayBypassed();
  RelayMode RelayState();
  void GPIOTest();
  void Update();

private:
  void UpdateLed(uint8_t pin, LEDMode mode, uint16_t* time);
  void UpdateBuzzer();
  void BuzzerBeep(uint16_t tOn, uint16_t tOff, uint8_t tCount); //tOn = on time in ms, tOff = off time in ms, tCount = no of repeats
  LEDMode m_ConnState;
  LEDMode m_TxRxState;
  BuzzerMode m_BuzzerState;
  RelayMode m_RelayState;
  uint16_t m_ConnLedTime;
  uint16_t m_TxRxLedTime;
  uint8_t m_InitialRelOn;
  int m_BuzzTime;

  Adafruit_MCP23017 gpio;
};
#endif //GPIOHANDLER_H
