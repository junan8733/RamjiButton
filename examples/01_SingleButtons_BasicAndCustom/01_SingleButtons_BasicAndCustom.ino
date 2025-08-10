// - 이 예제는 버튼 2개를 조합 없이 각각 독립적으로 동작시키는 예제입니다.
// - 버튼의 객체 button1, button2를 생성한 후에, 각각 이벤트를 감지.
// - button1은 기본 동작 함수들을 지정해서 button1 객체의 기본 내장 .doIt() 함수로 구동하고,
// - button2의 동작은 커스텀으로 doIt() 구동 함수를 통해서 구동합니다.

// - This example is an example where two buttons operate independently without combination.
// - After creating the button instances button1 and button2, events are detected for each.
// - button1 is configured with default action functions and operates using the built-in .doIt() function of the button1 instance,
// - and the operation of button2 is executed through a custom .doIt() execution function.

#include <Arduino.h>
#include "RamjiButton.h"

//////////////////////////////////////////////////////////////////////////////////////////////

// 비동기적 수행을 위한 구문. 아주 절대적으로 필수적이지는 않다.
// 쓰이는 if(every(일련번호, 10)) {} 작업들의 종류, 즉 일련번호 개수 만큼 previousMillis, previousRecorded 크기를 맞춰준다.
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

// 버튼 클래스 객체.
// 핀 모드는 따로 안적어주면 기본 INPUT_PULLUP, 버튼 누르면 뜨는 상태는 기본 LOW다.
// INPUT_PULLUP, INPUT_PULLDOWN 핀 모드를 지정해주면 버튼을 눌렀을 때 뜨는 상태가 LOW인지 HIGH인지도 자동으로 지정되는데.
// pinMode를 (INPUT)으로 했다든가 해서 혹시 원한다면 setup()이나 loop()에서 button.setLOWHIGH(LOW or HIGH);로 버튼을 눌렀을 때 뜨는 상태를 수동으로 지정해줄 수 있다.
// Button button1(buttonPin01); // 이렇게 하고 그냥 setup()에서 button1.onClick = onClick; 해도 가능하다.
// Button button2(buttonPin02); // 아니면 이렇게 하고 그냥 함수 지정 안하고 event() 함수만 쓰고 커스텀 구동 함수 쓸 수도 있다.
Button button2(buttonPin02, INPUT_PULLUP);
// Button button1(buttonPin01, INPUT_PULLDOWN); // INPUT_PULLDOWN을 써야한다면 이렇게 INPUT_PULLDOWN으로 바꾸어주면 된다.
// Button button2(buttonPin02, INPUT); // 차후에라도 바꾸려면 button.setPinMode(INPUT_PULLUP or INPUT_PULLDOWN);
// INPUT으로 했을 경우 => setup()이나 loop()에서 button2.setLOWHIGH(LOW); or button2.setLOWHIGH(HIGH); 직접 해주면 된다.
Button button1(buttonPin01, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick); // 기본 구동 함수 쓸 때1. 이렇게 기본 동작 함수를 매치해서 쓰면 button.doIt()함수를 써서 구동한다.
// Button button1(buttonPin01, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick, oneButtonTripleClick, oneButtonQuadClick, oneButtonPentaClick, oneButtonHexaClick, oneButtonHeptaClick, oneButtonOctaClick, oneButtonNonaClick, oneButtonDecaClick);
// Button button2(buttonPin02, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick);
// Button button2(buttonPin02, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick, oneButtonTripleClick, oneButtonQuadClick, oneButtonPentaClick, oneButtonHexaClick, oneButtonHeptaClick, oneButtonOctaClick, oneButtonNonaClick, oneButtonDecaClick);

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

  // button2.onLongPress = oneButtonLongPress;
  // button2.onManyPress = oneButtonManyPress;
  // button2.onClick = oneButtonClick;
  // button2.onDoubleClick = oneButtonDoubleClick;
  // button2.onTripleClick = oneButtonTripleClick;
  // button2.onQuadClick = oneButtonQuadClick;
  // button2.onPentaClick = oneButtonPentaClick;
  // button2.onHexaClick = oneButtonHexaClick;
  // button2.onHeptaClick = oneButtonHeptaClick;
  // button2.onOctaClick = oneButtonOctaClick;
  // button2.onNonaClick = oneButtonNonaClick;
  // button2.onDecaClick = oneButtonDecaClick;
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
// 버튼1, 버튼2의 이벤트를 저장하기 위한 변수.
int8_t event1 = NO_ACTION, event2 = NO_ACTION;

void buttonCheckAndExecute() {
  // 버튼 각각 독립 동작 수행.
  // 두 버튼 조합키를 안쓰는 경우 한 버튼용 함수인 button.event() + ( button.doIt() or doIt() )만 쓰면 된다.
  if(every(0, 10)) {
    // event()함수를 거쳐서 받은 반환값이 NO_ACTION(0)이 아니면 어떤 동작이란 소린데.
    // event()함수만 거치면 그 동작이 아직 수행되기 전이다.
    // 반환값으로도 반환되고 그게 NO_ACTION이든 아니든 action변수에 저장돼있다.
    // NO_ACTION, CLICK, DOUBLECLICK, LONGPRESS, MANYPRESS 등등..
    event1 = button1.event(); // 버튼1의 이벤트 감지.
    event2 = button2.event(); // 버튼2의 이벤트 감지.

    if(event1) { button1.doIt(event1); } // 버튼1의 이벤트에 따른 등록된 기본 동작 수행.
    if(event2) { doIt(event2); } // 버튼2의 이벤트에 따른 커스텀 동작 수행.
  }
}

void loop() {
  // 메인 루프가 길어서 오래 걸리면 버튼만 따로 FreeRTOS, 멀티 코어를 쓰거나,
  buttonCheckAndExecute(); // 만일 그럴 수 없다면 이걸 메인 루프 중간 중간에 여러번 호출할 수 있다.
}
