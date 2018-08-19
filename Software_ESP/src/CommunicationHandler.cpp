#include "CommunicationHandler.h"
#include "example_config.h"
#include <ArduinoJson.h>
#include <WiFiServer.h>

CommunicationHandler::CommunicationHandler(){

}

CommunicationHandler::~CommunicationHandler(){

}
bool CommunicationHandler::VerifyCard( uint8_t* uid, uint8_t byteCnt, uint8_t state){
  Serial.println("Verify Card start");
  if( !uid) return false;
  if(!IsConnected()) return false;

  COMMRESPONSE resp = {0};

  char sUid[10] = {0};
  for(int i = 0; i < byteCnt; i++){sprintf(sUid,"%02x",uid[i]);}

  int iReceivedBytes = 0;
  int iSendBytes = 0;
  Serial.println("Verify Card create JSON");
  JsonObject& objSend = m_JsonSend.createObject();
  // objRec = m_JsonRec.createObject();
  objSend["Sender"] = Sender;
  objSend["Status"] = state;
  objSend["Cmd"] = RFID_CMD_CARD_DATA;
  objSend["CardUID"] = sUid;
  objSend["ByteCnt"] = byteCnt;
  if(!objSend.success()) return -1;
  if(!objSend.printTo(m_SendBuffer)) return -1;
  iSendBytes = strlen_P(m_SendBuffer);
  if(iSendBytes >= (SEND_BUFF_LEN - 1) ) return -1; //1023 cause we need an extra \n
  m_SendBuffer[iSendBytes]='\n';
  m_SendBuffer[iSendBytes+1]=0;
  iSendBytes++;
  iReceivedBytes = SendAndReceive(&m_JsonClient, iSendBytes);
  JsonObject& objRec = m_JsonRec.parseObject(m_RecBuffer);

  if(!objRec.success()) return -1;

  Serial.printf("%d bytes received!\n",iReceivedBytes);

  if(iReceivedBytes > 0){
    Serial.println("parsing succeeded");
    resp.Response = objRec["Response"];
    Serial.printf("Response = %d\n",resp.Response);
  }
  return (resp.Response == RFID_RESPONSE_ACK);
}

bool CommunicationHandler::QueryOpen(){

}

bool CommunicationHandler::IsConnected(){
  return m_JsonClient.connected();
}

bool CommunicationHandler::Connect(IPAddress ip, int port){
  Serial.printf("Connect start to %s at port: %d\n", ip.toString().c_str(), port);
  if( m_TcpClient.connect(ip, port) ){
    Serial.println("Connected to Port Server");
    uint16_t iPort = 0;
    m_SendBuffer[0]='a';
     m_SendBuffer[1]=0;
    int iRecBytes = SendAndReceive(&m_TcpClient, 1);
    if(iRecBytes >= sizeof(uint16_t)){
      iPort =*(uint16_t*)m_RecBuffer;
      Serial.printf("Port No received: %d", iPort);
      if(m_JsonClient.connect(ip, iPort)){
        m_TcpClient.stop();
        return true;
      }
      Serial.println("Connection to Json client failed!");
    }
    Serial.println("Wrong No of bytes received");
    m_TcpClient.stop();
  }
  Serial.println("\nConnect end");
  return false;
}

int  CommunicationHandler::SendAndReceive(WiFiClient* client, int byteSend){
  Serial.printf("%d bytes sent\n",byteSend);
  client->write(m_SendBuffer, byteSend);

  int iCommTimeout = millis();
  Serial.println("Waiting for data to receive");
  while( !(client->available()) && ( (millis() - iCommTimeout) < 5000)  ){
    Serial.print(".");
    delay(10);
  }
  if( ( (millis() - iCommTimeout) > 5000) ){
    Serial.println("Timeout expired!");
    return -1;
  }
  return client->read((uint8_t*)m_RecBuffer, REC_BUFF_LEN);

}

void CommunicationHandler::Disconnect(){
  Serial.println("Comm Disconnect");
  m_TcpClient.stop();
  m_JsonClient.stop();

}
