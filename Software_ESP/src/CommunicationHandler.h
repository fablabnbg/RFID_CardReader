#ifndef COMMUNICATIONHANDLER_H
#define COMMUNICATIONHANDLER_H

#include <ESP8266Wifi.h>
#include <ArduinoJson.h>

#define SEND_BUFF_LEN 1024
#define REC_BUFF_LEN   512
#define RFID_STATE_RELAY_ON  0x01
#define RFID_STATE_RELAY_OFF 0x02
#define RFID_STATE_RUNNING   0x10
#define RFID_STATE_ERROR     0x20

#define RFID_SENDER_LASER    0x01
#define RFID_SENDER_FRIDGE   0x02
#define RFID_SENDER_DRILL    0x03
#define RFID_SENDER_PRINTER  0x04
#define RFID_SENDER_UNKNOWN  0xFF

#define RFID_SENDER_LASER_NAME   "Laser"
#define RFID_SENDER_FRIDGE_NAME  "Fridge"
#define RFID_SENDER_DRILL_NAME   "Drill"
#define RFID_SENDER_PRINTER_NAME "Printer"
#define RFID_SENDER_UNKNOWN_NAME "Unknown"

#define RFID_CMD_CARD_DATA   0x01 //client: card data incoming                     Send: RFIDCARDINFO Resp: COMMRESPONSE
#define RFID_CMD_REL_BRIDGED 0x02 //client: relay got bridged manually             Send: COMMHEAD Resp: COMMRESPONSE
#define RFID_CMD_DEV_OPENED  0x03 //client: sabotage! someone opened the device!   Send: COMMHEAD Resp: COMMRESPONSE
#define RFID_CMD_UNLOCK      0x04 //server: relay on until RFID_CMD_LOCK           Receive: COMMRESPONSE
#define RFID_CMD_LOCK        0x05 //server: relay off                              Receive: COMMRESPONSE

#define RFID_RESPONSE_ACK    0x00
#define RFID_RESPONSE_NAK    0xFF

#define SERVER_PORT          0xF14E
#define SERVER_MORE_DATA     0x01
#define SERVER_DATA_END      0x00
#define SERVER_BUFFER_MAX    1024

#pragma pack(push)
#pragma pack(1)

typedef struct _tagCOMMHEAD{
  uint8_t Sender[64];               //who is sending?
  uint8_t Status;               //whats the current state? Relay off, on, error ?!
  uint8_t Cmd;                  //Command what kind of packet it is
}COMMHEAD;

typedef struct _tagRFIDCARDINFO{
  COMMHEAD Head;
  uint8_t CardUID[10];          //Card UID with <ByteCnt> valid bytes
  uint8_t ByteCnt;              //byte count of UID

}RFIDCARDINFO;


typedef struct _tagCOMMRESPONSE{
  COMMHEAD Head;
  uint8_t Response;
}COMMRESPONSE;

#pragma pack(pop)

class CommunicationHandler{
public:
  CommunicationHandler();
  ~CommunicationHandler();
  bool VerifyCard(uint8_t* uid, uint8_t byteCnt, uint8_t state);
  bool QueryOpen();
  bool IsConnected();
  bool Connect(IPAddress ip, int port);
  int  SendAndReceive(WiFiClient* client, int byteSend);
  void Disconnect();

private:
  uint8_t* m_ServerBuffer;
  uint16_t m_ServerBufferLen;
  StaticJsonBuffer<SEND_BUFF_LEN> m_JsonSend;
  StaticJsonBuffer<REC_BUFF_LEN> m_JsonRec;
  char m_SendBuffer[SEND_BUFF_LEN];
  char m_RecBuffer[REC_BUFF_LEN];
  WiFiClient m_TcpClient;
  WiFiClient m_JsonClient;
};

#endif //COMMUNICATIONHANDLER_H
