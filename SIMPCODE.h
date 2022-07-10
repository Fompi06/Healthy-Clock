#pragma once
#include <Arduino.h>

#ifdef TIMER_ENABLE
#define wait(x, y) while(millis() - x <= y)
#define update(x) x=millis()
#define monitor(x, y) if(millis() - x >= y)
#define newTimer(x) uint32_t x
#else
#define wait(x, y)
#define update(x)
#define monitor(x, y)
#define newTimer(x)
#endif

#ifdef BUTTON_ENABLE
#define checkButtonGND(x) if(!digitalRead(x))
#define checkButton(x) if(digitalRead(x))
#define setButtonGND(x) pinMode(x, INPUT_PULLUP)
#define setButton(x) pinMode(x, INPUT)
#else
#define checkButtonGND(x)
#define checkButton(x)
#define setButtonGND(x)
#define setButton(x)
#endif

#ifdef LED_ENABLE
#define EnableLed(x) digitalWrite(x, 1)
#define DisableLed(x) digitalWrite(x, 0)
#define convertLed(x) digitalWrite(x, !digitalRead(x))
#define EnablePWMLed(x, y) analogWrite(x, y)
#define setLED(x) pinMode(x, OUTPUT)
#else
#define EnableLed(x)
#define DisableLed(x)
#define EnablePWMLed(x, y)
#define setLED(x)
#define convertLed(x)
#endif