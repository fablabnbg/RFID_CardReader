#ifndef RFIDSTATEMACHINE_H
#define RFIDSTATEMACHINE_H

typedef enum _tagRFIDState{
  eStateRunning = 1,
  eStateConnecting,
  eStateInactive
}RFIDState;

typedef enum _tagRFIDTrigger{
  eTriggerOn = 0,
  eTriggerOff = 1
}RFIDTrigger;

#endif //RFIDSTATEMACHINE_H
