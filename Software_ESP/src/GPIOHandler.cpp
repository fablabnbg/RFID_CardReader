
#include <GPIOHandler.h>
#include <Adafruit_MCP23017.h>
#include <Arduino.h>
#include <example_config.h>

GPIOHandler::GPIOHandler()
{
  gpio.begin();
  gpio.pinMode(CONN_LED_PIN, OUTPUT);
  gpio.pinMode(TXRX_LED_PIN, OUTPUT);
  gpio.pinMode(BUZZ_PIN, OUTPUT);
  gpio.pinMode(RELAY_PIN, OUTPUT);
  gpio.pinMode(RELAY_PIN_READ, INPUT);
  gpio.pullUp(RELAY_PIN_READ, HIGH);

  gpio.digitalWrite(CONN_LED_PIN, LOW);
  gpio.digitalWrite(TXRX_LED_PIN, LOW);
  gpio.digitalWrite(BUZZ_PIN, LOW);
  gpio.digitalWrite(RELAY_PIN, LOW);

  m_ConnLedTime = 0;
  m_TxRxLedTime = 0;
  m_BuzzTime = 0;
  m_ConnState = LEDMode::LEDOff;
  m_TxRxState = LEDMode::LEDOff;
  m_BuzzerState = BuzzerMode::BuzzOff;
  m_RelayState = RelayMode::RelOff;
  m_InitialRelOn = 1;

}

GPIOHandler::~GPIOHandler()
{

}
void GPIOHandler::GPIOTest(){
  gpio.digitalWrite(CONN_LED_PIN, LOW);
  delay(500);
  gpio.digitalWrite(CONN_LED_PIN, HIGH);
  gpio.digitalWrite(TXRX_LED_PIN, LOW);
  delay(500);
  gpio.digitalWrite(TXRX_LED_PIN, HIGH);
  BuzzerBeep(100, 1, 1);
}
void GPIOHandler::LEDConn(LEDMode mode)
{
  m_ConnState = mode;
  if(mode == LEDOn){
    gpio.digitalWrite(CONN_LED_PIN, LOW);
  }
  if(mode == LEDOff){
    gpio.digitalWrite(CONN_LED_PIN, HIGH);
  }
}
void GPIOHandler::LEDTxRx(LEDMode mode)
{
  m_TxRxState = mode;
  if(mode == LEDOn){
    gpio.digitalWrite(TXRX_LED_PIN, LOW);
  }
  if(mode == LEDOff){
    gpio.digitalWrite(TXRX_LED_PIN, HIGH);
  }
}
void GPIOHandler::Buzzer(BuzzerMode mode)
{
  if(m_BuzzerState != mode) //new mode, restart timer
  {
    m_BuzzTime = millis();
  }
  m_BuzzerState = mode;
}
void GPIOHandler::Relay(RelayMode mode)
{
  m_RelayState = mode;
}

bool GPIOHandler::RelayBypassed(){

  //Serial.printf(" Relay pin read %d ", gpio.digitalRead(RELAY_PIN_READ));
  if( ( m_RelayState == RelOff ) && ( gpio.digitalRead(RELAY_PIN_READ) == HIGH ) ){
    return true;
  }
  return false;
}
RelayMode GPIOHandler::RelayState(){
  return m_RelayState;
}

void GPIOHandler::Update()
{
  //Serial.println("Ctrl Update start");
  UpdateLed(CONN_LED_PIN, m_ConnState, &m_ConnLedTime);
  UpdateLed(TXRX_LED_PIN, m_TxRxState, &m_TxRxLedTime);
  //Serial.println("Before Update Buzzer");
  UpdateBuzzer();

  switch(m_RelayState){
    case RelOff:
      gpio.digitalWrite(RELAY_PIN, LOW);
      m_InitialRelOn = 1;
      break;
    case RelOn:
      gpio.digitalWrite(RELAY_PIN, HIGH);
      if(m_InitialRelOn){
        delay(Timeout_Block);
        m_InitialRelOn = 0;
      }
      break;
    break;
  }
}

void GPIOHandler::UpdateLed(uint8_t pin, LEDMode mode, uint16_t* time){
  switch(mode){
    case LEDOff:
      gpio.digitalWrite(pin, HIGH);
      //Serial.printf("Turning pin %d to low\n",pin);
      *time = 0;
      break;
    case LEDOn:
      gpio.digitalWrite(pin, LOW);
      *time = 0;
      break;
    case LEDBlink:
      m_ConnLedTime+=CYCLE_TIME;
      if(*time >= LED_BLINK_TIME_MS){
        gpio.digitalWrite(pin, !gpio.digitalRead(pin));
        *time = 0;
      }
    break;
  }
}
void GPIOHandler::UpdateBuzzer(){
  int iStartTime = millis();
  //Serial.printf("Buzzer state: %d\n",m_BuzzerState);
  switch(m_BuzzerState){
    case BuzzOff:
      gpio.digitalWrite(BUZZ_PIN, LOW);
      break;
    case BuzzOn:
      gpio.digitalWrite(BUZZ_PIN, HIGH);
      break;
    case BuzzConfirm:
      gpio.digitalWrite(BUZZ_PIN, HIGH);
      delay(1000);
      gpio.digitalWrite(BUZZ_PIN, LOW);
      m_BuzzerState = BuzzOff;
    break;
    case BuzzWaitForUser:

      if(gpio.digitalRead(BUZZ_PIN) == HIGH){

        if(millis() - m_BuzzTime > 3000){ //3s beep
          gpio.digitalWrite(BUZZ_PIN, LOW);
          m_BuzzTime = millis();
        }
      }else{
        if(millis() - m_BuzzTime > 2000){ //2s non-beep
          gpio.digitalWrite(BUZZ_PIN, HIGH);
          m_BuzzTime = millis();
        }
      }
      break;


    case BuzzAck:
      gpio.digitalWrite(BUZZ_PIN, HIGH); //blocking
      delay(2000);
      gpio.digitalWrite(BUZZ_PIN, LOW);
      m_BuzzerState = BuzzOff;
    break;
    case BuzzNak:
      BuzzerBeep(2000, 200, 2);
      gpio.digitalWrite(BUZZ_PIN, HIGH); //blocking, its ok, something went wrong
      delay(10000);
      gpio.digitalWrite(BUZZ_PIN, LOW);
      m_BuzzerState = BuzzOff;
    break;
    case BuzzAnnoy:
      if(millis() - m_BuzzTime > 60000){
        gpio.digitalWrite(BUZZ_PIN, HIGH);
        delay(5000);
        gpio.digitalWrite(BUZZ_PIN, LOW);
        m_BuzzTime = millis();
      }
      break;
  }
}
void GPIOHandler::BuzzerBeep(uint16_t tOn, uint16_t tOff, uint8_t tCount){
  for(uint8_t i = 0; i < tCount; i++){
    gpio.digitalWrite(BUZZ_PIN, HIGH);
    delay(tOn);
    gpio.digitalWrite(BUZZ_PIN, LOW);
    if(tCount > 1){
      delay(tOff);
    }
  }
  m_BuzzTime = 0;
  m_BuzzerState = BuzzOff;
}
