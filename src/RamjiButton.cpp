#include <Arduino.h>
#include "RamjiButton.h"

//////////////////////////////////////////////////////////////////////////////////////////////

// 디버깅을 위한 함수.
// String pad(unsigned int num, int width) {
//   char buf[16]; // unsigned int 최대 자리수 + width가 10보다 클 경우를 대비한 여유 6
//   snprintf(buf, sizeof(buf), "%*u", width, num); // 오른쪽 정렬
//   return String(buf);
// }

//////////////////////////////////////////////////////////////////////////////////////////////

// Constructor. s0, s1, s2, s3, 시그널핀, 시그널핀의 핀 모드. 핀 모드 안주면 기본 INPUT_PULLUP이다.
// 굳이 필요 없는 것 같기는 하지만 ANALOG_INPUT, ANALOG_OUTPUT 이런 것도 핀 모드로 줄 수는 있다.
CD74HC4067::CD74HC4067(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t sig, uint8_t mode) {
    pin0 = p0;
    pin1 = p1;
    pin2 = p2;
    pin3 = p3;
    signalpin = sig;
    pinmode = mode;
    pinMode(pin0, OUTPUT);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);

    digitalWrite(pin0, LOW);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);

    if (pinmode != ANALOG_INPUT && pinmode != ANALOG_OUTPUT) {
      pinMode(signalpin, pinmode);
      if (pinmode == OUTPUT) digitalWrite(signalpin, LOW);
    }
    else if (pinmode == ANALOG_INPUT) {
      pinMode(signalpin, INPUT);
    }
    else if (pinmode == ANALOG_OUTPUT) {
      pinMode(signalpin, OUTPUT);
      analogWrite(signalpin, 0);
    }
}

void CD74HC4067::changeMode(uint8_t mode) {
    // 원한다면 시그널 핀의 핀모드를 변경할 수 있다.
    pinmode = mode;
    if (pinmode != ANALOG_INPUT && pinmode != ANALOG_OUTPUT) {
      pinMode(signalpin, pinmode);
      if (pinmode == OUTPUT) digitalWrite(signalpin, LOW);
    }
    else if (pinmode == ANALOG_INPUT) {
      pinMode(signalpin, INPUT);
    }
    else if (pinmode == ANALOG_OUTPUT) {
      pinMode(signalpin, OUTPUT);
    }
}

void CD74HC4067::selectChannel(uint8_t channel) {
    // C0(0) ~ C15(15)까지 채널 선택. 한번 선택하면 다시 선택할 때까지 그 상태가 유지된다.
    digitalWrite(pin0, channel & 0x01);
    digitalWrite(pin1, channel & 0x02);
    digitalWrite(pin2, channel & 0x04);
    digitalWrite(pin3, channel & 0x08);
}

int CD74HC4067::dRead() {
    // 선택한 채널로부터 디지털 읽기.
    return digitalRead(signalpin);
}

int CD74HC4067::dRead(uint8_t channel) {
    // 선택 채널 바꾸면서 디지털 읽기.
    selectChannel(channel);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    return digitalRead(signalpin);
}

int CD74HC4067::aRead() {
    // 선택한 채널로부터 아날로그 읽기.
    return analogRead(signalpin);
}

int CD74HC4067::aRead(uint8_t channel) {
    // 선택 채널 바꾸면서 아날로그 읽기.
    selectChannel(channel);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    return analogRead(signalpin);
}

void CD74HC4067::dWrite(uint8_t highlow) {
    // 선택한 채널로 디지털 출력.
    digitalWrite(signalpin, highlow);
}

void CD74HC4067::dWrite(uint8_t channel, uint8_t highlow) {
    // 선택 채널 바꾸면서 디지멸 출력.
    selectChannel(channel);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    digitalWrite(signalpin, highlow);
}

void CD74HC4067::aWrite(uint8_t duty) {
    // 선택한 채널로 아날로그 출력.
    analogWrite(signalpin, duty);
}

void CD74HC4067::aWrite(uint8_t channel, uint8_t duty) {
    // 선택 채널 바꾸면서 아날로그 출력.
    selectChannel(channel);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    analogWrite(signalpin, duty);
}

//////////////////////////////////////////////////////////////////////////////////////////////

// 지정할 함수 던져주면 지정되고 안던져주면 그냥 nullptr 배정된다.
// Button aaa(SIG_button);
// Button aaa(SIG_button, onClick, onDoubleClick, onLongPress, onManyPress);
// Button aaa = Button(SIG_button, onClick, onDoubleClick, onLongPress, onManyPress);
// 참고로 onLongPress만 지정하고 싶으면 Button button(5, nullptr, nullptr, onLongPress);
// 생성자에 넣어준 onLongPress 같은 것들은 main.cpp에서 만든 임의의 함수.
// 생성자에서 지정 안했어도 setup()이나 loop()에서 차후에 이렇게 지정할 수도 있다.
// button.onClick = onClick; // 오른쪽 onClick은 main.cpp에서 만든 임의의 함수.
// button.onDoubleClick = onDoubleClick;
// button.onLongPress = onLongPress;
// button.onManyPress = onManyPress;
Button::Button(uint8_t pin
                , uint8_t pinModeValue
                , void (*onLongPress)()
                , void (*onManyPress)()
                , void (*onClick)()
                , void (*onDoubleClick)()
                , void (*onTripleClick)()
                , void (*onQuadClick)()
                , void (*onPentaClick)()
                , void (*onHexaClick)()
                , void (*onHeptaClick)()
                , void (*onOctaClick)()
                , void (*onNonaClick)()
                , void (*onDecaClick)()
                )
    : pin(pin), pinModeValue(pinModeValue), pressed(false)
      , onLongPress(onLongPress)
      , onManyPress(onManyPress)
      , onClick(onClick)
      , onDoubleClick(onDoubleClick)
      , onTripleClick(onTripleClick)
      , onQuadClick(onQuadClick)
      , onPentaClick(onPentaClick)
      , onHexaClick(onHexaClick)
      , onHeptaClick(onHeptaClick)
      , onOctaClick(onOctaClick)
      , onNonaClick(onNonaClick)
      , onDecaClick(onDecaClick)
{
  pinMode(pin, pinModeValue);
  if (pinModeValue == INPUT_PULLUP) LOWHIGH = LOW;
  else if (pinModeValue == INPUT_PULLDOWN) LOWHIGH = HIGH;
}

void Button::update() {
  pressed = (digitalRead(pin) == LOWHIGH);
}

void Button::debugPrint() {
  Serial.print("pin:" + String(pin)+" ");
  Serial.print("upTime-downTime:" + String(upTime-downTime)+" ");
  Serial.print("now-downTime:" + String(now-downTime)+" ");
  Serial.print("now-shortCallTime:" + String(now-shortCallTime)+" ");
  Serial.print("now-longLogicTime:" + String(now-longLogicTime)+" ");
  Serial.print("now-actionTime[MANYPRESS]:" + String(now-actionTime[MANYPRESS])+" ");
  Serial.println("clickCount:" + String(actionClickCount)+" ");
}

void Button::longPress() { if (onLongPress) onLongPress(); }
void Button::manyPress() { if (onManyPress) onManyPress(); }
void Button::click() { if (onClick) onClick(); }
void Button::doubleClick() { if (onDoubleClick) onDoubleClick(); }
void Button::tripleClick() { if (onTripleClick) onTripleClick(); }
void Button::quadClick() { if (onQuadClick) onQuadClick(); }
void Button::pentaClick() { if (onPentaClick) onPentaClick(); }
void Button::hexaClick() { if (onHexaClick) onHexaClick(); }
void Button:: heptaClick() { if (onHeptaClick) onHeptaClick(); }
void Button::octaClick() { if (onOctaClick) onOctaClick(); }
void Button::nonaClick() { if (onNonaClick) onNonaClick(); }
void Button::decaClick() { if (onDecaClick) onDecaClick(); }

// 이벤트 감지 함수.
// 버튼의 동작 판정 시 그 동작 번호를.
// 동작 판정이 없을 시 action = NO_ACTION(0)을 리턴.
// 더 정확히는 동작 판정 시 action에 그걸 저장하고, action값을 리턴.
int8_t Button::event() {
    bool actionGet = false; // 동작 판정이 있는지 체킹.
    now = millis(); // 현재 시점을 계속 체킹.

  // 디바운싱 체킹.
    // 디바운싱 상태인지 체크해서 시간이 지나면 해제. 해제해야 "manyPress()" 같은 함수들이 doIt()에서 실행된다.
    if (debounceActive && now - lastActionTime >= debounceInterval) {
      debounceActive = false;
    }

  // 빠른 동작의 경우 디바운싱이 해제가 안되어 동작 실행이 안되는 경우를 위한 디버깅..
    // Serial.print("debounceActive:" + String(debounceActive)+" ");
    // Serial.println("now-lastActionTime ("+String(pin)+"):" + String(now-lastActionTime)+" ");

  // 버튼을 새로이 누르거나 뗐는지, 갱신이 있는지 체킹하기 위한 것.
    pre_downTime = downTime; // 버튼 다운, 업 시간이 변화되는지를 계속 체킹.
    pre_upTime = upTime;

  // 버튼 상태 체킹.
    // 버튼 다운, 업 발생 시 실시간으로 알아차리며 그 시간과 상태를 체킹한다.
    if(!pressed && digitalRead(pin) == LOWHIGH) {
      downTime = now;
      pressed = Pressed;
    } else if(pressed && digitalRead(pin) == !LOWHIGH) {
      upTime = now;
      pressed = Released;
    }

  // 쇼트, 롱 로직 선정부.
    // ShortState 로직으로 들어갈지 LongState 로직으로 들어갈지 판단한다.
    // 버튼 업 시간이 갱신되었고, 다운->업 시간이 짧다면,
    if(pre_upTime!=upTime && upTime - downTime <= LONG_PRESS_TIME) { // 버튼이 콕 눌렸다 떼어질 때마다,
      shortCallTime = upTime; // 시간 체킹 하고.
      clickCount++; // 클릭 카운트 올리고.
      state = intoShortStateLogic; // ShortState 로직으로 들어간다.
    }
    // 그렇지 않은 수행들 중에서.
    // LongState 로직이 아니고, 버튼이 눌려져 있고, 이전 버튼 다운 시간에서 오래 지났다면.
    else if(state!=intoLongStateLogic && pressed == Pressed && now - downTime > LONG_PRESS_TIME) { // 버튼이 꾸욱 눌리고 있으면 LongState 로직으로 들어가고.
      longLogicTime = now; // 시간 체킹하고.
      state = intoLongStateLogic; // LongState 로직으로 들어간다.
    }

  // 선정된 로직에 따른 최종 액션 판정부.
    switch (state) {
    case noneState:
      break;
    case intoShortStateLogic:
      // 현재 시점과 최근 짧게 누름 시간의 차이가 재누름 시간 한도보다 초과했다면 액션을 수행한다.
      // 아직 재누름 시간 초과 안했으면 아무것도 안한다.
      if(now-shortCallTime > SHORT_REPRESS_TIME) {
        actionGet = true; // 밑에 것들 중 하나는 반드시 판정이 되므로 일단 동작 판정이 있다고 체킹.
        switch (clickCount) {
        case 1: action = CLICK; break;
        case 2: action = DOUBLECLICK; break;
        case 3: action = TRIPLECLICK; break;
        case 4: action = QUADCLICK; break;
        case 5: action = PENTACLICK; break;
        case 6: action = HEXACLICK; break;
        case 7: action = HEPTACLICK; break;
        case 8: action = OCTACLICK; break;
        case 9: action = NONACLICK; break;
        default: action = DECACLICK; break;
        }
        actionClickCount = clickCount;
        clickCount = 0; // 스테이터스 초기화.
        state = noneState;
      }
      break;
    case intoLongStateLogic:
      // 버튼이 눌려있고, 연속 누름이 활성화되었다면,
      if(pressed && manyTriggered) {
        if(now-actionTime[MANYPRESS] >= MANY_REPRESS_TIME) { // 연속 누름 시간이 되면 계속 수행.
          actionGet = true;
          action = MANYPRESS;
        }
      }
      // 버튼이 눌려있고, 현재 재누름 시간이 지났다면,
      else if(pressed && now-longLogicTime >= MANY_TRIGGER_TIME) {
        manyTriggered = true; // 연속 누름 활성화.
        actionGet = true;
        action = MANYPRESS;
      }
      // 그 전에 버튼이 떼어졌다면,
      else if(!pressed) {
        if(manyTriggered==0) { // 연속 누름이 활성화돼있지 않은 상태라면,
          actionGet = true;
          action = LONGPRESS;
        }

        state = noneState; // 상태 초기화.
        manyTriggered = false;
      }
      actionClickCount = 0; // 이전에 연속 클릭에서 기록된 게 롱키 로직으로 넘어오면 남아있어서.
      break;
    default:
      break;
    }
  // 감지된 게 있고, 디바운싱이 해제돼있는 상태라면,
  if (actionGet && !debounceActive) { // NO_ACTION일 때에는 디바운싱이 체킹되지 않도록 한다.
    actionTime[action] = now; // 시간 체킹하기.
    debounceActive = true;
    lastActionTime = now; // 현재 시간 저장
  }
  // 감지된 게 있지만, 디바운싱이 활성화돼있는 상태이거나,
  // 감지된 게 없으면, action은 NO_ACTION이 된다.
  else action = NO_ACTION;
  return action; // action을 반환
}

// 판정이 끝난 action 수행부. 한 버튼 용.
// event()함수를 거쳐서 받은 반환값이 NO_ACTION(0)이 아니면 어떤 동작이란 소린데.
// event()함수만 거치면 그 동작이 아직 수행되기 전이다.
// 반환값으로도 반환되고 그게 NO_ACTION이든 아니든 action변수에 저장돼있다.
// 무튼 그 반환값을 받아서 실제 수행을 하는 함수.
//
// action 값을 받았을 때 이 함수와 비슷하게 클래스 외부에서 만들면,
// 외부 임의의 함수를 구동할 수 있다. 파라미터 받는 함수라든가. 그런 거 얼마든지 편하게 구동.
// cd4067을 쓰는 경우 클래스 외부에서 버튼들의 action값을 받아놨다가 한번에 구동할 수도 있겠고.
// 버튼 두 개 조합키를 쓰는 경우 조합 동작으로 판정되면 그에 맞는 doIt 함수를 따로 구동할 수 있다.
// 비슷하게 만든다는 게 a가 NO_ACTION인 경우를 배제하고, 값에 따른 동작 함수를 배정한다는 것이다.
void Button::doIt(int8_t a) {
  // Serial.println(a); // 디버깅.. 이 함수가 실행은 되었는지, 어느 액션 번호를 받았는지 확인.
  // if(a != NO_ACTION) debugPrint(); // 디버깅..
  if (a != NO_ACTION) {
    switch (a) {
      case NO_ACTION: break;
      case LONGPRESS: longPress(); break;
      case MANYPRESS: manyPress(); break;
      case CLICK: click(); break;
      case DOUBLECLICK: doubleClick(); break;
      case TRIPLECLICK: tripleClick(); break;
      case QUADCLICK: quadClick(); break;
      case PENTACLICK: pentaClick(); break;
      case HEXACLICK: hexaClick(); break;
      case HEPTACLICK: heptaClick(); break;
      case OCTACLICK: octaClick(); break;
      case NONACLICK: nonaClick(); break;
      case DECACLICK: decaClick(); break;
      default: break;
    }
  }
}

// pin
// uint8_t Button::getPin() { return pin; }
// void Button::setPin(uint8_t p) { pin = p; }
// // pinModeValue
// uint8_t Button::getPinMode() { return pinModeValue; }
void Button::setPinMode(uint8_t v) {
  pinModeValue = v;
  pinMode(pin, pinModeValue);
  if (pinModeValue == INPUT_PULLUP) LOWHIGH = LOW;
  else if (pinModeValue == INPUT_PULLDOWN) LOWHIGH = HIGH;
}
// LOWHIGH
uint8_t Button::getLOWHIGH() { return LOWHIGH; }
void Button::setLOWHIGH(uint8_t lh) { LOWHIGH = lh; }
// // state
// uint8_t Button::getState() { return state; }
// void Button::setState(uint8_t s) { state = s; }
// // clickCount
// uint8_t Button::getClickCount() { return clickCount; }
// void Button::setClickCount(uint8_t count) { clickCount = count; }
// // exClickCount
// uint8_t Button::getActionClickCount() { return actionClickCount; }
// void Button::setActionClickCount(uint8_t count) { actionClickCount = count; }
// // action
// int8_t Button::getAction() { return action; }
// void Button::setAction(int8_t a) { action = a; }
// // now
// unsigned long Button::getNow() { return now; }
// void Button::setNow(unsigned long n) { now = n; }
// // downTime
// unsigned long Button::getDownTime() { return downTime; }
// void Button::setDownTime(unsigned long t) { downTime = t; }
// // upTime
// unsigned long Button::getUpTime() { return upTime; }
// void Button::setUpTime(unsigned long t) { upTime = t; }
// // pre_downTime
// unsigned long Button::getPreDownTime() { return pre_downTime; }
// void Button::setPreDownTime(unsigned long t) { pre_downTime = t; }
// // pre_upTime
// unsigned long Button::getPreUpTime() { return pre_upTime; }
// void Button::setPreUpTime(unsigned long t) { pre_upTime = t; }
// // shortCallTime
// unsigned long Button::getShortCallTime() { return shortCallTime; }
// void Button::setShortCallTime(unsigned long t) { shortCallTime = t; }
// // longLogicTime
// unsigned long Button::getLongLogicTime() { return longLogicTime; }
// void Button::setLongLogicTime(unsigned long t) { longLogicTime = t; }
// actionTime
unsigned long Button::getActionTime(int8_t i) { return actionTime[i]; }
// void Button::setActionTime(int8_t i, unsigned long t) { actionTime[i] = t; }
// // pressed
// bool Button::isPressed() { return pressed; }
// void Button::setPressed(bool p) { pressed = p; }
// // manyTriggered
// bool Button::isManyTriggered() { return manyTriggered; }
// void Button::setManyTriggered(bool m) { manyTriggered = m; }
// lastActionTime
// unsigned long Button::getLastActionTime() { return lastActionTime; }
void Button::setLastActionTime(unsigned long t) { lastActionTime = t; }
// debounceActive
bool Button::isDebounceActive() { return debounceActive; }
void Button::setDebounceActive(bool active) { debounceActive = active; }

//////////////////////////////////////////////////////////////////////////////////////////////

// twoButtonCombo 관련.

// TwoButtonCombo buttonCombo(button1, button2); // 두 버튼 조합키를 쓰는 경우.
// TwoButtonCombo buttonCombo1(button[12], button[13], &mux, 12, 13); // CD74HC4067의 채널을 이용해서 두 버튼 조합키를 쓰는 경우.
// TwoButtonCombo buttonCombo2(button[14], button[15], &mux, 14, 15);
TwoButtonCombo::TwoButtonCombo(Button &button1, Button &button2,
                               CD74HC4067* mux, int8_t ch1, int8_t ch2
                               , void (*onLongPress)()
                               , void (*onManyPress)()
                               , void (*onClick)()
                               , void (*onDoubleClick)()
                               , void (*onTripleClick)()
                               , void (*onQuadClick)()
                               , void (*onPentaClick)()
                               , void (*onHexaClick)()
                               , void (*onHeptaClick)()
                               , void (*onOctaClick)()
                               , void (*onNonaClick)()
                               , void (*onDecaClick)()
                               )
  : bt1(button1), bt2(button2), cd4067(mux),
    cd4067_channel1(ch1), cd4067_channel2(ch2),
    waitForOtherButton(false), waitStartTime(0), twoButtonManyPressTime(0),
    currentEvent1(NO_ACTION), currentEvent2(NO_ACTION),
    actionSaved1(NO_ACTION), actionSaved2(NO_ACTION),
    pre_actionSaved1(NO_ACTION), pre_actionSaved2(NO_ACTION),
    combinationWork(NOT_DECIDED)
    , onLongPress(onLongPress)
    , onManyPress(onManyPress)
    , onClick(onClick)
    , onDoubleClick(onDoubleClick)
    , onTripleClick(onTripleClick)
    , onQuadClick(onQuadClick)
    , onPentaClick(onPentaClick)
    , onHexaClick(onHexaClick)
    , onHeptaClick(onHeptaClick)
    , onOctaClick(onOctaClick)
    , onNonaClick(onNonaClick)
    , onDecaClick(onDecaClick)
{
  // event()는 선언 시점에 초기화됨.
}

void TwoButtonCombo::longPress() { if (onLongPress) onLongPress(); }
void TwoButtonCombo::manyPress() { if (onManyPress) onManyPress(); }
void TwoButtonCombo::click() { if (onClick) onClick(); }
void TwoButtonCombo::doubleClick() { if (onDoubleClick) onDoubleClick(); }
void TwoButtonCombo::tripleClick() { if (onTripleClick) onTripleClick(); }
void TwoButtonCombo::quadClick() { if (onQuadClick) onQuadClick(); }
void TwoButtonCombo::pentaClick() { if (onPentaClick) onPentaClick(); }
void TwoButtonCombo::hexaClick() { if (onHexaClick) onHexaClick(); }
void TwoButtonCombo:: heptaClick() { if (onHeptaClick) onHeptaClick(); }
void TwoButtonCombo::octaClick() { if (onOctaClick) onOctaClick(); }
void TwoButtonCombo::nonaClick() { if (onNonaClick) onNonaClick(); }
void TwoButtonCombo::decaClick() { if (onDecaClick) onDecaClick(); }

// 두 버튼 조합을 포함한 입력 감지 함수.
// 매 수행시마다 초기화된 twoButtonEventDetected[]에 감지된 이벤트들을 기록한다.
// 버튼 두 개 조합키를 쓰는 경우 두 버튼들의 action값이 같을 때 조건에 따라서 조합 action 판정을 할 수 있다.
// 여러 버튼이라도 각각 동작하면 되고 두 버튼 조합 동작이 필요 없다면,
// 각각의 버튼 객체를 만들어서.
// 입력 감지로 그냥 각 버튼들의 button.event() 함수를 쓰면 된다.
int8_t* TwoButtonCombo::event() {
  // twoButtonEventDetected[] 배열의 초기화.
  resetTwoButtonEventDetected();
  // event()함수를 거쳐서 받은 반환값이 NO_ACTION(0)이 아니면 어떤 동작이란 소린데.
  // event()함수만 거치면 그 동작이 아직 수행되기 전이다.
  // 반환값으로도 반환되고 그게 NO_ACTION이든 아니든 action변수에 저장돼있다.
  // NO_ACTION, CLICK, DOUBLECLICK, LONGPRESS, MANYPRESS 등등..
  if (cd4067!=nullptr) {
    cd4067->selectChannel(cd4067_channel1);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    currentEvent1 = bt1.event(); // 버튼1의 이벤트 감지.
    cd4067->selectChannel(cd4067_channel2);
    delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
    currentEvent2 = bt2.event(); // 버튼2의 이벤트 감지.
  } else {
    currentEvent1 = bt1.event(); // 버튼1의 이벤트 감지.
    currentEvent2 = bt2.event(); // 버튼2의 이벤트 감지.
  }
  // 여기 now라고 해서 시간 체킹을 해서 쓰는데.
  // event() 함수 내에서 실은 이벤트 발생 시간을 기록하고 있다.
  // 그래서 그 시간이 이 now보다 아주 미세하게 빠를 수 있는데.
  // 이 now가 이렇게 event() 함수 호출 이후에 있는 게,
  // now - bt1.getActionTime(LONGPRESS) 이런 것들을 계산할 때에 도움이 된다.
  // 만일 now가 event()보다 앞에 있으면,
  // 의미 있는 이벤트 발생 시 now가 이벤트 발생 시간과 거의 차이는 없지만 아주 조금 더 작게 되어.
  // now - bt1.getActionTime(LONGPRESS) 이런 것들이 42억xxxx. 이런 엄청 큰 숫자로 나오게 될 수 있다.
  // 만약에 그래도 코드가 돌고 작동을 하는 데에 지장이 없다면 뭐 상관 없지만.
  // 그래도 이렇게 두는 것이 계산을 하는 데에 있어서, 더 예측가능하고 안정적이다.
  // 이 now를 event() 앞에 두지 말 것.
  unsigned long now = millis();

  // currentEvent1이 뭔가 들어온다면, 그걸 잠시 받아놓고.
  // TWO_BUTTON_TOLLERANCE_TIME 동안은 가만히 currentEvent2가 그거랑 동일한 게 들어오는지를 본다.
  // currentEvent2가 동일한 게 들어오면 조합 실행.
  // currentEvent2가 동일한 게 안들어오면 둘 다 실행.
  // 안들어오면 그냥 실행.
  // 실행 후에는 action 초기화.
  // 1 0   0 1
  // 0 0   0 0
  // 1 0   0 1
  // 0 1   1 0
  // 1 0   0 1
  // 0 2   2 0

  // event는 NO_ACTION이 아닌 것들이 순간적으로 들어왔다가 다시 NO_ACTION이 들어오는 경우가 많으므로.
  // 두 버튼의 연계를 위해서 NO_ACTION(0) 아닌 것이 들어왔을 때에 따로 저장을 해준다.
  if(currentEvent1) actionSaved1 = currentEvent1;
  if(currentEvent2) actionSaved2 = currentEvent2;

  ///// 한 버튼에 액션이 들어온 순간 일단 대기하기 위한 시간 컨트롤.

  // 둘 중 한 버튼에 액션이 들어왔고, 다른 버튼은 NO_ACTION이라면,
  // waitForOtherButton이 활성화된다.
  // waitForOtherButton이 활성화 될 때에 그 시간이 waitStartTime으로 저장된다.
  // 원래는 기본적으로 생각해보자면 if문 조건 구성을 pre_actionSaved 말고 event로 해야하지만,
  // actionSaved도 액션이 판정되고 나면 초기화되므로 actionSaved를 썼다.
  // manyPress 시 0 4 0 4 event값은 이런 식으로 0 아닌 값이 간헐적으로 계속 들어오고 말고를 반복하게 되는데.
  // 이로 인해 waitForOtherButton이 계속 활성화되게 되어.
  // manyPress 연속 동작이 수행되지 않는다.
  // 이를 방지하고자 조건문에서 event 말고 actionSaved를 썼다.
  // actionSaved는 0 4 4 이런 식으로 처음 manyPress가 들어오면 그 0 아닌 값이 유지가 돼서.
  // waitForOtherButton이 처음 한번만 활성화되게 된다.
  // 그래서 첫 manyPress 시에만 다른 버튼 event를 기다리고,
  // 그 대기 시간이 지나면 manyPress 연속 동작이 수행되게 된다.
  if(((pre_actionSaved1 == NO_ACTION && actionSaved1 != NO_ACTION) && actionSaved2 == NO_ACTION) ||
    ((pre_actionSaved2 == NO_ACTION && actionSaved2 != NO_ACTION) && actionSaved1 == NO_ACTION)) {
    waitForOtherButton = true;
    waitStartTime = now;
  }
  // TWO_BUTTON_TOLLERANCE_TIME이 지나면 waitForOtherButton을 끈다.
  if(now-waitStartTime > TWO_BUTTON_TOLLERANCE_TIME) {
    waitForOtherButton = false;
    waitStartTime = 0;
  }

  // 디버깅 part1.
  // int width = 2;
  // Serial.print(pad(currentEvent1, width)+" ");
  // Serial.print(pad(currentEvent2, width)+" ");
  // Serial.print("// ");
  // Serial.print(pad(actionSaved1, width)+" ");
  // Serial.print(pad(actionSaved2, width)+" ");
  // Serial.print("now-b1(M): " + pad(now - bt1.getActionTime(MANYPRESS), 10)+" ");
  // Serial.print("now-b2(M): " + pad(now - bt2.getActionTime(MANYPRESS), 10)+" ");

  ///// 조합 동작인지 독립 동작인지 일단 대기인지, combinationWork 결정부.

  // 버튼1에 뭔가 들어왔고 버튼2에 아무것도 없을 때. (0 아닌 x) 0
  if(actionSaved1&&!actionSaved2) {
    // 현재 시간과 최근 들어온 액션의 차이가 TWO_BUTTON_TOLLERANCE_TIME보다 크면, 독립 수행.
    if(!waitForOtherButton) combinationWork = NO_COMBINATION;
    // 한 버튼 manyPress시 첫 번째 combinationWork 판정이 NO_COMBINATION(1)이면.
    // (manyPress가 하나 들어왔을 때 대기가 시작되고.)
    // (그 첫 번째 대기가 풀릴 때까지 다른 버튼 manyPress가 안넘어오면)
    // 이후 그 다음 대기가 없도록 combinationWork을 NOT_DECIDED가 아닌 NO_COMBINATION으로 고정한다.
    else if(actionSaved1 == MANYPRESS && combinationWork == NO_COMBINATION) combinationWork = NO_COMBINATION;
    else combinationWork = NOT_DECIDED; // 그렇지 않으면 결정 유보.
  }
  // 버튼1에 아무것도 없고 버튼2에 뭔가 들어왔을 때. 0 (0 아닌 x)
  else if(!actionSaved1&&actionSaved2) {
    // 현재 시간과 최근 들어온 액션의 차이가 TWO_BUTTON_TOLLERANCE_TIME보다 크면, 독립 수행.
    if(!waitForOtherButton) combinationWork = NO_COMBINATION;
    else if(actionSaved2 == MANYPRESS && combinationWork == NO_COMBINATION) combinationWork = NO_COMBINATION;
    else combinationWork = NOT_DECIDED; // 그렇지 않으면 결정 유보.
  }
  // 두 버튼에 액션이 둘 다 들어오게 되는 순간. (0 아닌 x) (0 아닌 y)
  else if(actionSaved1&&actionSaved2) {
    // 그 둘이 같은 액션이 들어왔으면, 조합 수행. x x
    if(actionSaved1 == actionSaved2) combinationWork = YES_COMBINATION;
    // 다른 액션이 들어왔으면, 독립 수행. x !x
    else if(actionSaved1 != actionSaved2) combinationWork = NO_COMBINATION;
  }

  // 디버깅 part2.
  // Serial.print(String(waitForOtherButton ? "\twaiting " : "\tnotWaiting "));
  // Serial.print("\tcombinationWork: " + pad(combinationWork, width)+" ");

  ///// 결정된 combinationWork를 통한 동작 판정부.

  // 조합 액션 판정.
  if(combinationWork == YES_COMBINATION) {
    twoButtonEventDetected[2] = actionSaved1; // 버튼들의 조합 액션을 twoButtonEventDetected[2]에 저장.
    if(actionSaved1==MANYPRESS) twoButtonManyPressTime = now; // 두 버튼 manyPress 작동 시 시간을 저장.
    // 액션 들어오는 것 체킹 초기화.
    actionSaved1 = NO_ACTION;
    actionSaved2 = NO_ACTION;
  }

  // 독립 액션 판정.
  else if(combinationWork == NO_COMBINATION) {
    // 버튼1.
    // 두 버튼 manyPress 후 한 버튼 manyPress나 한 버튼 클릭을 억제.
    if((actionSaved1==MANYPRESS || actionSaved1==CLICK) &&
      now-twoButtonManyPressTime < ACTION_SUPPRESS_TIME+TWO_BUTTON_TOLLERANCE_TIME) {}
    // 그렇지 않으면 수행한다.
    else{
      twoButtonEventDetected[0] = actionSaved1; // 버튼1의 액션을 twoButtonEventDetected[0]에 저장.
    }
    // 억제하는 기작이 효율적으로 작동하도록 버튼1과 버튼2를 따로 한다.
    // 버튼2.
    // 두 버튼 manyPress 후 한 버튼 manyPress나 한 버튼 클릭을 억제.
    if((actionSaved2==MANYPRESS || actionSaved2==CLICK) &&
      now-twoButtonManyPressTime < ACTION_SUPPRESS_TIME+TWO_BUTTON_TOLLERANCE_TIME){}
    else{
      twoButtonEventDetected[1] = actionSaved2; // 버튼2의 액션을 twoButtonEventDetected[1]에 저장.
    }

    // 액션 들어오는 것 체킹 초기화.
    actionSaved1 = NO_ACTION;
    actionSaved2 = NO_ACTION;
  }

  // 디버깅 part3.
  // Serial.println("now-b1b2(M): " + pad(now-twoButtonManyPressTime, 10)+" ");

  // 한 버튼 manyPress 후 이 시간이 지나면 combinationWork이 NOT_DECIDED로 초기화된다.
  // 이걸 안하면 한 버튼 manyPress 후 combinationWork이 계속 NO_COMBINATION(1)로 고정돼있어서
  // 그 다음에 manyPress아 아닌 다른 액션이 오게 되면 다시 대기가 시작되거나 하는 등 정상적이지만.
  // 만일 두 버튼 manyPress가 오게되면 약간 오동작이 난다.
  // 두 버튼 manyPress여야 하는 상황에서 두 버튼 독립 수행이 튀어나오게 되고..
  // 무튼 한 버튼 manyPress가 끝난 후 일정 시간이 지나면 고정을 풀어준다.
  // 이 시간은 혹시 모르니 manyPress 반복 시간보다 길어야겠다.
  if(combinationWork == NO_COMBINATION && now - bt1.getActionTime(MANYPRESS) > COMBINATION_INITIALIZE_TIME &&
    now - bt2.getActionTime(MANYPRESS) > COMBINATION_INITIALIZE_TIME) combinationWork = NOT_DECIDED;

  // event 들어오는 것의 변화를 감지하기 위한 저장. 두 버튼 시 필요하다.
  pre_actionSaved1 = actionSaved1;
  pre_actionSaved2 = actionSaved2;

  return twoButtonEventDetected;
}

// 커스텀으로 만든 두 버튼 조합 동작용 구동 함수.
// 두 버튼 동시에 누르는 "조합 동작"만 수행한다.
// 각 버튼들의 독립 동작 수행은 버튼마다 button.doIt()이든 doIt()이든 각각 별개의 구동 함수를 통해서 처리한다.
// 원하면 파라미터를 넣어서 쓰자.
void TwoButtonCombo::doIt(int8_t a) {
  // Serial.println(a); // 디버깅.. 이 함수가 실행은 되었는지, 어느 액션 번호를 받았는지 확인.
  if (a != NO_ACTION) {
    switch (a) {
      case NO_ACTION: break;
      case LONGPRESS: longPress(); break;
      case MANYPRESS: manyPress(); break;
      case CLICK: click(); break;
      case DOUBLECLICK: doubleClick(); break;
      case TRIPLECLICK: tripleClick(); break;
      case QUADCLICK: quadClick(); break;
      case PENTACLICK: pentaClick(); break;
      case HEXACLICK: hexaClick(); break;
      case HEPTACLICK: heptaClick(); break;
      case OCTACLICK: octaClick(); break;
      case NONACLICK: nonaClick(); break;
      case DECACLICK: decaClick(); break;
      default: break;
    }
  }
}

Button& TwoButtonCombo::getBt1() { return bt1; }
void TwoButtonCombo::setBt1(Button& b) { bt1 = b; }
Button& TwoButtonCombo::getBt2() { return bt2; }
void TwoButtonCombo::setBt2(Button& b) { bt2 = b; }
CD74HC4067* TwoButtonCombo::getCD4067() { return cd4067; }
void TwoButtonCombo::setCD4067(CD74HC4067* mux) { cd4067 = mux; }
int8_t TwoButtonCombo::getCD4067_channel1() { return cd4067_channel1; }
void TwoButtonCombo::setCD4067_channel1(int8_t ch) { cd4067_channel1 = ch; }
int8_t TwoButtonCombo::getCD4067_channel2() { return cd4067_channel2; }
void TwoButtonCombo::setCD4067_channel2(int8_t ch) { cd4067_channel2 = ch; }

int8_t* TwoButtonCombo::getTwoButtonEventDetected() { return twoButtonEventDetected; }
int8_t TwoButtonCombo::getTwoButtonEventDetected(int index) {
  return (index >= 0 && index < 3) ? twoButtonEventDetected[index] : NO_ACTION;
}
void TwoButtonCombo::setTwoButtonEventDetected(int index, int8_t value) {
  if (index >= 0 && index < 3) twoButtonEventDetected[index] = value;
}
void TwoButtonCombo::resetTwoButtonEventDetected() {
  for (int8_t &val : twoButtonEventDetected) val = NO_ACTION;
}