#include <i2cEncoderMiniLib.h>
//#include <i2cNavKey.h>

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <BleConnectionStatus.h>
#include <BleMouse.h>
#include <M5Core2.h>
#include "bmp280.h"

#include "mymenu.h"
#include "mymenuelement.h"
#include "flypad.h"


//#define P0 1013.25
//QNH 22.12.2020 LKPR
#define P0 1010.2
unsigned int value_QNH = 0;

const int EncoderIntPin = G27; /* Definition of the interrupt pin. You can change according to your board */
i2cEncoderMiniLib Encoder(0x20);
BMP280 bmp;

#define TOUCH_MAX_X 320

// Replace with your network credentials
const char* ssid     = "Nokia8";
const char* password = "1315backspace";

// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);

// Variables to save date and time
//String formattedDate;
//String dayStamp;
//String timeStamp;

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
unsigned long doScanWifi = 0;

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

  encoder_setup();

  DisplayInit();
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(3, 3);
  M5.Lcd.printf("FLIGHTPAD");
  //M5.Lcd.setTextColor(BLACK);
  //M5.Lcd.setCursor(10, 26);
  //M5.Lcd.printf("Press Left Button to recording!");


  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  //WiFi.begin(ssid, password);

  WiFi.onEvent(WiFiStationEvent);
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  // Disconnect from an AP if it was previously connected
  WiFi.disconnect();

  doScanWifi = millis() + 100;
  
  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }*/
/*
    // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */

/*
// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
  */

  Serial.println("Starting BLE mouse work!");
  bleMouse.begin();

  menu_setup();


}



void WiFiStationEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("Wifi event:");
    Serial.print(event);
    //Serial.print(", info:");
    //Serial.println(info);
    Serial.println();
    
     switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: 
          Serial.println("Event not found...");
          break;
    }
}

void loop() {
  M5.update();

  taskMouse();
  taskDispSlow();
  taskVibrate();
  taskBaro1();
  taskBaro2();
  taskWifiScan();
  //taskNtp();
  task_encoder();
  task_menu();

  delay(1);
}

void taskWifiScan() {
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t* bssid;
  int32_t channel;
  bool hidden;
  int scanResult;
  
  if(doScanWifi==0) return;

  if(doScanWifi>millis()) return;

  int8_t sc;

  sc = WiFi.scanComplete();
  Serial.printf(PSTR("Starting WiFi scan...%d/n"),sc);  
  
  scanResult = WiFi.scanNetworks(/*async=*/true, /*hidden=*/true);
  Serial.printf(PSTR("Scan started, result:%d\n"), scanResult);

  
  while(true) {
    sc = WiFi.scanComplete();
    if(sc==-1) {
      Serial.print(F("."));
      delay(500);  
    } else if(sc < 0) {
      Serial.printf(PSTR("Scan error:%d/n"),sc);
      return;
    } else {
      Serial.printf(PSTR("Scan result:%d/n"),sc);
      break;
    }
  }  
  scanResult = sc;

  if (scanResult == 0) {
    Serial.println(F("No networks found"));
  } else if (scanResult > 0) {
    Serial.printf(PSTR("%d networks found:\n"), scanResult);

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++) {
      //WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel);

      Serial.printf(PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %s\n"),
                    i,
                    channel,
                    bssid[0], bssid[1], bssid[2],
                    bssid[3], bssid[4], bssid[5],
                    rssi,
                    //(encryptionType == ENC_TYPE_NONE) ? ' ' : '*',
                    '?',
                    hidden ? 'H' : 'V',
                    ssid.c_str());
      delay(0);
    }
  } else {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }

  

  doScanWifi = 0;
}
/*
unsigned long lastRunNtp = 0;
void taskNtp() {
  unsigned long age = millis() - lastRunNtp;
  if(age<1000) return;
  lastRunNtp = millis();

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedTime();
  Serial.println("Time: "+formattedDate); 
  
    writeToRectangle(LIGHTGREY, BLACK, 200, formattedDate.c_str());

}*/

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
double baro_sealevel1 = -1;
double baro_sealevel2 = -1;
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
    baro_sealevel1 = -1;
    baro_sealevel2 = -1;
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

  baro_a = bmp.altitude(baro_p, value_QNH);
  baro_sealevel1 = bmp.sealevel(baro_p, 350);
  baro_sealevel2 = bmp.sealevel(baro_p, 202);
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

const word BOX_LEFT = 254 - 110;
const word BOX_WIDTH = 64 + 110;
const word BOX_S_WIDTH = 110+30;
const word SBOX_HEIGHT = 20;
const word LBOX_HEIGHT = 34;
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

    sprintf(txt, "%07.2f mBar", baro_sealevel1);
    writeToRectangle(LIGHTGREY, BLACK, 140, txt);
    sprintf(txt, "%07.2f mBar", baro_sealevel2);
    writeToRectangle(LIGHTGREY, BLACK, 160, txt);

  /*
    if(WiFi.status() != WL_CONNECTED) {
     sprintf(txt, "NOT CONN:");
     writeToRectangle(RED, BLACK, 180, txt);
    } else {
      //writeToRectangle(GREEN, BLACK, 180, String(WiFi.localIP()).c_str());
      writeToRectangle(GREEN, BLACK, 180, WiFi.localIP().toString().c_str());
    }
    */

    sprintf(txt, "%04.0f ft", baro_a*3.2808399);
    writeToRectangleBig(WHITE, BLACK, 180, txt);
    
    
    
  } else {
    writeToRectangle(RED, BLACK, 80, TXT_MIS);
    writeToRectangle(RED, BLACK, 100, "baro missing");
    writeToRectangle(RED, BLACK, 120, TXT_MIS);
  }


}

void writeToRectangle(uint32_t color, uint32_t backgroud, word y, const char* text) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(BOX_LEFT, y, BOX_WIDTH, SBOX_HEIGHT, backgroud);
  M5.Lcd.drawRect(BOX_LEFT, y, BOX_WIDTH, SBOX_HEIGHT, ORANGE);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(BOX_LEFT + 4, y + 3);
  M5.Lcd.print(text);
}

void writeToRectangleBig(uint32_t color, uint32_t backgroud, word y, const char* text) {
  M5.Lcd.setTextSize(4);
  M5.Lcd.fillRect(BOX_LEFT, y, BOX_WIDTH, LBOX_HEIGHT, backgroud);
  M5.Lcd.drawRect(BOX_LEFT, y, BOX_WIDTH, LBOX_HEIGHT, ORANGE);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(BOX_LEFT + 4, y + 3);
  M5.Lcd.print(text);
}

void writeToRectangleLeft(uint32_t color, uint32_t backgroud, word y, const char* text) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(0, y, BOX_S_WIDTH, SBOX_HEIGHT, backgroud);
  M5.Lcd.drawRect(0, y, BOX_S_WIDTH, SBOX_HEIGHT, ORANGE);
  M5.Lcd.setTextColor(color);
  M5.Lcd.setCursor(0 + 4, y + 3);
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


//I2C Encoder Mini part:
//unsigned long last_encoder_run = 0;
void task_encoder() {
  if (digitalRead(EncoderIntPin) != LOW) return;
  
  //unsigned long age = millis() - last_encoder_run;
  //if(age<10) return;

  //last_encoder_run = millis();
  Encoder.updateStatus(); 
}

void encoder_setup() {
  pinMode(EncoderIntPin, INPUT);
  Encoder.reset();
  Encoder.begin(i2cEncoderMiniLib::WRAP_DISABLE
                | i2cEncoderMiniLib::DIRE_LEFT | i2cEncoderMiniLib::IPUP_ENABLE
                | i2cEncoderMiniLib::RMOD_X1 );

  Encoder.writeCounter((int32_t) 0); /* Reset the counter value */
  Encoder.writeMax((int32_t) 10); /* Set the maximum threshold*/
  Encoder.writeMin((int32_t) - 10); /* Set the minimum threshold */
  Encoder.writeStep((int32_t) 1); /* Set the step to 1*/
  Encoder.writeDoublePushPeriod(50); /*Set a period for the double push of 500ms */

  // Definition of the events
  Encoder.onIncrement = menu_go_prev;
  Encoder.onDecrement = menu_go_next;
  
  Encoder.onChange = encoder_change;
  Encoder.onMax = encoder_max;
  Encoder.onMin = encoder_min;
  Encoder.onButtonPush = encoder_push;
  Encoder.onButtonRelease = encoder_released;
  Encoder.onButtonDoublePush = encoder_double_push;
  Encoder.onButtonLongPush = menu_go_start;

  /* Enable the I2C Encoder V2 interrupts according to the previus attached callback */
  Encoder.autoconfigInterrupt();
}

void encoder_show(bool button) {
  char txt[40];
  
  int8_t val = Encoder.readCounterByte(); 
  uint8_t st = Encoder.readStatus();
  sprintf(txt, "%+03d st:%02X", val,st);
  writeToRectangle(LIGHTGREY, button?BLUE:BLACK, 214, txt);
}

//Callback when the CVAL is incremented
void encoder_change(i2cEncoderMiniLib* obj) {  
  encoder_show(false);
}


//Callback when CVAL reach MAX
void encoder_max(i2cEncoderMiniLib* obj) {
  Serial.print("Maximum threshold: ");
  Serial.println(Encoder.readCounterByte());
}

//Callback when CVAL reach MIN
void encoder_min(i2cEncoderMiniLib* obj) {
  Serial.print("Minimum threshold: ");
  Serial.println(Encoder.readCounterByte());
}

//Callback when the encoder is pushed
void encoder_push(i2cEncoderMiniLib* obj) {
  Serial.println("Encoder is pushed!");
  encoder_show(true);
  menu_actual->go_down();  
}

//Callback when the encoder is released
void encoder_released(i2cEncoderMiniLib* obj) {
  Serial.println("Encoder is released");
}

//Callback when the encoder is double pushed
void encoder_double_push(i2cEncoderMiniLib* obj) {
  Serial.println("Encoder is double pushed!");
}

//Callback when the encoder is long pushed
void encoder_long_push(i2cEncoderMiniLib* obj) {
  Serial.println("Encoder is long pushed!");
}


//Menu definition

//Gesture swipeDown(topHalf, bottomHalf, "Swipe Down");

extern MYMENU_SELECT mm_info;
extern MYMENU_SELECT mm_set_alt;
extern MYMENU_SELECT mm_set_qnh;
extern MYMENU_NINP mm_sqnh_1;
extern MYMENU_NINP mm_sqnh_2;
extern MYMENU_NINP mm_sqnh_3;
extern MYMENU_NINP mm_sqnh_4;
extern MYMENU_CHECK  mm_sqnh_done;

extern MYMENU_MSG  mm_empty;



MYMENU_SELECT mm_info("main info",NULL, &mm_set_alt,&mm_empty);
MYMENU_SELECT mm_set_alt("set altitude",&mm_info,&mm_set_qnh,&mm_empty);
MYMENU_SELECT mm_set_qnh("set QNH",&mm_set_alt,NULL,&mm_sqnh_1);

MYMENU_NINP mm_sqnh_1(1,1000,&mm_sqnh_2,mm_sqnh_show);
MYMENU_NINP mm_sqnh_2(0,100,&mm_sqnh_3,mm_sqnh_show);
MYMENU_NINP mm_sqnh_3(1,10,&mm_sqnh_4,mm_sqnh_show);
MYMENU_NINP mm_sqnh_4(3,1,&mm_info,mm_sqnh_show);
MYMENU_CHECK  mm_sqnh_done(2000,&mm_info);

MYMENU_MSG  mm_empty("EMPTY SELECTION",500,&mm_info);

MYMENUELEMENT* menu_actual = &mm_info;

void mm_sqnh_show() {
  char txt[40];
  mm_sqnh_calc();
  sprintf(txt, "QNH:%04dhPa", value_QNH);
  bool range_ok = (value_QNH>960) && (value_QNH<1040);
  writeToRectangleLeft(range_ok?GREEN:RED, BLACK, 40, txt);   
}
void mm_sqnh_calc() {
  value_QNH = mm_sqnh_1.getVX() + mm_sqnh_2.getVX() + mm_sqnh_3.getVX() + mm_sqnh_4.getVX() ;
}

void menu_setup() {
  mm_sqnh_calc();
  menu_actual->show();
}

void task_menu() {

}

void menu_go_prev(i2cEncoderMiniLib* obj) {
  menu_actual->go_prev();
}

void menu_go_next(i2cEncoderMiniLib* obj) {
  menu_actual->go_next();
}

void menu_go_start(i2cEncoderMiniLib* obj) {
  menu_actual = &mm_info;
  menu_actual->show();
}
