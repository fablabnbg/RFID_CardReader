
// CONFIG: PIN layout
#define RST_PIN         D0  //16      // Configurable, see typical pin layout above
#define SS_PIN          D8  //15      // Configurable, see typical pin layout above
// Defaults for SPI
// MOSI  = 13	D7
// MISO  = 12	D6
// SCK   = 14	D5
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266Wifi.h>

#include <Wire.h>  // Wire is needed for I2C (TWI)
#include "Adafruit_MCP23017.h"


// To protect real passwords, config.h is not included on the github repository.
//   So if this file is not found, copy
//   example_config.h to config.h and adapt values
#include "example_config.h"
#include "GPIOHandler.h"
#include "CommunicationHandler.h"
#include "RFIDStateMachine.h"
#include "TimeoutHandler.h"

MFRC522 Cardreader(SS_PIN, RST_PIN);  // Create MFRC522 instance
MFRC522::Uid CurrentCard = {0};
GPIOHandler Ctrl;
RFIDState State;
RFIDTrigger Trigger;
WiFiClient Client;
TimeoutHandler Timeout;
CommunicationHandler Comm;

bool SameCard(){
	bool bOk = true;
	if(Cardreader.uid.size == CurrentCard.size
	&& Cardreader.uid.sak  == CurrentCard.sak){
		for(int i = 0; i < 10; i++){
			bOk = bOk && (Cardreader.uid.uidByte[i] == CurrentCard.uidByte[i]);
		}
		Serial.println("Cards are " + bOk ? "identical " : "not identical");
		return bOk;
	}else{
		return false;
	}
}
uint8_t GetState(){
	uint8_t commState;
	if(State == eStateRunning){
		commState = RFID_STATE_RUNNING;
	}else{
		commState = RFID_STATE_ERROR;
	}
	if(Ctrl.RelayState() == RelOn){
		commState |= RFID_STATE_RELAY_ON;
	}else{
		commState |= RFID_STATE_RELAY_OFF;
	}
	return commState;
}

void Verify(){
	Ctrl.LEDTxRx(LEDOn);
	Serial.println("Card verification");
	if(Comm.VerifyCard(Cardreader.uid.uidByte, Cardreader.uid.size, GetState())){
		Serial.println("Access granted");
		CurrentCard = Cardreader.uid;
		Ctrl.Relay(RelOn);
		Timeout.ResetTimeout();
		Ctrl.Buzzer(BuzzAck);
	}else{
		Serial.println("Access denied");
		Ctrl.Relay(RelOff);
		Ctrl.Buzzer(BuzzNak);
	}
	Ctrl.LEDTxRx(LEDOff);
}

void setup() {
	/* Initialize Serial
	 *   74880 is the baudrate of the bootloader. I set the application to the same value,
	 *   so you can see all messages at startup. As 74880 is not a common baud rate,
	 *   some terminal application may not support it. In this case, set baudrate to
	 *   a more common value, e.g. 115200.
	*/
	Serial.begin(74880);
	Serial.println("Setup");
	Ctrl.GPIOTest();
  WiFi.begin(ap_ssid, ap_password);
	State = eStateConnecting;
  Trigger = eTriggerOn;
	SPI.begin();				// Init SPI with default values
	Cardreader.PCD_Init();		// Init MFRC522
}

void loop(){
	//Serial.printf("Start\n");
	while(WiFi.status() != WL_CONNECTED){
		Serial.print(".");
		Ctrl.LEDConn(LEDOff);
		Ctrl.Buzzer(BuzzOff);
		Ctrl.Update();
		delay(100);
	}
  //Serial.printf("Wifi connected to %s\n", WiFi.localIP().toString().c_str());
	if(Ctrl.RelayBypassed()){
		Ctrl.Buzzer(BuzzAnnoy);
		Serial.printf("Relay Bypassed!");
		State = eStateInactive;
	}else{
		if(State == eStateInactive){
			Serial.println("Relay Bypassing resolved");
			Ctrl.Buzzer(BuzzOff);
			State = eStateRunning;
		}
	}
	//Serial.printf("Before Is Connected\n");
	//TO DO: Server connection and checking if data is available ?!
	//			 reading of enclosure switch (sabotage)
	if(!Comm.IsConnected()){
		Comm.Disconnect();
		if(!Comm.Connect(Server, Port)){
			State = eStateConnecting;
			Serial.println("Connecting to RFID Server ...");
		}else{
			Serial.println("Connection to RFID Server succeeded");
			Ctrl.LEDConn(LEDOn);
			//Client.println("Halo1bims!");
			State = eStateRunning;
		}
  }
//Serial.printf("Before Card Check\n");
	if( (State == eStateRunning) && Cardreader.PICC_IsNewCardPresent() && Cardreader.PICC_ReadCardSerial() ){
		Serial.println("Card detected");
		Cardreader.PICC_DumpToSerial(&(Cardreader.uid));
		if(Ctrl.RelayState() == RelOn){
			if(SameCard()){
				if(Timeout.TimeoutCritical()){
					Serial.println("Timeout reset");
					Timeout.ResetTimeout();
					Ctrl.Buzzer(BuzzAck);
				}else{
					Serial.println("Timeout decreased");
					Timeout.SetTimeoutCritical();
					Ctrl.Buzzer(BuzzWaitForUser);
				}
			}else{ //friendly fire
				 Verify();
			}
		}else{
			 Verify();
		}


	}
//Serial.printf("Before Timeout Check\n");
	if ( !Timeout.UpdateTimeout()){
		Serial.println("Timeout expired");
			Ctrl.Relay(RelOff);
			Ctrl.Buzzer(BuzzOff);
	}
	if ( Timeout.TimeoutCritical()){
		Serial.println("Timeout critical");
		Ctrl.Buzzer(BuzzWaitForUser);
	}
	//Serial.printf("Before Ctrl Update\n");
	Ctrl.Update();
	//Serial.printf("End\n");
}

/*
void loop() {
	if(WiFi.status() != WL_CONNECTED){
		State = eStateConnecting;
		Ctrl.LEDConn(LEDOff);
		Ctrl.Buzzer(BuzzOff);
		Ctrl.Update();
	}
	if(Ctrl.RelayBypassed()){
		Ctrl.Buzzer(BuzzLongBeeps);
		Serial.printf("Relay Bypassed!");
		State = eStateInactive;
	}

  switch(State){
		case eStateConnecting:
		  Serial.printf("State = Connecting");
			while (WiFi.status() != WL_CONNECTED)
		  {
		    delay(500);
		    Serial.print(".");
		  }
			if(Client.connect(Server, Port)){
				Serial.println("Connection to RFID Server succeeded");
				Client.println("Halo1bims!");
			}
			State = eStateRunning;
			Ctrl.LEDConn(LEDOn);
			break;
		case eStateRunning:
		  Serial.printf("State = Running");
			Ctrl.Buzzer(BuzzOff);
			if( Cardreader.PICC_ReadCardSerial() ){
				Cardreader.PICC_DumpToSerial(&(Cardreader.uid));
				Serial.printf("New Card present: Size = %d \n",Cardreader.uid.size);
				Serial.printf("UID = [");
				for(int i = 0; i < 10; i++){
					Serial.printf(" 0x%x",Cardreader.uid.uidByte[i]);
				}
				Serial.printf("] \n Sak= %d",Cardreader.uid.sak);
				State = eStateVerify;
				Ctrl.LEDTxRx(LEDOn);
			}else{
				Serial.printf("No card present");
				Ctrl.LEDTxRx(LEDOff);
			}
			break;
		case eStateVerify:
		  Serial.printf("State = Verify");
			//send card info to server
			if(true){ //check if server accepts card info
				CurrentCard = Cardreader.uid;
				Ctrl.Buzzer(BuzzShortBeepsPos);
			}else{
				Ctrl.Buzzer(BuzzShortBeepsNeg);
			}
			State = eStateCardPresent;
			break;
		case eStateCardPresent:
			Serial.printf("State = Card Present");
			switch(Trigger){
				case eTriggerOn:
					Serial.printf("Turn circuit on");
					Ctrl.Relay(RelOn);
					Trigger = eTriggerOff;
				break;
				case eTriggerOff:
					if (SameCard()){
						Serial.printf("Turn circuit off");
						Ctrl.Buzzer(BuzzRelOff);
						Ctrl.Relay(RelOff);
					}
					Trigger = eTriggerOn;
				break;
			}
			State = eStateRunning;
			break;
		case eStateInactive:
			State = eStateRunning;
			break;
		default:
			break;
	}
	delay(CYCLE_TIME);
	Ctrl.Update();

}*/
