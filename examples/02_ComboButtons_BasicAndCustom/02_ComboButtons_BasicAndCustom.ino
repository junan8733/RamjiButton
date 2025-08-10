// - 이 예제는 두 버튼 조합 감지가 가능한 버튼 콤보 객체 2개를 각각 동작시키는 예제입니다.
// - 총 4개의 버튼을 2개, 2개씩 써서 서로 다른 버튼 콤보 객체를 2개 만든 후에, 각 콤보 객체들로 모든 이벤트들을 감지.
// - buttonCombo1은 기본 동작 함수들을 지정해서 버튼 객체들과 버튼 콤보 객체의 기본 내장 .doIt() 함수로 구동하고,
// - buttonCombo2의 동작은 커스텀으로 doIt(), comboDoIt() 구동 함수를 통해서 구동합니다.

// - This example is an example where two button combo instances that can detect two-button combinations are operated separately.
// - A total of four buttons are used, two for each, to create two different button combo instances, and all events are detected through each combo instance.
// - buttonCombo1 is configured with default action functions and operates using the built-in .doIt() functions of the button instances and the button combo instance,
// - and the operation of buttonCombo2 is executed through the custom .doIt() and comboDoIt() execution functions.

#include <Arduino.h>
#include "RamjiButton.h"

//////////////////////////////////////////////////////////////////////////////////////////////

// 비동기적 수행을 위한 구문. 아주 절대적으로 필수적이지는 않다.
// every의 whichOne 개수만큼 previousMillis, previousRecorded 크기를 맞춰준다.
static unsigned long previousMillis[] = {0};
static uint8_t previousRecorded[] = {0};
static unsigned long currentMillis = 0;
void recordPreviousMillis(uint8_t whichOne) {
  if(!previousRecorded[whichOne]) {
    previousMillis[whichOne] = millis();
    previousRecorded[whichOne] = 1;
  }
}
boolean every(uint8_t whichOne, unsigned long delayMillis) {
  recordPreviousMillis(whichOne);
  if(previousRecorded[whichOne]) {
    currentMillis = millis();
    if (currentMillis - previousMillis[whichOne] >= delayMillis) {
      previousMillis[whichOne] = currentMillis;
      previousRecorded[whichOne] = 0;
      return true; // 시간이 충분히 지남.
    }
  }
  return false; // 아직 대기 중.
}

//////////////////////////////////////////////////////////////////////////////////////////////

#define buttonPin01 16
#define buttonPin02 17
#define buttonPin03 19
#define buttonPin04 20

//////////////////////////////////////////////////////////////////////////////////////////////

// 클래스 내부의 기본적인 구동 함수에 배치되는 함수들. 함수 이름은 꼭 이렇게 안해도 됨.
// 파라미터를 받을 수 없다. 함수 내에서 전역변수 같은 건 쓸 수 있겠고.
void oneButtonLongPress() { Serial.println("one button long press."); }
void oneButtonManyPress() { Serial.println("one button many press."); }
void oneButtonClick() {  Serial.println("one button click."); }
void oneButtonDoubleClick() { Serial.println("one button double click."); }
void oneButtonTripleClick() { Serial.println("one button triple click."); }
void oneButtonQuadClick() { Serial.println("one button quad click."); }
void oneButtonPentaClick() { Serial.println("one button penta click."); }
void oneButtonHexaClick() { Serial.println("one button hexa click."); }
void oneButtonHeptaClick() { Serial.println("one button hepta click."); }
void oneButtonOctaClick() { Serial.println("one button octa click."); }
void oneButtonNonaClick() { Serial.println("one button nona click."); }
void oneButtonDecaClick() { Serial.println("one button deca click."); }

//////////////////////////////////////////////////////////////////////////////////////////////

void twoButtonLongPress() { Serial.println("two button long press."); }
void twoButtonManyPress() { Serial.println("two button many press."); }
void twoButtonClick() { Serial.println("two button click."); }
void twoButtonDoubleClick() { Serial.println("two button double click."); }
void twoButtonTripleClick() { Serial.println("two button triple click."); }
void twoButtonQuadClick() { Serial.println("two button quad click."); }
void twoButtonPentaClick() { Serial.println("two button penta click."); }
void twoButtonHexaClick() { Serial.println("two button hexa click."); }
void twoButtonHeptaClick() { Serial.println("two button hepta click."); }
void twoButtonOctaClick() { Serial.println("two button octa click."); }
void twoButtonNonaClick() { Serial.println("two button nona click."); }
void twoButtonDecaClick() { Serial.println("two button deca click."); }

//////////////////////////////////////////////////////////////////////////////////////////////

// 커스텀 구동 함수에 배치되는 함수들. 함수 이름은 꼭 이렇게 안해도 됨.
// 자유도가 높고 파라미터도 쉽게 쓸 수 있다.
void customOneButtonLongPress() { Serial.println("custom one button long press."); }
void customOneButtonManyPress() { Serial.println("custom one button many press."); }
void customOneButtonClick() { Serial.println("custom one button click."); }
void customOneButtonDoubleClick() { Serial.println("custom one button double click."); }
void customOneButtonTripleClick() { Serial.println("custom one button triple click."); }
void customOneButtonQuadClick() { Serial.println("custom one button quad click."); }
void customOneButtonPentaClick() { Serial.println("custom one button penta click."); }
void customOneButtonHexaClick() { Serial.println("custom one button hexa click."); }
void customOneButtonHeptaClick() { Serial.println("custom one button hepta click."); }
void customOneButtonOctaClick() { Serial.println("custom one button octa click."); }
void customOneButtonNonaClick() { Serial.println("custom one button nona click."); }
void customOneButtonDecaClick() { Serial.println("custom one button deca click."); }

// 커스텀 구동 함수. 버튼 두 개 조합 동작 가능. 자유도가 높고 파라미터도 쉽게 쓸 수 있다.
// 버튼 클래스 외부의 함수를 얼마든지 편하게 구동할 수 있다.
// event()함수만 거치면 그 동작이 아직 수행되기 전이다.
// NO_ACTION, CLICK, DOUBLECLICK, LONGPRESS, MANYPRESS 등등..
// 이런 반환값들을 숫자로 얻을 수 있는데.
// 이걸 실제 수행할 doIt() 같은 함수한테 던져주는 거다.
// 이러한 구동 함수에서는, 값에 따른 동작 함수를 배정해주면 된다.
// cd4067을 쓰는 경우 for문을 돌며 각 버튼들의 반환값을 받아놨다가 한번에 구동할 수도 있겠고.
// 버튼 두 개 조합키를 쓰는 경우 조합 동작으로 판정되면 그에 맞는 doIt() 함수를 따로 구동할 수 있다.
// 원하면 파라미터를 넣어서 쓰자.
void doIt(int8_t a) {
  if (a != NO_ACTION) {
    switch (a) {
      case NO_ACTION: break;
      case LONGPRESS: customOneButtonLongPress(); break;
      case MANYPRESS: customOneButtonManyPress(); break;
      case CLICK: customOneButtonClick(); break;
      case DOUBLECLICK: customOneButtonDoubleClick(); break;
        // case TRIPLECLICK: customOneButtonTripleClick(); break;
        // case QUADCLICK: customOneButtonQuadClick(); break;
        // case PENTACLICK: customOneButtonPentaClick(); break;
        // case HEXACLICK: customOneButtonHexaClick(); break;
        // case HEPTACLICK: customOneButtonHeptaClick(); break;
        // case OCTACLICK: customOneButtonOctaClick(); break;
        // case NONACLICK: customOneButtonNonaClick(); break;
        // case DECACLICK: customOneButtonDecaClick(); break;
      default: break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

void customTwoButtonLongPress() { Serial.println("custom two button long press."); }
void customTwoButtonManyPress() { Serial.println("custom two button many press."); }
void customTwoButtonClick() { Serial.println("custom two button click."); }
void customTwoButtonDoubleClick() { Serial.println("custom two button double click."); }
void customTwoButtonTripleClick() { Serial.println("custom two button triple click."); }
void customTwoButtonQuadClick() { Serial.println("custom two button quad click."); }
void customTwoButtonPentaClick() { Serial.println("custom two button penta click."); }
void customTwoButtonHexaClick() { Serial.println("custom two button hexa click."); }
void customTwoButtonHeptaClick() { Serial.println("custom two button hepta click."); }
void customTwoButtonOctaClick() { Serial.println("custom two button octa click."); }
void customTwoButtonNonaClick() { Serial.println("custom two button nona click."); }
void customTwoButtonDecaClick() { Serial.println("custom two button deca click."); }

// 커스텀으로 만든 두 버튼 조합 동작용 구동 함수.
// 두 버튼 동시에 누르는 "조합 동작"만 수행한다.
// 각 버튼들의 독립 동작 수행은 버튼마다 button.doIt()이든 doIt()이든 각각 별개의 구동 함수를 통해서 처리한다.
// 원하면 파라미터를 넣어서 쓰자.

// comboDoIt(event); 이런 식으로 쓴다.
void comboDoIt(int8_t a) {
  // Serial.println(a); // 디버깅.. 이 함수가 실행은 되었는지, 어느 액션 번호를 받았는지 확인.
  if (a != NO_ACTION) { // <= 이거 생각보다 중요할 수 있으니 지우지 않는 게 좋음. if(a)라고 해도 되지만 이게 조금 더 명시적이어서 이렇게 해놓음.
    switch (a) {
      case NO_ACTION: break;
      case LONGPRESS: customTwoButtonLongPress(); break;
      case MANYPRESS: customTwoButtonManyPress(); break;
      case CLICK: customTwoButtonClick(); break;
      case DOUBLECLICK: customTwoButtonDoubleClick(); break;
        // case TRIPLECLICK: customTwoButtonTripleClick(); break;
        // case QUADCLICK: customTwoButtonQuadClick(); break;
        // case PENTACLICK: customTwoButtonPentaClick(); break;
        // case HEXACLICK: customTwoButtonHexaClick(); break;
        // case HEPTACLICK: customTwoButtonHeptaClick(); break;
        // case OCTACLICK: customTwoButtonOctaClick(); break;
        // case NONACLICK: customTwoButtonNonaClick(); break;
        // case DECACLICK: customTwoButtonDecaClick(); break;
      default: break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

// 버튼 클래스 객체.
// 핀 모드는 따로 안적어주면 기본 INPUT_PULLUP, 버튼 누르면 뜨는 상태는 기본 LOW다.
// INPUT_PULLUP, INPUT_PULLDOWN 핀 모드를 지정해주면 버튼을 눌렀을 때 뜨는 상태가 LOW인지 HIGH인지도 자동으로 지정되는데.
// pinMode를 (INPUT)으로 했다든가 해서 혹시 원한다면 setup()이나 loop()에서 button.setLOWHIGH(LOW or HIGH);로 버튼을 눌렀을 때 뜨는 상태를 수동으로 지정해줄 수 있다.
// Button button1(buttonPin01); // 이렇게 하고 그냥 setup()에서 button1.onClick = onClick; 해도 가능하다.
// Button button2(buttonPin02); // 아니면 이렇게 하고 그냥 함수 지정 안하고 event() 함수만 쓰고 커스텀 구동 함수 쓸 수도 있다.
// Button button1(buttonPin01, INPUT_PULLDOWN); // INPUT_PULLDOWN을 써야한다면 이렇게 INPUT_PULLDOWN으로 바꾸어주면 된다.
// Button button2(buttonPin02, INPUT); // 차후에라도 바꾸려면 button.setPinMode(INPUT_PULLUP or INPUT_PULLDOWN);
// INPUT으로 했을 경우 => setup()이나 loop()에서 button2.setLOWHIGH(LOW); or button2.setLOWHIGH(HIGH); 직접 해주면 된다.
Button button1(buttonPin01, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick); // 기본 구동 함수 쓸 때1. 이렇게 기본 동작 함수를 매치해서 쓰면 button.doIt()함수를 써서 구동한다.
// Button button1(buttonPin01, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick, oneButtonTripleClick, oneButtonQuadClick, oneButtonPentaClick, oneButtonHexaClick, oneButtonHeptaClick, oneButtonOctaClick, oneButtonNonaClick, oneButtonDecaClick);
Button button2(buttonPin02, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick);
// Button button2(buttonPin02, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick, oneButtonTripleClick, oneButtonQuadClick, oneButtonPentaClick, oneButtonHexaClick, oneButtonHeptaClick, oneButtonOctaClick, oneButtonNonaClick, oneButtonDecaClick);

// 두 버튼 조합키를 쓰려는 경우 각 버튼 객체를 만들고 버튼 객체들을 던져줘서 이렇게 버튼콤보 객체를 생성한다.
TwoButtonCombo buttonCombo1(button1, button2); // 두 버튼 조합키를 쓰는 경우.
// TwoButtonCombo buttonCombo1(button1, button2, nullptr, -1, -1, twoButtonLongPress, twoButtonManyPress, twoButtonClick, twoButtonDoubleClick); // 두 버튼 조합키 기본 동작들을 생성자에서 등록할 수도 있다. 이러한 함수들은 buttonCombo1.doIt()을 통해 수행된다.
// TwoButtonCombo buttonCombo1(button1, button2, nullptr, -1, -1, twoButtonLongPress, twoButtonManyPress, twoButtonClick, twoButtonDoubleClick, twoButtonTripleClick, twoButtonQuadClick, twoButtonPentaClick, twoButtonHexaClick, twoButtonHeptaClick, twoButtonOctaClick, twoButtonNonaClick, twoButtonDecaClick);

// 커스텀 구동 함수 쓸거니깐 이렇게만 생성해주었다.
Button button3(buttonPin03, INPUT_PULLUP);
Button button4(buttonPin04, INPUT_PULLUP);
// 두 버튼 조합키를 쓸 거다. 조합 동작도 커스텀 구동 함수로 쓸 거라면 기본 동작 함수를 지정해주지 않아도 된다.
TwoButtonCombo buttonCombo2(button3, button4);

//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  // 기본 구동 함수 쓸 때2. 기본 동작 함수들 등록.
  // 생성자에서 지정 못했어도 setup이나 loop에서 이렇게 할 수 있다.
  // 지정하는 함수들이 파라미터를 받지않는 함수들이다.
  // 이렇게 기본 동작 함수를 매치해서 쓰면 button.doIt()함수를 써서 구동한다.
  // 파라미터 받는 함수들을 구동하고 싶으면 button.doIt()함수를 안쓰고,
  // 이런 지정도 하든 안하든 상관 없이.
  // main.cpp에서 doIt() 함수를 만들어서 쓰면 된다.
  button1.onLongPress = oneButtonLongPress;
  button1.onManyPress = oneButtonManyPress;
  button1.onClick = oneButtonClick;
  button1.onDoubleClick = oneButtonDoubleClick;
  // button1.onTripleClick = oneButtonTripleClick;
  // button1.onQuadClick = oneButtonQuadClick;
  // button1.onPentaClick = oneButtonPentaClick;
  // button1.onHexaClick = oneButtonHexaClick;
  // button1.onHeptaClick = oneButtonHeptaClick;
  // button1.onOctaClick = oneButtonOctaClick;
  // button1.onNonaClick = oneButtonNonaClick;
  // button1.onDecaClick = oneButtonDecaClick;

  button2.onLongPress = oneButtonLongPress;
  button2.onManyPress = oneButtonManyPress;
  button2.onClick = oneButtonClick;
  button2.onDoubleClick = oneButtonDoubleClick;
  // button2.onTripleClick = oneButtonTripleClick;
  // button2.onQuadClick = oneButtonQuadClick;
  // button2.onPentaClick = oneButtonPentaClick;
  // button2.onHexaClick = oneButtonHexaClick;
  // button2.onHeptaClick = oneButtonHeptaClick;
  // button2.onOctaClick = oneButtonOctaClick;
  // button2.onNonaClick = oneButtonNonaClick;
  // button2.onDecaClick = oneButtonDecaClick;

  // 두 버튼 조합키 기본 동작 함수들을 등록할 수 있다. buttonCombo1.doIt()을 통해 수행된다.
  buttonCombo1.onLongPress = twoButtonLongPress;
  buttonCombo1.onManyPress = twoButtonManyPress;
  buttonCombo1.onClick = twoButtonClick;
  buttonCombo1.onDoubleClick = twoButtonDoubleClick;
  // buttonCombo1.onTripleClick = twoButtonTripleClick;
  // buttonCombo1.onQuadClick = twoButtonQuadClick;
  // buttonCombo1.onPentaClick = twoButtonPentaClick;
  // buttonCombo1.onHexaClick = twoButtonHexaClick;
  // buttonCombo1.onHeptaClick = twoButtonHeptaClick;
  // buttonCombo1.onOctaClick = twoButtonOctaClick;
  // buttonCombo1.onNonaClick = twoButtonNonaClick;
  // buttonCombo1.onDecaClick = twoButtonDecaClick;

  // 버튼3, 버튼4는 그냥 커스텀 구동 함수 쓸거라서 이런 지정이 없다.
}

////////////////////////////////////////////////////////////////////////////////////////////

// RamjiButton.h에 정의된 ACTION enum이 이렇다.
// 맨 앞 NO_ACTION = 0.
// enum ACTION {
//   NO_ACTION, // 0
//   CLICK, DOUBLECLICK, TRIPLECLICK, QUADCLICK, PENTACLICK, // 1, 2, 3, 4, 5
//   HEXACLICK, HEPTACLICK, OCTACLICK, NONACLICK, DECACLICK, // 6, 7, 8, 9, 10
//   LONGPRESS, MANYPRESS, // 11, 12
//   NUMBER_OF_ACTIONS // 총 개수는 NO_ACTION(0)을 포함해서 13
// };

void buttonCheckAndExecute() {
  // 두 버튼 조합 동작 수행.
  if(every(0, 10)) {
    // 두 버튼 조합을 포함한 이벤트 감지. 두 버튼 동일한 동작으로 동시에 누르면 조합 이벤트로 감지한다.
    // 객체 내 twoButtonEventDetected[3]에 감지된 이벤트들이 저장된다. 이걸 events로 받았다.
    // twoButtonEventDetected[3] = {감지된 버튼1 이벤트, 감지된 버튼2 이벤트, 감지된 버튼1과 버튼2 조합 이벤트}
    // button1.event(), button2.event() 안쓰고 그냥 이거 하나 쓰면 된다.
    int8_t* events1 = buttonCombo1.event(); // 전체 이벤트 감지.
    int8_t* events2 = buttonCombo2.event(); // 전체 이벤트 감지.

    // 독립 동작 판정시 버튼1 독립 동작 수행.
    if(events1[0]) { buttonCombo1.getBt1().doIt(events1[0]); } // 등록된 기본 독립 동작 수행.
    // 독립 동작 판정시 버튼2 독립 동작 수행.
    if(events1[1]) { buttonCombo1.getBt2().doIt(events1[1]); } // 등록된 기본 독립 동작 수행.
    // 두 버튼 조합 동작 판정시 버튼1, 버튼2 두 버튼 조합 동작 수행.
    if(events1[2]) { buttonCombo1.doIt(events1[2]); } // 등록된 기본 조합 동작 수행.

    // 독립 동작 판정시 버튼3 독립 동작 수행.
    if(events2[0]) { doIt(events2[0]); } // 커스텀 동작 수행.
    // 독립 동작 판정시 버튼4 독립 동작 수행.
    if(events2[1]) { doIt(events2[1]); } // 커스텀 동작 수행.
    // 두 버튼 조합 동작 판정시 버튼3, 버튼4 두 버튼 조합 동작 수행.
    if(events2[2]) { comboDoIt(events2[2]); } // 커스텀 동작 수행.
  }
}

void loop() {
  // 메인 루프가 길어서 오래 걸리면 버튼만 따로 FreeRTOS, 멀티 코어를 쓰거나,
  buttonCheckAndExecute(); // 만일 그럴 수 없다면 이걸 메인 루프 중간 중간에 여러번 호출할 수 있다.
}
