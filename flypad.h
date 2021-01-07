#ifndef FLYPAD_h
#define FLYPAD_h

#include "mymenuelement.h"


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

extern MYMENUELEMENT* menu_actual;

void writeToRectangle(uint32_t color, uint32_t backgroud, word y, const char* text);
void writeToRectangleBig(uint32_t color, uint32_t backgroud, word y, const char* text);
void writeToRectangleLeft(uint32_t color, uint32_t backgroud, word y, const char* text);

#endif
