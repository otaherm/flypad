#include <BleConnectionStatus.h>
#include <BleMouse.h>

#include <M5Core2.h>

#define TOUCH_MAX_X 320

//TouchZone topHalf(0,0,320,120);
//TouchZone bottomHalf(0,120,320,160);
//Gesture swipeDown(topHalf, bottomHalf, "Swipe Down");

//TouchButton lt = TouchButton(0, 0, 160, 120, "left-top");
//TouchButton lb = TouchButton(0, 120, 160, 120, "left-bottom");
//TouchButton rt = TouchButton(160, 0, 160, 120, "right-top");
//TouchButton rb = TouchButton(160, 120, 160, 120, "right-bottom");

TouchButton btn_left = TouchButton(0, 240, 109, 40, "left");
TouchButton btn_middle = TouchButton(110, 240, 109, 40, "middle");
TouchButton btn_right = TouchButton(218, 240, 109, 40, "right");

BleMouse bleMouse("Flightpad", "HERM.CZ", 100);

const bool DEBUG = false;

void DisplayInit(void)
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
}

void setup() {
  M5.begin();
  M5.Touch.addHandler(eventDisplay);
  M5.Touch.addHandler(colorButtons, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  //swipeDown.addHandler(yayWeSwiped);
  //rt.addHandler(dblTapped, TE_DBLTAP);

  btn_left.addHandler(buttonPressed);
  btn_middle.addHandler(buttonPressed);
  btn_right.addHandler(buttonPressed);

  DisplayInit();
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(3, 3);
  M5.Lcd.printf("FLIGHTPAD");
  //M5.Lcd.setTextColor(BLACK);
  //M5.Lcd.setCursor(10, 26);
  //M5.Lcd.printf("Press Left Button to recording!");

  Serial.println("Starting BLE mouse work!");
  bleMouse.begin();


}

void loop() {
  M5.update();

  taskMouse();
  taskDispSlow();

  delay(1);
}

int sum_x = 0;
int sum_y = 0;

unsigned long lastRunMouse = millis();
void taskMouse() {
  unsigned long now = millis();
  //if(now - lastRunMouse < 1) return;
  if(sum_x == 0 && sum_y == 0) return;

  lastRunMouse = now;
  
  if(bleMouse.isConnected() && (sum_x != 0 || sum_y != 0)) {
    bleMouse.move(sum_x, sum_y);    
  }
  sum_x = 0;
  sum_y = 0;

  
}

const word BOX_LEFT = 254;
const word BOX_WIDTH = 64;
const word BOX_HEIGHT = 20;
unsigned long lastDispSlow = millis();
void taskDispSlow() {
  unsigned long now = millis();
  if (now - lastDispSlow < 10000) return;

  lastDispSlow = now;

  //Serial.printf("BAT: %3.1fV\r\n", M5.Axp.GetBatVoltage());
   

  char txt[40];

if (bleMouse.isConnected()) { 
  writeToRectangle(BLUE,BLACK,0,"CONN");  
} else {
  writeToRectangle(RED,BLACK,0,"!!!!");  
  }

  sprintf(txt,"%3.1fV",M5.Axp.GetBatVoltage());
  writeToRectangle(LIGHTGREY,BLACK,20,txt);  
  
  sprintf(txt,"%+03.0fmA",M5.Axp.GetBatCurrent());
  writeToRectangle(LIGHTGREY,BLACK,40,txt);

  sprintf(txt,"%03.0fmA",M5.Axp.GetVinCurrent());
  writeToRectangle(LIGHTGREY,BLACK,60,txt);
}

void writeToRectangle(uint32_t color,uint32_t backgroud,word y,const char* text) {
  M5.Lcd.fillRect(BOX_LEFT, y, BOX_WIDTH, BOX_HEIGHT, backgroud);
  M5.Lcd.drawRect(BOX_LEFT, y, BOX_WIDTH, BOX_HEIGHT, ORANGE);
  M5.Lcd.setTextColor(color);  
  M5.Lcd.setCursor(BOX_LEFT+4, y+3);
  M5.Lcd.print(text);  
}

void eventDisplay(TouchEvent& e) {
  if(DEBUG) {
    Serial.printf("%-12s finger%d  %-18s (%3d, %3d)", M5.Touch.eventTypeName(e), e.finger, M5.Touch.eventObjName(e),  e.from.x, e.from.y);
    if (e.type != TE_TOUCH && e.type != TE_TAP && e.type != TE_DBLTAP) {
      Serial.printf("--> (%3d, %3d)  %5d ms", e.to.x, e.to.y, e.duration);
    }
    Serial.println();
  }
  if (e.type == TE_MOVE) {
    //bleMouse.move(e.to.x - e.from.x, e.to.y - e.from.y);
    sum_x += e.to.x - e.from.x;
    sum_y += e.to.y - e.from.y;
  } else if (e.type == TE_DBLTAP) {
    bleMouse.click(MOUSE_LEFT);
  }
}

void colorButtons(TouchEvent& e) {
  TouchButton& b = *e.button;
  M5.Lcd.fillRect(b.x, b.y, b.w, b.h, b.isPressed() ? WHITE : BLACK);
}

void yayWeSwiped(TouchEvent& e) {
  if(DEBUG) Serial.println("--- SWIPE DOWN DETECTED ---");
}

void dblTapped(TouchEvent& e) {
  if(DEBUG) Serial.println("--- TOP RIGHT BUTTON WAS DOUBLETAPPED ---");
}

void buttonPressed(TouchEvent& e) {
  TouchButton& b = *e.button;
  if(DEBUG) Serial.printf("Button: %s: event: %s\n\n", b.name, M5.Touch.eventObjName(e));

  if (!bleMouse.isConnected()) {
    if(DEBUG) Serial.println("Mouse not connected!");
    return;
  }

  if (&b == &btn_left) {
    if(DEBUG) Serial.println("DETECTED LEFT BUTTON");
    bleMouse.click(MOUSE_LEFT);
  } else if (&b == &btn_middle) {
    if(DEBUG) Serial.println("DETECTED MIDDLE BUTTON");
    bleMouse.move(10, 0);
  } else if (&b == &btn_right) {
    if(DEBUG) Serial.println("DETECTED RIGHT BUTTON");
    bleMouse.move(-10, 0);
  } else {
    if(DEBUG) Serial.println("BUTTON NOT RECOGNIZED!");
  }
}
