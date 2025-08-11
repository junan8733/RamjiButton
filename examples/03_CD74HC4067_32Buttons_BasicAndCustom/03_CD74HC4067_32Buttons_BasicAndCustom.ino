// - CD74HC4067은 1개당 16개의 버튼을 연결할 수 있습니다. 이 예제에서는 2개의 CD74HC4067 객체를 생성하고, 각 16개씩 총 32개의 버튼을 다룹니다.
// - 각 CD74HC4067마다 채널 0 ~ 11은 각각 단일 버튼으로 패턴을 감지하고, 채널 12 ~ 13, 14 ~ 15는 두 버튼 콤보 조합으로 패턴 입력을 감지.
// - CD74HC4067_1은 기본 동작 함수들을 지정해서 버튼 객체들과 버튼 콤보 객체의 기본 내장 .doIt() 함수로 구동하고,
// - CD74HC4067_2의 동작은 커스텀으로 doIt(), comboDoIt() 구동 함수를 통해서 구동합니다.
// - CD74HC4067 사용 시 버튼 콤보 (TwoButtonCombo) 객체를 생성할 때 이와 관련된 CD74HC4067의 정보를 넣어줘야 합니다.

// - Each CD74HC4067 can connect up to 16 buttons. In this example, two CD74HC4067 instances are created, and a total of 32 buttons are handled, 16 for each.
// - For each CD74HC4067, events are detected on channels 0–11 as individual button patterns, while channels 12–13 and 14–15 detect patterns from two-button combinations.
// - CD74HC4067_1 is configured with default action functions and operates using the built-in .doIt() functions of the button instances and button combo instances,
// - and the operation of CD74HC4067_2 is executed through the custom .doIt() and comboDoIt() execution functions.
// - When using CD74HC4067, when creating a button combo (TwoButtonCombo) instance, information related to the corresponding CD74HC4067 must be provided.

#include <Arduino.h>
#include "RamjiButton.h"

//////////////////////////////////////////////////////////////////////////////////////////////

// 비동기적 수행을 위한 구문. 아주 절대적으로 필수적이지는 않다. if(every()) 구문은 필요없으면 안써도 된다.
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

// CD74HC4067
#define cd4067_1_SIG_pin 15
#define cd4067_1_S0 11
#define cd4067_1_S1 12
#define cd4067_1_S2 13
#define cd4067_1_S3 14

#define cd4067_2_SIG_pin 22
#define cd4067_2_S0 11
#define cd4067_2_S1 12
#define cd4067_2_S2 13
#define cd4067_2_S3 14
// #define cd4067_2_S0 18
// #define cd4067_2_S1 19
// #define cd4067_2_S2 20
// #define cd4067_2_S3 21

// CD74HC4067 객체. 안적으면 INPUT_PULLUP이 기본인데, 핀 모드를 그냥 명시적으로 INPUT_PULLUP으로 설정해서 생성했다.
// 굳이 필요 없는 것 같기는 하지만 ANALOG_INPUT, ANALOG_OUTPUT 이런 것도 핀 모드로 줄 수는 있다.
// 0 ~ 15번 채널로 mux.selectChannel(채널번호)을 사용해서 채널을 선택할 수 있다.
// 채널 변경 후에는 약간의 딜레이를 주면 안정성이 올라가는 듯하고.
// mux.changeMode(OUTPUT); mux.changeMode(INPUT_PULLUP); 등등으로 그때 그때 핀 모드 변경 가능.
// 참고로 버튼 객체 생성 시에 버튼 입력을 받는 핀의 핀 모드가 자동으로 INPUT_PULLUP 이런 걸로 변경되는데.
// CD74HC4067에 버튼을 달아 쓸 때에는 필요 시 시그널 핀의 핀 모드를 CD74HC4067 객체로 컨트롤 하는 게 좋을 수 있다.
CD74HC4067 mux1(cd4067_1_S0, cd4067_1_S1, cd4067_1_S2, cd4067_1_S3, cd4067_1_SIG_pin, INPUT_PULLUP);
CD74HC4067 mux2(cd4067_2_S0, cd4067_2_S1, cd4067_2_S2, cd4067_2_S3, cd4067_2_SIG_pin, INPUT_PULLUP);

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

// CD74HC4067
Button button_4067_1[16] = {
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[0]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[1]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[2]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[3]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[4]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[5]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[6]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[7]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[8]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[9]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[10]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[11]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[12]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[13]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[14]
  Button(cd4067_1_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[15]
};

Button button_4067_2[16] = {
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[0]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[1]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[2]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[3]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[4]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[5]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[6]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[7]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[8]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[9]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[10]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[11]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[12]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[13]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[14]
  Button(cd4067_2_SIG_pin, INPUT_PULLUP, oneButtonLongPress, oneButtonManyPress, oneButtonClick, oneButtonDoubleClick), // button[15]
};

// CD74HC4067을 쓰면서 두 버튼 조합키를 쓰는 경우 이렇게 버튼 객체들 외에도 CD74HC4067 객체, CD74HC4067 객체의 두 버튼에 해당하는 채널 번호들을 던져주도록 한다.
TwoButtonCombo buttonCombo1(button_4067_1[12], button_4067_1[13], &mux1, 12, 13);
TwoButtonCombo buttonCombo2(button_4067_1[14], button_4067_1[15], &mux1, 14, 15);

TwoButtonCombo buttonCombo3(button_4067_2[12], button_4067_2[13], &mux2, 12, 13);
TwoButtonCombo buttonCombo4(button_4067_2[14], button_4067_2[15], &mux2, 14, 15);

//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

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

  buttonCombo2.onLongPress = twoButtonLongPress;
  buttonCombo2.onManyPress = twoButtonManyPress;
  buttonCombo2.onClick = twoButtonClick;
  buttonCombo2.onDoubleClick = twoButtonDoubleClick;
  // buttonCombo2.onTripleClick = twoButtonTripleClick;
  // buttonCombo2.onQuadClick = twoButtonQuadClick;
  // buttonCombo2.onPentaClick = twoButtonPentaClick;
  // buttonCombo2.onHexaClick = twoButtonHexaClick;
  // buttonCombo2.onHeptaClick = twoButtonHeptaClick;
  // buttonCombo2.onOctaClick = twoButtonOctaClick;
  // buttonCombo2.onNonaClick = twoButtonNonaClick;
  // buttonCombo2.onDecaClick = twoButtonDecaClick;

  // 버튼콤보3, 버튼콤보4 조합은 그냥 커스텀 구동 함수 쓸거라서 이런 지정이 없다.
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
// CD74HC4067로 받는 이벤트들 저장하기 위한 배열.
int8_t event_4067_1[16] = {
  NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION,
  NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION
};
int8_t event_4067_2[16] = {
  NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION,
  NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION, NO_ACTION
};

void buttonCheckAndExecute() {
  // 2개의 CD74HC4067로 각 채널들에서 이벤트를 감지하고 독립 및 조합 동작 수행.
  if(every(0, 10)){
    // CD74HC4067_1 채널 0 ~ 11은 이벤트를 감지해서 각 버튼 독립 동작 수행.
    for (int i = 0; i < 12; i++) {
      mux1.selectChannel(i);
      delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
      event_4067_1[i] = button_4067_1[i].event(); // 각 버튼 별 이벤트 감지.
    }
    for (int i = 0; i < 12; i++) {
      if (event_4067_1[i]) {
        Serial.print("button_4067_1[");
        Serial.print(i);
        Serial.print("] => ");
        button_4067_1[i].doIt(event_4067_1[i]); // 각 버튼 별 독립 동작 수행.
        // doIt(event_4067_1[i]); // 각 버튼 별 커스텀 동작 수행.
      }
    }

    // CD74HC4067_1 채널 12, 13은 이벤트를 감지해서 독립 및 조합 동작 수행.
    int8_t* events1 = buttonCombo1.event(); // 전체 이벤트 감지.

    if (events1[0]) { buttonCombo1.getBt1().doIt(events1[0]); } // 등록된 기본 동작 수행.
    if (events1[1]) { buttonCombo1.getBt2().doIt(events1[1]); } // 등록된 기본 동작 수행.
    if (events1[2]) { buttonCombo1.doIt(events1[2]); } // 등록된 기본 조합 동작 수행.

    // CD74HC4067_1 채널 14, 15도 이벤트를 감지해서 독립 및 조합 동작 수행.
    int8_t* events2 = buttonCombo2.event(); // 전체 이벤트 감지.

    if (events2[0]) { buttonCombo2.getBt1().doIt(events2[0]); } // 등록된 기본 동작 수행.
    if (events2[1]) { buttonCombo2.getBt2().doIt(events2[1]); } // 등록된 기본 동작 수행.
    if (events2[2]) { buttonCombo2.doIt(events2[2]); } // 등록된 기본 조합 동작 수행.

    // CD74HC4067_2 채널 0 ~ 11은 이벤트를 감지해서 각 버튼 독립 동작 수행.
    for (int i = 0; i < 12; i++) {
      mux2.selectChannel(i);
      delay(3); // 안정화를 위한 약간의 딜레이가 필요하다.
      event_4067_2[i] = button_4067_2[i].event(); // 각 버튼 별 이벤트 감지.
    }
    for (int i = 0; i < 12; i++) {
      if (event_4067_2[i]) {
        Serial.print("button_4067_2[");
        Serial.print(i);
        Serial.print("] => ");
        // button_4067_2[i].doIt(event_4067_2[i]); // 각 버튼 별 독립 동작 수행.
        doIt(event_4067_2[i]); // 각 버튼 별 커스텀 동작 수행.
      }
    }

    // CD74HC4067_2 채널 12, 13은 이벤트를 감지해서 독립 및 조합 동작 수행.
    int8_t* events3 = buttonCombo3.event(); // 전체 이벤트 감지.

    if (events3[0]) { doIt(events3[0]); } // 커스텀 동작 수행.
    if (events3[1]) { doIt(events3[1]); } // 커스텀 동작 수행.
    if (events3[2]) { comboDoIt(events3[2]); } // 커스텀 조합 동작 수행.

    // CD74HC4067_2 채널 14, 15도 이벤트를 감지해서 독립 및 조합 동작 수행.
    int8_t* events4 = buttonCombo4.event(); // 전체 이벤트 감지.

    if (events4[0]) { doIt(events4[0]); } // 커스텀 동작 수행.
    if (events4[1]) { doIt(events4[1]); } // 커스텀 동작 수행.
    if (events4[2]) { comboDoIt(events4[2]); } // 커스텀 조합 동작 수행.
  }
}

void loop() {
  // 메인 루프가 길어서 오래 걸리면 버튼만 따로 FreeRTOS, 멀티 코어를 쓰거나,
  buttonCheckAndExecute(); // 만일 그럴 수 없다면 이걸 메인 루프 중간 중간에 여러번 호출할 수 있다.
}
