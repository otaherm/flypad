#include <BleConnectionStatus.h>
#include <BleMouse.h>
#include <M5Core2.h>
#include "bmp280.h"

//#define P0 1013.25
//QNH 22.12.2020 LKPR
#define P0 1016.8

BMP280 bmp;

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
const long CLICK_DELAY = 300;

unsigned long vibrate = 0;

void DisplayInit(void)
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
}

bool bmp_ok = false;

void setup() {
  M5.begin();
  M5.Touch.addHandler(eventDisplay);
  M5.Touch.addHandler(colorButtons, TE_BTNONLY + TE_TOUCH + TE_RELEASE);
  //swipeDown.addHandler(yayWeSwiped);
  //rt.addHandler(dblTapped, TE_DBLTAP);

  if (bmp_ok = bmp.begin()) {
    Serial.println("BMP init success!");
    bmp.setOversampling(16);
    //M5.Lcd.drawString("BPS Example", 90, 0, 4);
  } else {
    Serial.println("BMP init failed!");
  }

  btn_left.addHandler(buttonPressed);
  btn_middle.addHandler(buttonPressed);
  btn_right.addHandler(buttonPressed);

  M5.Axp.SetLed(0);

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
  taskVibrate();
  taskBaro1();
  taskBaro2();

  delay(1);
}

bool isVibrating = false;

void taskVibrate() {
  const bool vibrate_now = vibrate >= millis();
  if (vibrate_now == isVibrating) return;


  M5.Axp.SetLDOEnable(3, vibrate_now ? 1 : 0);
  isVibrating = vibrate_now;
}

unsigned long last_baro = millis();
double baro_t = -1;
double baro_p = -1;
double baro_a = -1;
double baro_sealevel = -1;
unsigned long run_baro2 = 0;
void taskBaro1() {
  unsigned long age = millis() - last_baro;
  if (age < 1500) return;

  last_baro = millis();

  if (!bmp_ok) return;

  //char result = bmp.startMeasurment();
  char wait_ms = bmp.startMeasurment();
  if (wait_ms == 0) {
    baro_a = -1;
    baro_t = -1;
    baro_p = -1;
    baro_sealevel = -1;
    Serial.println("BMP Error 1.");
    return;
  }

  run_baro2 = millis() + wait_ms;
}

void taskBaro2() {
  if (run_baro2 == 0) return;
  if (run_baro2 > millis()) return;

  run_baro2 = 0;

  char result = bmp.getTemperatureAndPressure(baro_t, baro_p);
  if (result == 0) {
    Serial.println("BMP Error 2.");
    return;
  }

  baro_a = bmp.altitude(baro_p, P0);
  baro_sealevel = bmp.sealevel(baro_p, 350);
  //Serial.print("T = \t");Serial.print(baro_t,2); Serial.print(" degC\t");
  //Serial.print("P = \t");Serial.print(baro_p,2); Serial.print(" mBar\t");
  //Serial.print("A = \t");Serial.print(baro_a,2); Serial.println(" m");
  //display_result(T, P, A);
}

int sum_x = 0;
int sum_y = 0;
long last_TE_RELEASE = -1;

unsigned long lastRunMouse = millis();
void taskMouse() {
  unsigned long now = millis();
  //if(now - lastRunMouse < 1) return;
  if (sum_x == 0 && sum_y == 0) return;

  lastRunMouse = now;

  if (bleMouse.isConnected() && (sum_x != 0 || sum_y != 0)) {
    bleMouse.move(sum_x, sum_y);
  }
  sum_x = 0;
  sum_y = 0;


}

const word BOX_LEFT = 254 - 90;
const word BOX_WIDTH = 64 + 90;
const word BOX_HEIGHT = 20;
unsigned long lastDispSlow = millis();
void taskDispSlow() {
  unsigned long now = millis();
  if (now - lastDispSlow < 1500) return;

  lastDispSlow = now;

  //Serial.printf("BAT: %3.1fV\r\n", M5.Axp.GetBatVoltage());


  char txt[40];

#define TXT_ERR "!!!!!!!!!!!!"
#define TXT_MIS "------------"

  if (bleMouse.isConnected()) {
    writeToRectangle(BLUE, BLACK, 0, " CONNECTED");
  } else {
    writeToRectangle(RED, BLACK, 0, TXT_ERR);
  }

  sprintf(txt, "   %+3.1f V", M5.Axp.GetBatVoltage());
  writeToRectangle(LIGHTGREY, BLACK, 20, txt);

  sprintf(txt, "   %+04.0f mA", M5.Axp.GetBatCurrent());
  writeToRectangle(LIGHTGREY, BLACK, 40, txt);

  sprintf(txt, "   %+04.0f mA", M5.Axp.GetVinCurrent());
  writeToRectangle(LIGHTGREY, BLACK, 60, txt);

  if (bmp_ok) {
    sprintf(txt, " %03.1f degC", baro_t);
    writeToRectangle(LIGHTGREY, BLACK, 80, txt);

    sprintf(txt, "%07.2f mBar", baro_p);
    writeToRectangle(LIGHTGREY, BLACK, 100, txt);

    sprintf(txt, " %07.2f m", baro_a);
    writeToRectangle(LIGHTGREY, BLACK, 120, txt);

    sprintf(txt, "%07.2f mBar", baro_sealevel);
    writeToRectangle(LIGHTGREY, BLACK, 140, txt);
  } else {
    writeToRectangle(RED, BLACK, 80, TXT_MIS);
    writeToRectangle(RED, BLACK, 100, "baro missing");
    writeToRectangle(RED, BLACK, 120, TXT_MIS);
  }


}

void writeToRectangle(uint32_t color, uint32_t backgroud, word y, const char* text) {
  M5.Lcd.fillRect(BOX_LEFT, y, BOX_WIDTH, BOX_HEIGHT, backgroud);
  M5.Lcd.drawRect(BOX_LEFT, y, BOX_WIDTH, BOX_HEIGHT, ORANGE);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(BOX_LEFT + 4, y + 3);
  M5.Lcd.print(text);
}

bool button_down = false;

void eventDisplay(TouchEvent& e) {
  long age;
  if (e.type == TE_RELEASE) {
    last_TE_RELEASE = millis();
    if (button_down) {
      bleMouse.release(MOUSE_LEFT);
      if (DEBUG) Serial.println("LEFT BTN RELEASE");

      vibrate = millis() + 60;


      button_down = false;
    }

  } else if (e.type == TE_TOUCH) {
    age = millis() - last_TE_RELEASE;
    if (age < 300) {
      //button_down = true;
      bleMouse.press(MOUSE_LEFT);
      if (DEBUG) Serial.println("LEFT BTN PRESS");
      vibrate = millis() + 160;
      button_down = true;
    } else {
      //button_down = false;
    }
  }

  if (DEBUG) {
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
    //bleMouse.click(MOUSE_LEFT);
  }
}

void colorButtons(TouchEvent& e) {
  TouchButton& b = *e.button;
  M5.Lcd.fillRect(b.x, b.y, b.w, b.h, b.isPressed() ? WHITE : BLACK);
}

void yayWeSwiped(TouchEvent& e) {
  if (DEBUG) Serial.println("--- SWIPE DOWN DETECTED ---");
}

void dblTapped(TouchEvent& e) {
  if (DEBUG) Serial.println("--- TOP RIGHT BUTTON WAS DOUBLETAPPED ---");
}

void buttonPressed(TouchEvent& e) {
  TouchButton& b = *e.button;
  if (DEBUG) Serial.printf("Button: %s: event: %s\n\n", b.name, M5.Touch.eventObjName(e));

  if (!bleMouse.isConnected()) {
    if (DEBUG) Serial.println("Mouse not connected!");
    return;
  }

  if (&b == &btn_left) {
    if (DEBUG) Serial.println("DETECTED LEFT BUTTON");
    bleMouse.click(MOUSE_LEFT);
  } else if (&b == &btn_middle) {
    if (DEBUG) Serial.println("DETECTED MIDDLE BUTTON");
    bleMouse.move(10, 0);
  } else if (&b == &btn_right) {
    if (DEBUG) Serial.println("DETECTED RIGHT BUTTON");
    bleMouse.move(-10, 0);
  } else {
    if (DEBUG) Serial.println("BUTTON NOT RECOGNIZED!");
  }
}
