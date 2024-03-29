#include <Joystick.h>

#define DEBUG
//#undef  DEBUG

#define CW 3
#define DT 2
#define CYCLE_ENGINE_COMMAND 8
#define DEBOUNCE_TIME 5

#define BLUE_LED A0
#define GREEN_LED A1
#define RED_LED A2

#define POWR 0
#define PROP 1
#define MIXT 2

#define STEPS 2

volatile int lastState = LOW;

#define THROTTLE  0
#define PROP  1
#define MIXTURE  2

unsigned long engineSelectDelay;

volatile int SELECTED = 0;
volatile int value[3] = {0,250,250};

volatile unsigned long timestamp;

Joystick_ joy(
  0x03,                      // hidReportId 
  JOYSTICK_TYPE_JOYSTICK,    // joystickType
  0,                         // buttonCount 
  0,                         // hatSwitchCount 
  false,                     // includeXAxis 
  false,                     // includeYAxis 
  false,                     // includeZAxis 
  false,                     // includeRxAxis 
  false,                     // includeRyAxis 
  false,                     // includeRzAxis 
  false,                     // includeRudder 
  true,                      // includeThrottle 
  true,                      // includeAccelerator
  true,                      // includeBrake 
  false                      // includeSteering 
);

void setup() {
  pinMode(CW, INPUT);
  pinMode(DT, INPUT);
  pinMode(CYCLE_ENGINE_COMMAND, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CW), push_engine_value, CHANGE);
  timestamp = millis();

  pinMode(BLUE_LED, INPUT);
  pinMode(RED_LED, INPUT);
  pinMode(GREEN_LED, INPUT);

  joy.begin();
  joy.setThrottleRange(0, 250);
  joy.setAcceleratorRange(0, 250);
  joy.setBrakeRange(0, 250);

  joy.setThrottle(value[THROTTLE]);
  joy.setAccelerator(value[PROP]);
  joy.setBrake(value[MIXTURE]);

  selectEngine(THROTTLE);

  engineSelectDelay = millis();

  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {

  unsigned char cE = digitalRead(CYCLE_ENGINE_COMMAND);
  if( ( millis() - engineSelectDelay ) >= 350 && cE == LOW ) {
    SELECTED = (SELECTED + 1) % 3;
    selectEngine(SELECTED);
    
    #ifdef DEBUG
      Serial.print("Cycle Engine ");Serial.println(SELECTED);
    #endif
    
    engineSelectDelay = millis();
  }

}

void push_engine_value() {

  unsigned long t = millis();
  if( t - timestamp < DEBOUNCE_TIME ) return;

  int state = digitalRead(CW);
  if( state != lastState ) {
    if( state = digitalRead(DT) ) {
      value[SELECTED] += STEPS;
    } else  {
      value[SELECTED] -= STEPS;
    }

    #ifdef DEBUG
      Serial.print("value[SELECTED]: "); Serial.println(value[SELECTED]);
    #endif

    int l = constrain(value[SELECTED], 0, 250);
    value[SELECTED] = l;

    switch(SELECTED) {
      case THROTTLE:
        joy.setThrottle(value[SELECTED]);
        #ifdef DEBUG
          Serial.print("THROTTLE: ");Serial.println(value[SELECTED]);
        #endif
        break;
      case PROP:
        joy.setAccelerator(value[SELECTED]);
        #ifdef DEBUG
          Serial.print("PROP: ");Serial.println(value[SELECTED]);
        #endif
        break;
      case MIXTURE:
        joy.setBrake(value[SELECTED]);
        #ifdef DEBUG
          Serial.print("MIXTURE: ");Serial.println(value[SELECTED]);
        #endif
        break;
    }

  }

  timestamp = t;

}

void selectEngine(uint16_t selection) {
  switch(selection) {
    case THROTTLE: // THROTTLE. WHITE
      analogWrite(BLUE_LED, 250);
      analogWrite(GREEN_LED, 250);
      analogWrite(RED_LED, 250);
      SELECTED = THROTTLE;
      break;
    case PROP: // PROP. BLUE
      analogWrite(BLUE_LED, 250);
      analogWrite(GREEN_LED, 0);
      analogWrite(RED_LED, 0);
      SELECTED = PROP;
      break;
    case MIXTURE: // MIXTURE. RED
      analogWrite(BLUE_LED, 0);
      analogWrite(GREEN_LED, 0);
      analogWrite(RED_LED, 250);
      SELECTED = MIXTURE;
      break;
  }
}
