// - 이 예제는 라즈베리파이 피코의 듀얼 코어를 사용해서 버튼의 패턴 감지 buttonCheck()와, 감지된 패턴에 따른 동작 수행 buttonExecute()을 병렬로 수행하는 예제입니다.
// - 피코 보드에서 가능합니다. 피코에서 무사히 테스팅되었으나, 듀얼 코드를 지원하는 다른 보드에서도 이 코드가 잘 작동하는지는 보드가 없어 테스트하지 못했습니다.
// - 버튼1, 버튼2, 버튼1과 버튼2 콤보, 이렇게 3가지 패턴 감지를 저장하는 큐를 각각 따로 썼습니다.
// - 패턴 감지부에서는, 패턴을 감지하면 해당 숫자를 비교적 간단히 UniversalQueue 큐에 저장하거나,
// 필요 시 큐에 파라미터들을 함께 저장하기 위해서 EventBox 구조체와 MemoryPoolQueue를 사용합니다.
// - 버튼1의 입력 감지는 큐에 감지된 숫자만 저장하고, 버튼2, 버튼1과 버튼2 콤보 입력 감지는 파라미터를 함께 저장하는 EventBox와 MemoryPoolQueue 방식을 썼습니다.
// - 패턴에 따른 동작 수행부에서는, 큐에 들어있는 것이 있으면 꺼내서 기본 구동 함수 .doIt()이나 커스텀 구동 함수를 구동합니다.
// - EventBox와 MemoryPoolQueue 사용 시 queue.allocate()와 queue.free() 관련 프로세스 관리를 확실히 해주는 것이 필요합니다. EventBox를 꺼내 쓴 후에는 반드시 잊지않고 queue.free()를 해주어야 합니다.

// - This example uses the dual cores of the Raspberry Pi Pico to perform the button pattern detection buttonCheck() and the action execution buttonExecute() corresponding to the detected pattern in parallel.
// - It is available on the Pico board. It has been tested successfully on the Pico, but it has not been tested on other boards that support dual cores due to lack of availability.
// - Three queues are used separately to store detections for button1, button2, and button1 + button2 combo.
// - In the pattern detection part, when a pattern is detected, the detected number is simply stored in the UniversalQueue,
// - or, when parameters need to be stored together in the queue, the EventBox structure and MemoryPoolQueue are used.
// - Detection of button1 inputs stores only the detected number in the queue, while detection of button2 and button1 + button2 combo inputs uses the EventBox and MemoryPoolQueue method to store parameters together.
// - In the action execution part, if there is an item in the queue, it is taken out and executed via the built-in .doIt() function or a custom execution function.
// - When using EventBox and MemoryPoolQueue, it is necessary to reliably manage the processes related to queue.allocate() and queue.free().
// - After using an EventBox, be sure to call queue.free() without forgetting.

#include <Arduino.h>
#define xPortGetCoreID get_core_num

#include "RamjiButton.h"
#include "UniversalQueue.h"

#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////

// 비동기적 수행을 위한 구문. 아주 절대적으로 필수적이지는 않다. if(every()) 구문은 필요없으면 안써도 된다.
// 쓰이는 if(every(일련번호, 10)) {} 작업들의 종류, 즉 일련번호 개수 만큼 previousMillis, previousRecorded 크기를 맞춰준다.
static unsigned long previousMillis[] = {0,0};
static uint8_t previousRecorded[] = {0,0};
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

class SimpleClass {
public:
  int value;
  SimpleClass(int v = 0) : value(v) {}
  int getValue() const { return value; }
};

// 파라미터가 있을 때 파라미터들을 함께 저장할 이벤트 구조체
// 벡터나 클래스 객체들을 담을 수도 있다고 하는데,
// 대신 참조형이라서 당연히 그 안의 값들이 변하면 가져다 쓸 때도 변한 값을 읽게 된다고.
struct EventBox {
  int8_t action; // 이벤트로 감지한 액션 번호.

  // 선택적 파라미터
  int intParam = 0;
  float floatParam = 0.0f;
  const char* strParam = nullptr; // 포인터.
  std::string stringParam;        // std::string도 사용 가능.
  std::vector<int> dataVector; // 벡터 가능.
  SimpleClass simpleObj{0};    // 간단한 클래스 객체 가능.
};

// 큐의 최대 저장 개수. 큐에는 구조체의 포인터(주소값)만 저장된다.
// MemoryPool을 생성하는 경우, 이 개수만큼의 용량을 가진, EventBox들의 MemoryPool도 함께 생성된다.
constexpr size_t eventQueueSize = 10;
// 큐에 단순 int8_t를 저장하는 거라면 MemoryPool을 쓸 필요가 없다.
// 그냥 메모리 풀 없이 쓰는 기본 UniversalQueue를 사용하는 것이 훨씬 더 간단하고 효율적.
UniversalQueue <int8_t> button1Queue(eventQueueSize); // 버튼1 이벤트 독립 동작 큐
// UniversalQueue <EventBox*> button2Queue(eventQueueSize); // 버튼2 이벤트 독립 동작 큐
// 이렇게도 쓸 수 있지만 EventBox의 new, delete를 확실히 해줘야 한다.
// EventBox* e = new EventBox(); 해서 push. => pop 해서 다 쓰고 반드시 delete e;
// EventBox를 쓸 거라면 이렇게 미리 할당된 MemoryPool을 사용하는 방식이 조금 더 빠르고 안정적이라고 한다.
// MemoryPool을 쓰는 UniversalQueue.
MemoryPoolQueue<EventBox, eventQueueSize> button2Queue; // 버튼2 이벤트 독립 동작 큐
MemoryPoolQueue <EventBox, eventQueueSize> buttonCombo12Queue; // 버튼1, 2 콤보 이벤트 조합 동작 큐

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
TwoButtonCombo buttonCombo(button1, button2); // 두 버튼 조합키를 쓰는 경우.
// TwoButtonCombo buttonCombo(button1, button2, nullptr, -1, -1, twoButtonLongPress, twoButtonManyPress, twoButtonClick, twoButtonDoubleClick); // 두 버튼 조합키 기본 동작들을 생성자에서 등록할 수도 있다. 이러한 함수들은 buttonCombo.doIt()을 통해 수행된다.
// TwoButtonCombo buttonCombo(button1, button2, nullptr, -1, -1, twoButtonLongPress, twoButtonManyPress, twoButtonClick, twoButtonDoubleClick, twoButtonTripleClick, twoButtonQuadClick, twoButtonPentaClick, twoButtonHexaClick, twoButtonHeptaClick, twoButtonOctaClick, twoButtonNonaClick, twoButtonDecaClick);

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

  // 두 버튼 조합키 기본 동작 함수들을 등록할 수 있다. buttonCombo.doIt()을 통해 수행된다.
  buttonCombo.onLongPress = twoButtonLongPress;
  buttonCombo.onManyPress = twoButtonManyPress;
  buttonCombo.onClick = twoButtonClick;
  buttonCombo.onDoubleClick = twoButtonDoubleClick;
  // buttonCombo.onTripleClick = twoButtonTripleClick;
  // buttonCombo.onQuadClick = twoButtonQuadClick;
  // buttonCombo.onPentaClick = twoButtonPentaClick;
  // buttonCombo.onHexaClick = twoButtonHexaClick;
  // buttonCombo.onHeptaClick = twoButtonHeptaClick;
  // buttonCombo.onOctaClick = twoButtonOctaClick;
  // buttonCombo.onNonaClick = twoButtonNonaClick;
  // buttonCombo.onDecaClick = twoButtonDecaClick;
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
void buttonCheck() {
  // 버튼 각각 독립 이벤트 감지 수행.
  // 두 버튼 조합키를 안쓰는 경우 한 버튼용 함수인 button.event() + ( button.doIt() or doIt() )만 쓰면 된다.
  // if(every(0, 10)) {
  //   // event()함수를 거쳐서 받은 반환값이 NO_ACTION(0)이 아니면 어떤 동작이란 소린데.
  //   // event()함수만 거치면 그 동작이 아직 수행되기 전이다.
  //   // 반환값으로도 반환되고 그게 NO_ACTION이든 아니든 action변수에 저장돼있다.
  //   // NO_ACTION, CLICK, DOUBLECLICK, LONGPRESS, MANYPRESS 등등..
  //   event1 = button1.event(); // 버튼1의 이벤트 감지.
  //   event2 = button2.event(); // 버튼2의 이벤트 감지.
  //
  //   // 버튼1 이벤트 큐에 담기.
  //   if (event1 != NO_ACTION) {
  //     button1Queue.push(event1);
  //   }
  //
  //   // 버튼2 이벤트 큐에 담기.
  //   if (event2 != NO_ACTION) {
  //     EventBox* e = button2Queue.allocate(); // MemoryPool에 할당 후 주소값 가져옴.
  //     if (e) { // 메모리 할당 성공 시에만 수행.
  //       e->action = event2;
  //       e->intParam = 123;
  //       e->floatParam = 3.14f;
  //       e->strParam = "bt2!";
  //       e->stringParam = "bt2!";
  //       e->dataVector = {1, 2, 3, 4, 5};
  //       e->simpleObj = SimpleClass(100);
  //       if (!button2Queue.push(e)) button2Queue.free(e); // 1번만 정확하게 푸시해준다. 큐가 가득 찼다거나 해서 푸시 실패 시 메모리 해제.
  //     }
  //   }
  // }

  ////////////////////////////

  // 두 버튼 조합을 포함한 이벤트 감지 수행.
  if(every(0, 10)) {
    // 두 버튼 조합을 포함한 이벤트 감지. 두 버튼 동일한 동작으로 동시에 누르면 조합 이벤트로 감지한다.
    // 객체 내 twoButtonEventDetected[3]에 감지된 이벤트들이 저장된다. 이걸 events로 받았다.
    // twoButtonEventDetected[3] = {감지된 버튼1 이벤트, 감지된 버튼2 이벤트, 감지된 버튼1과 버튼2 조합 이벤트}
    // button1.event(), button2.event() 안쓰고 그냥 이거 하나 쓰면 된다.
    int8_t* events = buttonCombo.event(); // 전체 이벤트 감지.

    // 버튼1 이벤트 큐에 담기.
    if (events[0] != NO_ACTION) {
      button1Queue.push(events[0]);
    }
    // 버튼2 이벤트 큐에 담기.
    if (events[1] != NO_ACTION) {
      EventBox* e = button2Queue.allocate(); // MemoryPool에 할당 후 주소값 가져옴.
      if (e) { // 메모리 할당 성공 시에만 수행.
        e->action = events[1];
        e->intParam = 123;
        e->floatParam = 3.14f;
        e->strParam = "bt2!";
        e->stringParam = "bt2!";
        e->dataVector = {1, 2, 3, 4, 5};
        e->simpleObj = SimpleClass(100);
        if (!button2Queue.push(e)) button2Queue.free(e); // 1번만 정확하게 푸시해준다. 큐가 가득 찼다거나 해서 푸시 실패 시 메모리 해제.
      }
    }
    // 콤보 이벤트 큐에 담기.
    if (events[2] != NO_ACTION) {
      EventBox* e = buttonCombo12Queue.allocate(); // MemoryPool에 할당 후 주소값 가져옴.
      if (e) { // 메모리 할당 성공 시에만 수행.
        e->action = events[2];
        e->intParam = 123;
        e->floatParam = 3.14f;
        e->strParam = "combo!";
        e->stringParam = "combo!";
        e->dataVector = {1, 2, 3, 4, 5};
        e->simpleObj = SimpleClass(100);
        if (!buttonCombo12Queue.push(e)) buttonCombo12Queue.free(e); // 1번만 정확하게 푸시해준다. 큐가 가득 찼다거나 해서 푸시 실패 시 메모리 해제.
      }
    }
  }

}

void buttonExecute() {
  // 큐에 저장된 이벤트들 꺼내서 동작 수행.
  if (every(1, 10)) {
    // 큐 처리.
    // 버튼1 독립 동작 큐 처리.
    while (!button1Queue.isEmpty()) {
      int8_t e;
      if (button1Queue.pop(e)) button1.doIt(e); // doIt(e);
    };
    // 버튼2 독립 동작 큐 처리.
    while (!button2Queue.isEmpty()) {
      EventBox* e;
      if (button2Queue.pop(e)) {
        button2.doIt(e->action);
        // doIt(e->action);
        // e 다 쓰고 나면.
        button2Queue.free(e); // 반드시 1번만 해제해줘야 한다. 반드시 정확하게 해줘야 해서 pop 성공 시 수행.
      }
    };
    // 버튼 1, 2 조합 동작 큐 처리.
    while (!buttonCombo12Queue.isEmpty()) {
      EventBox* e;
      if (buttonCombo12Queue.pop(e)) {
        buttonCombo.doIt(e->action);
        // comboDoIt(e->action);
        // e 다 쓰고 나면.
        buttonCombo12Queue.free(e); // 반드시 1번만 해제해줘야 한다. 반드시 정확하게 해줘야 해서 pop 성공 시 수행.
      }
    };
  }
}

void loop() {
  buttonCheck(); // 이걸 루프 중간 중간에 여러번 호출할 수 있다.
}

void loop1() {
  buttonExecute(); // 이걸 루프 중간 중간에 여러번 호출할 수 있다.
}
