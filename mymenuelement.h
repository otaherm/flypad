#ifndef MYMENUELEMENT_h
#define MYMENUELEMENT_h

//#include "flypad.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class MYMENUELEMENT {  
public:
    MYMENUELEMENT(const char* const title, MYMENUELEMENT* next, MYMENUELEMENT* prev); // base type
    const char* getTitle();
    MYMENUELEMENT* const next;
    MYMENUELEMENT* const prev;

    virtual void go_prev();
    virtual void go_next();
    virtual void go_down();
    virtual void show();
private:
    const char* const title;    
};

class MYMENU_SELECT: public MYMENUELEMENT {
  public:  
  MYMENU_SELECT(const char* const title, MYMENUELEMENT* next, MYMENUELEMENT* prev, MYMENUELEMENT* down); // base type
  void go_down() override;

  MYMENUELEMENT* const down;
  
};

class MYMENU_NINP: public MYMENUELEMENT {
  public: 
  MYMENU_NINP(const uint8_t def_val, const int level, MYMENUELEMENT* next, void (*const cb_show)(void));
  void go_down() override;  
  void go_prev() override;
  void go_next() override;
  uint8_t value;
  const int level;
  void (*const cb_show)(void);
  void show() override;
  int getVX() {return level*value;};
  
};

class MYMENU_MSG: public MYMENUELEMENT {
  public:
  MYMENU_MSG(const char* const title,const unsigned long delay_ms,MYMENUELEMENT* next);
  const unsigned long delay_ms;
  void go_down() override;  
  void go_prev() override;
};

class MYMENU_CHECK: public MYMENUELEMENT {
  public:  
  MYMENU_CHECK(const unsigned long delay_ms,MYMENUELEMENT* next);
  const unsigned long delay_ms;
  
};


#endif
