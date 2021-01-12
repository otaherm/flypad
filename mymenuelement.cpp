#include "mymenu.h"
#include "mymenuelement.h"
#include <M5Core2.h>
#include "flypad.h"



MYMENUELEMENT::MYMENUELEMENT(const char* const _title,  MYMENUELEMENT* next,  MYMENUELEMENT* prev)
  : title(_title),
    next(next),
    prev(prev),
    cb_down(NULL) {  }

const char* MYMENUELEMENT::getTitle() {
  return this->title;
}

void MYMENUELEMENT::setDownCallback(void (*const cb_down)(void)) {
  this->cb_down = cb_down;
}

void MYMENUELEMENT::go_prev() {
  if (prev != NULL) {
    menu_actual = prev;
    menu_actual->enter();
  }
  menu_actual->show();
}

void MYMENUELEMENT::go_next() {
  if (next != NULL) {
    menu_actual = next;
    menu_actual->enter();
  }
  menu_actual->show();
}

void MYMENUELEMENT::go_down() {
  if (cb_down != NULL) {
    cb_down();
    menu_actual->enter();
  }
}

void MYMENUELEMENT::enter() {
  millis_entered = millis();
}

bool MYMENUELEMENT::has_own_screen() {
  return false;
}

bool MYMENUELEMENT::isSelected() {
  return (this == menu_actual);
}

void MYMENUELEMENT::task() {

}

MYMENU_SELECT::MYMENU_SELECT(const char* const title, MYMENUELEMENT* next, MYMENUELEMENT* prev, MYMENUELEMENT* const down)
  : MYMENUELEMENT(title, next, prev),
    down(down) {
}; // base type

void MYMENU_SELECT::go_down() {
  if (cb_down != NULL) cb_down();
  if (down != NULL) {
    menu_actual = down;
    menu_actual->enter();
  }
  menu_actual->show();
}


MYMENU_NINP::MYMENU_NINP(MYMENUELEMENT* next, void (*const cb_show)(void), int value_min, int value_max)
  : MYMENUELEMENT(NULL, next, NULL),
    cb_show(cb_show),
    value_min(value_min),
    value_max(value_max) {
};
void MYMENU_NINP::go_down() {
  if (cb_down != NULL) cb_down();
  if (next != NULL) {
    menu_actual = next;
    menu_actual->enter();
  }
  menu_actual->show();
}
void MYMENU_NINP::go_next() {
  if (value > value_min) {
    value--;
  } else if(value < value_min) {
    value = value_min;
  }
  menu_actual->show();
}
void MYMENU_NINP::go_prev() {
  if (value < value_max) {
    value++;
  } else if(value > value_max) {
    value = value_max;
  }
  menu_actual->show();
}
void MYMENU_NINP::show() {
  if (cb_show != NULL) cb_show();
}
void MYMENU_NINP::draw(int idx, int x, int y, int width, int height, int count) {
  const int w1 = width / count;
  const bool selected = this->isSelected();
  if (!selected) return;
  M5.Lcd.drawRect(x + idx * w1 + 1, y + 1, w1 - 2, height - 2, WHITE);
  M5.Lcd.drawRect(x + idx * w1 + 2, y + 2, w1 - 4, height - 4, WHITE);
  //M5.Lcd.drawRect(x+idx*w1+3, y+3, w1-6, height-6, selected?ORANGE:DARKGREY);
  //M5.Lcd.drawRect(x+idx*w1+4, y+4, w1-8, height-8, selected?ORANGE:DARKGREY);
}


bool MYMENU_NINP::has_own_screen() {
  return true;
}


MYMENU_MSG::MYMENU_MSG(const char* const title, const unsigned long delay_ms,  void (*const cb_show)(void),MYMENUELEMENT* next)
  : MYMENUELEMENT(title, next, NULL),
    delay_ms(delay_ms),
    cb_show(cb_show) {
};
bool MYMENU_MSG::has_own_screen() {
  return true;
}
void MYMENU_MSG::show() {
  MYMENUELEMENT::show();
  if (cb_show != NULL) cb_show();
}

MYMENU_CHECK::MYMENU_CHECK(const unsigned long delay_ms, MYMENUELEMENT* next)
  : MYMENUELEMENT("", next, NULL),
    delay_ms(delay_ms) {

};
void MYMENU_MSG::go_down() {
  if (cb_down != NULL) cb_down();
  go_next();
}
void MYMENU_MSG::go_prev() {
  go_next();
}

void MYMENU_MSG::task() {
  if ((millis_entered + delay_ms) > millis())
    return;

  go_next();
}

void MYMENUELEMENT::show() {
  char txt[40];
  sprintf(txt, "%.11s", title);
  writeToRectangleLeft(GREEN, BLACK, 40, txt);
}
