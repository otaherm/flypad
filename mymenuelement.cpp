#include "mymenu.h"
#include "mymenuelement.h"
#include <M5Core2.h>
#include "flypad.h"



MYMENUELEMENT::MYMENUELEMENT(const char* const _title,  MYMENUELEMENT* next,  MYMENUELEMENT* prev)
  : title(_title),
    next(next),
    prev(prev) {  }

const char* MYMENUELEMENT::getTitle() {
  return this->title;
}

void MYMENUELEMENT::go_prev() {
  if(prev!=NULL) menu_actual = prev;
  menu_actual->show();
}

void MYMENUELEMENT::go_next() {
  if(next!=NULL) menu_actual = next;
  menu_actual->show();
}

void MYMENUELEMENT::go_down() {
 
}

MYMENU_SELECT::MYMENU_SELECT(const char* const title, MYMENUELEMENT* next, MYMENUELEMENT* prev,MYMENUELEMENT* const down)
  : MYMENUELEMENT(title,next,prev),
  down(down) {    
}; // base type

void MYMENU_SELECT::go_down() {
  if(down!=NULL) menu_actual = down;
  menu_actual->show();
}


MYMENU_NINP::MYMENU_NINP(const uint8_t def_val, const int level, MYMENUELEMENT* next,void (*const cb_show)(void))
  : MYMENUELEMENT(NULL,next,NULL),
  value(def_val),
  cb_show(cb_show),
  level(level) {
    
  };
void MYMENU_NINP::go_down() {
  if(next!=NULL) menu_actual = next;
  menu_actual->show();
}
void MYMENU_NINP::go_next() {
  value++;
  if(value>9) value = 0;
  menu_actual->show();
}
void MYMENU_NINP::go_prev() {
  value--;
  if(value>9) value = 9;
  menu_actual->show();
}
void MYMENU_NINP::show() {
  if(cb_show!=NULL) cb_show();
}

  
MYMENU_MSG::MYMENU_MSG(const char* const title,const unsigned long delay_ms,MYMENUELEMENT* next)
  : MYMENUELEMENT(title,next,NULL),
  delay_ms(delay_ms) {
};  

MYMENU_CHECK::MYMENU_CHECK(const unsigned long delay_ms,MYMENUELEMENT* next)
  : MYMENUELEMENT("",next,NULL),
  delay_ms(delay_ms) {
    
  };
void MYMENU_MSG::go_down() {
  go_next();
}
void MYMENU_MSG::go_prev() {
  go_next();
}

void MYMENUELEMENT::show() {
  char txt[40];
  sprintf(txt, "%.11s", title);
  writeToRectangleLeft(GREEN, BLACK, 40, txt);  
}
