#ifndef TIMEOUTHANDLER_H
#define TIMEOUTHANDLER_H

class TimeoutHandler{
public:
  void ResetTimeout();
  void SetTimeoutCritical();
  bool TimeoutCritical();
  bool UpdateTimeout();

private:
  int m_Timeout;
  int m_LastTime;

};

#endif //TIMEOUTHANDLER_H
