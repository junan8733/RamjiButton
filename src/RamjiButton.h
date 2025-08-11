#ifndef RAMJIBUTTON_H
#define RAMJIBUTTON_H

#include <Arduino.h>

#define Pressed true
#define Released false
#define noneState 0
#define intoShortStateLogic 1
#define intoLongStateLogic 2
// 한 버튼 액션이 들어온 후 이 시간 안에 다른 버튼의 액션이 판정돼서 들어오면 조합키로 판정되도록..
// 이 시간만큼 판정이 대기. 기다리게 된다. 이 시간이 지나면 독립 수행으로 판정된다.
// 두 버튼 manyPress후 버튼에서 손을 떼는 타이밍이 조금 늦었을 때 한 버튼 manyPress가 뜰 수 있는데 그걸 방지하는 데에도 이 시간이 관여한다.
#define TWO_BUTTON_TOLLERANCE_TIME 300
// 한 버튼 manyPress 후 combinationWork이 NO_COMBINATION으로 고정되는데 이 시간이 지나면 combinationWork이 NOT_DECIDED(0)으로 초기화된다.
// 오동작 방지에 필요하다.
#define COMBINATION_INITIALIZE_TIME 400
// 두 버튼 manyPress후 버튼에서 손을 떼는 타이밍이 조금 늦었을 때 한 버튼 manyPress나 한 버튼 클릭이 뜰 수 있는데,
// 두 버튼 manyPress후 (이 시간 + TWO_BUTTON_TOLLERANCE_TIME) 이내의 한 버튼 manyPress나 한 버튼 클릭 동작이 억제된다.
// 최소한 ACTION_SUPPRESS_TIME이 MANY_REPRESS_TIME 보다 크게 하는데, 260이나 이 정도 이상으로 크게 해놓는 게 좋다.
#define ACTION_SUPPRESS_TIME 450
#define SHORT_REPRESS_TIME 300 // 이 시간 안에 누르면 연속 클릭으로 인정. 이거보다 짧으면 연속 클릭 인식이 잘 안되기 시작한다.
#define LONG_PRESS_TIME 500 // long press 판정에 걸리는 시간.
#define MANY_TRIGGER_TIME 500 // (long press 판정 + 이 시간)이 지나면 연속 누름. 그 전에 떼면 길게 누름.
// manyPress 반복 시간. 연속 누름 동작 판정되면 이후 되풀이에 걸리는 시간. 하지만 체크하는 루프가 이거보다 늦게 돌 경우 당연히 그 속도로 수행되게 될 거다.
// 버튼 누르고 있으면 다다다다다!! 수행되는데 이걸 크게 하면 반복이 느려지고, 작게 하면 빨라진다.
#define MANY_REPRESS_TIME 20
// debounce 대기 시간. manyPress 반복 시간 MANY_REPRESS_TIME보다 약간 작게 해주는 것이 포인트!! => 1.0.3에서 상관 없게 되었다.
// 만일 manyPress 반복시간을 짧게 해주었는데 이상하게 그만큼 빠르게 동작이 수행되지 않는다면,
// 디바운싱이 manyPress가 그렇게 빨리 실행되지 않게 중간 중간에 주기적으로 막고 있는 것일 수 있다.
// 이럴 경우 debounceInterval을 10씩 줄여주면서 살펴본다. 100이었는데 70으로 고쳤더니 해결됐다. 혹시 모르니 60으로 해놓았다.
// 여차하면 50으로 해도 되겠다. 다만 그럴수록 버튼을 눌렀다 뗄 떼 빠르고 똑소리 나게 떼야한다.
inline unsigned long debounceInterval = 100; // manyPress 반복 시간 MANY_REPRESS_TIME보다 약간 작게.
// => 1.0.3에서 MANYPRESS 시 디바운싱을 하지 않도록 변경함으로써 MANY_REPRESS_TIME과 debounceInterval이 상관이 없게 되었다.

// 맨 앞 NO_ACTION = 0.
enum ACTION {
  NO_ACTION, // 0
  CLICK, DOUBLECLICK, TRIPLECLICK, QUADCLICK, PENTACLICK, // 1, 2, 3, 4, 5
  HEXACLICK, HEPTACLICK, OCTACLICK, NONACLICK, DECACLICK, // 6, 7, 8, 9, 10
  LONGPRESS, MANYPRESS, // 11, 12
  NUMBER_OF_ACTIONS // 총 개수는 NO_ACTION(0)을 포함해서 13
};

//////////////////////////////////////////////////////////////////////////////////////////////

#define ANALOG_INPUT 99
#define ANALOG_OUTPUT 100

class CD74HC4067 {
public:
    CD74HC4067(uint8_t sig, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t mode = INPUT_PULLUP);
    void changeMode(uint8_t mode);
    void selectChannel(uint8_t channel);
    int dRead();
    int dRead(uint8_t channel);
    int aRead();
    int aRead(uint8_t channel);
    void dWrite(uint8_t highlow);
    void dWrite(uint8_t channel, uint8_t highlow);
    void aWrite(uint8_t duty);
    void aWrite(uint8_t channel, uint8_t duty);

protected:
    uint8_t pin0;
    uint8_t pin1;
    uint8_t pin2;
    uint8_t pin3;
    uint8_t signalpin;
    uint8_t pinmode;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class Button {
public:
    Button(uint8_t pin
           , uint8_t pinModeValue = INPUT_PULLUP
           , void (*onLongPress)() = nullptr
           , void (*onManyPress)() = nullptr
           , void (*onClick)() = nullptr
           , void (*onDoubleClick)() = nullptr
           , void (*onTripleClick)() = nullptr
           , void (*onQuadClick)() = nullptr
           , void (*onPentaClick)() = nullptr
           , void (*onHexaClick)() = nullptr
           , void (*onHeptaClick)() = nullptr
           , void (*onOctaClick)() = nullptr
           , void (*onNonaClick)() = nullptr
           , void (*onDecaClick)() = nullptr
    );

    void debugPrint();

    void longPress();
    void manyPress();
    void click();
    void doubleClick();
    void tripleClick();
    void quadClick();
    void pentaClick();
    void hexaClick();
    void heptaClick();
    void octaClick();
    void nonaClick();
    void decaClick();
    void (*onLongPress)();
    void (*onManyPress)();
    void (*onClick)(); // 함수 포인터
    void (*onDoubleClick)();
    void (*onTripleClick)();
    void (*onQuadClick)();
    void (*onPentaClick)();
    void (*onHexaClick)();
    void (*onHeptaClick)();
    void (*onOctaClick)();
    void (*onNonaClick)();
    void (*onDecaClick)();

    void update();
    int8_t event();
    void doIt(int8_t a);
    // // pin
    // uint8_t getPin();
    // void setPin(uint8_t p);
    // // pinModeValue
    // uint8_t getPinMode();
    void setPinMode(uint8_t v);
    // LOWHIGH
    uint8_t getLOWHIGH();
    void setLOWHIGH(uint8_t lh);
    // // state
    // uint8_t getState();
    // void setState(uint8_t s);
    // // clickCount
    // uint8_t getClickCount();
    // void setClickCount(uint8_t count);
    // // exClickCount
    // uint8_t getActionClickCount();
    // void setActionClickCount(uint8_t count);
    // // action
    // int8_t getAction();
    // void setAction(int8_t a);
    // // now
    // unsigned long getNow();
    // void setNow(unsigned long n);
    // // downTime
    // unsigned long getDownTime();
    // void setDownTime(unsigned long t);
    // // upTime
    // unsigned long getUpTime();
    // void setUpTime(unsigned long t);
    // // pre_downTime
    // unsigned long getPreDownTime();
    // void setPreDownTime(unsigned long t);
    // // pre_upTime
    // unsigned long getPreUpTime();
    // void setPreUpTime(unsigned long t);
    // // shortCallTime
    // unsigned long getShortCallTime();
    // void setShortCallTime(unsigned long t);
    // // longLogicTime
    // unsigned long getLongLogicTime();
    // void setLongLogicTime(unsigned long t);
    // actionTime
    unsigned long getActionTime(int8_t i);
    // void setActionTime(int8_t i, unsigned long t);
    // // pressed
    // bool isPressed();
    // void setPressed(bool p);
    // // manyTriggered
    // bool isManyTriggered();
    // void setManyTriggered(bool m);
    // lastActionTime
    // unsigned long getLastActionTime();
    void setLastActionTime(unsigned long t);
    // debounceActive
    bool isDebounceActive();
    void setDebounceActive(bool active);

private:
    uint8_t pin; // 버튼이 연결된 핀 번호.
    uint8_t pinModeValue; // INPUT_PULLUP, INPUT_PULLDOWN.
    uint8_t LOWHIGH = LOW; // 버튼을 눌렀을 때 뜨는 상태.
    uint8_t state = noneState; // 최종 액션 판정부의 상태.
    uint8_t clickCount = 0; // 단순히 연속 클릭수를 체킹하기 위한 변수.
    uint8_t actionClickCount = 0; // 액션 번호가 리턴될 때 함께 기록되는 연속 클릭 수.
    int8_t action = NO_ACTION; // event 함수를 통해 리턴되는 액션 번호. 단순 저장용에 가까운데, 판정값을 event 함수를 끝까지 수행한 후 리턴하기 위해서 만들었다.
    unsigned long now = 0;
    unsigned long downTime = 0;
    unsigned long upTime = 0;
    unsigned long pre_downTime = 0;
    unsigned long pre_upTime = 0;
    unsigned long shortCallTime = 0; // 쇼트키 로직으로 들어간 시간, 클릭이 발생한 최근 시간.
    unsigned long longLogicTime = 0; // 롱키 로직으로 들어간 시간.
    unsigned long actionTime[NUMBER_OF_ACTIONS] = {0}; // 이벤트 판정이 완료된 최근 시간. enum ACTION과 개수와 인덱스가 같다.
    bool pressed = Released;
    bool manyTriggered = false; // 두 번째 manyPress 이벤트를 빠르게 구동하기 위한 bool값.
    unsigned long lastActionTime = 0; // 디바운싱을 위한 변수들.
    bool debounceActive = false;
};

//////////////////////////////////////////////////////////////////////////////////////////////

// 두 버튼 조합을 포함한 입력 감지 함수.
// void twoButtonEvent(Button &bt1, Button &bt2, int8_t (&arr)[3], CD74HC4067* cd4067 = nullptr, int8_t cd4067_channel1 = 0, int8_t cd4067_channel2 = 0);
enum COMBINATION { NOT_DECIDED, NO_COMBINATION, YES_COMBINATION }; // 0, 1, 2
class TwoButtonCombo {
public:
    TwoButtonCombo(Button &button1, Button &button2,
                   CD74HC4067* mux = nullptr, int8_t ch1 = -1, int8_t ch2 = -1
                   , void (*onLongPress)() = nullptr
                   , void (*onManyPress)() = nullptr
                   , void (*onClick)() = nullptr
                   , void (*onDoubleClick)() = nullptr
                   , void (*onTripleClick)() = nullptr
                   , void (*onQuadClick)() = nullptr
                   , void (*onPentaClick)() = nullptr
                   , void (*onHexaClick)() = nullptr
                   , void (*onHeptaClick)() = nullptr
                   , void (*onOctaClick)() = nullptr
                   , void (*onNonaClick)() = nullptr
                   , void (*onDecaClick)() = nullptr
                   );

    // 이벤트를 감지하고 내부 배열을 갱신하는 함수. 그 배열을 반환한다.
    int8_t* event();
    void doIt(int8_t a);

    void longPress();
    void manyPress();
    void click();
    void doubleClick();
    void tripleClick();
    void quadClick();
    void pentaClick();
    void hexaClick();
    void heptaClick();
    void octaClick();
    void nonaClick();
    void decaClick();
    void (*onLongPress)();
    void (*onManyPress)();
    void (*onClick)(); // 함수 포인터
    void (*onDoubleClick)();
    void (*onTripleClick)();
    void (*onQuadClick)();
    void (*onPentaClick)();
    void (*onHexaClick)();
    void (*onHeptaClick)();
    void (*onOctaClick)();
    void (*onNonaClick)();
    void (*onDecaClick)();

    Button& getBt1();
    void setBt1(Button& b);
    Button& getBt2();
    void setBt2(Button& b);
    CD74HC4067* getCD4067();
    void setCD4067(CD74HC4067* mux);
    int8_t getCD4067_channel1();
    void setCD4067_channel1(int8_t ch);
    int8_t getCD4067_channel2();
    void setCD4067_channel2(int8_t ch);

    int8_t* getTwoButtonEventDetected();
    int8_t getTwoButtonEventDetected(int index);
    void setTwoButtonEventDetected(int index, int8_t value);
    void resetTwoButtonEventDetected();

private:
    Button &bt1;
    Button &bt2;
    CD74HC4067* cd4067;
    int8_t cd4067_channel1;
    int8_t cd4067_channel2;

    bool waitForOtherButton = false;
    unsigned long waitStartTime = 0;
    unsigned long twoButtonManyPressTime = 0;

    int8_t currentEvent1 = NO_ACTION;
    int8_t currentEvent2 = NO_ACTION;
    int8_t actionSaved1 = NO_ACTION;
    int8_t actionSaved2 = NO_ACTION;
    int8_t pre_actionSaved1 = NO_ACTION;
    int8_t pre_actionSaved2 = NO_ACTION;
    uint8_t combinationWork = NOT_DECIDED;

    int8_t twoButtonEventDetected[3] = { NO_ACTION, NO_ACTION, NO_ACTION };
};

#endif //RAMJIBUTTON_H
