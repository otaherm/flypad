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
    virtual bool has_own_screen();
    bool isSelected();
    void setDownCallback(void (*const cb_down)(void));
    virtual void task();
    void enter();
  private:
    const char* const title;
  protected:
    void (* cb_down)(void);
    unsigned long millis_entered;
};

class MYMENU_SELECT: public MYMENUELEMENT {
  public:
    MYMENU_SELECT(const char* const title, MYMENUELEMENT* next, MYMENUELEMENT* prev, MYMENUELEMENT* down); // base type
    void go_down() override;

    MYMENUELEMENT* const down;

};

class MYMENU_NINP: public MYMENUELEMENT {
  public:
    MYMENU_NINP(MYMENUELEMENT* next, void (*const cb_show)(void), int value_min, int value_max);
    void go_down() override;
    void go_prev() override;
    void go_next() override;
    int value;
    const int value_min, value_max;
    void (*const cb_show)(void);
    void show() override;
    int getVX() {
      return value;
    };
    bool has_own_screen() override;
    void draw(int idx, int x, int y, int width, int height, int count);
};

class MYMENU_MSG: public MYMENUELEMENT {
  public:
    MYMENU_MSG(const char* const title, const unsigned long delay_ms, void (*const cb_show)(void), MYMENUELEMENT* next);
    const unsigned long delay_ms;
    void go_down() override;
    void go_prev() override;
    bool has_own_screen() override;
    void task() override;
    void show() override;
    void (*const cb_show)(void);
};

class MYMENU_CHECK: public MYMENUELEMENT {
  public:
    MYMENU_CHECK(const unsigned long delay_ms, MYMENUELEMENT* next);
    const unsigned long delay_ms;

};


#endif
