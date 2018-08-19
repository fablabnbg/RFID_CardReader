#include<TimeoutHandler.h>
#include<example_config.h>

void TimeoutHandler::ResetTimeout(){
  m_Timeout = Timeout_Val;
  m_LastTime = millis();
}
void TimeoutHandler::SetTimeoutCritical(){
  m_Timeout = Timeout_Critical;
}
bool TimeoutHandler::TimeoutCritical(){
  return ( ( m_Timeout <= Timeout_Critical ) && ( m_Timeout > 0 ) ); //Critical but not expired
}
bool TimeoutHandler::UpdateTimeout(){
  m_Timeout -= (millis() - m_LastTime);
  m_LastTime = millis();
  if(m_Timeout < 0){ m_Timeout = 0; }
  Serial.printf("Timeout %d \n", m_Timeout);
  return m_Timeout > 0;
}
