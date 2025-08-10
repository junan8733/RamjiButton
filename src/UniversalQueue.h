#ifndef UNIVERSALQUEUE_H
#define UNIVERSALQUEUE_H

// 함수	설명
// push()	데이터 삽입
// pop()	데이터 꺼내기
// isEmpty()	큐가 비어 있으면 true
// isFull()	큐가 가득 차 있으면 true
// size()	현재 큐에 들어 있는 아이템 개수
// capacity()	큐의 최대 저장 용량(생성자 인자 값)
//
// #define USE_FREERTOS   // FreeRTOS 환경일 때 이렇게 정의해준다.
// // #define ARDUINO_ARCH_RP2040  // 피코 투 코어 쓸 때. RP2040 환경일 경우 정의돼 있고, 이렇게 명시적으로 해주어도 좋겠고.
//
// #include "UniversalQueue.h"
//
// // 예1) 정수 큐, 최대 10개 저장
// UniversalQueue<int> intQueue(10);
// if (!intQueue.isInitialized()) {
//     Serial.println("큐 초기화 실패!");
//     return;
// }
//
// // 생성자 인자(capacity): 큐가 저장할 수 있는 최대 아이템 개수입니다.
// // 내부 초기화 실패 여부는 isInitialized()로 확인할 수 있습니다.
//
// int value = 42;
// bool ok = intQueue.push(value, 100);
// // 큐가 꽉 찼으면 timeout_ms = 100ms 동안 공간이 생기길 기다림 (FreeRTOS 환경에서만 동작!!)
// // 투 코어 RP2040 환경에서는 안기다린다. timeout_ms == 0 → 즉시, 아니면 blocking 모드
// // 기다리게 코드 만들 수 있지만 그만큼 cpu를 소모한다고.
// if (!ok) {
//     Serial.println("Push 실패: 큐가 가득 찼거나 타임아웃");
// }
//
// int out;
// bool got = intQueue.pop(out, 50);
// // 최대 50ms 동안 데이터가 들어오길 기다림 (FreeRTOS 환경에서만 동작!!)
// if (got) {
//     Serial.print("Pop 성공, 값 = ");
//     Serial.println(out);
// } else {
//     Serial.println("Pop 실패: 큐가 비었거나 타임아웃");
// }
//
// Serial.print("비어 있나요? ");    Serial.println(intQueue.isEmpty());
// Serial.print("가득 찼나요? ");  Serial.println(intQueue.isFull());
// Serial.print("현재 크기: ");     Serial.println(intQueue.size());
// Serial.print("최대 용량: ");     Serial.println(intQueue.capacity());
//
// // 예2) 포인터 큐, MyData* 타입을 최대 5개 저장
// UniversalQueue<MyData*> ptrQueue(5);
//
// // ptrQueue 예시
// MyData* p = new MyData(...); // "new"로 생성해서 포인터로 가리키게 만든 경우. new.
// ptrQueue.push(p);
//
// // 꺼낸 후 반드시 받은 포인터를 delete해주어야 한다. 반드시!!
// // 그리고 이게 이중으로 delete되지 않는 것도 중요해서 반드시 1번만 대신 확실하게 해줘야 한다고.
// MyData* r;
// if (ptrQueue.pop(r)) {
//     // r 사용...
//     delete r;
// }
// 그리고 포인터, 참조형 데이터를 담으면 그 대상이 소멸되지 않는지 유의한다.

//////////////////////////////////////////////////////////////////////////////////////////////

// UniversalQueue / MemoryPool / MemoryPoolQueue
// ESP32(Arduino core / ESP-IDF) 자동 지원 포함 버전
//
// 주요 사용법.
// - ESP32에서는 자동으로 FreeRTOS 분기가 활성화됨(따로 define 안 해도 됨).
// - RP2040(ARDUINO_ARCH_RP2040) 분기도 유지됨.
// - ISR에서 사용하려면 FreeRTOS 분기에서 제공하는 pushFromISR/popFromISR 사용 권장.
//
// 주의:
// - UniversalQueue는 trivially_copyable 타입만 지원합니다(static_assert).
// - MemoryPool의 allocate/free 짝은 반드시 지켜야 합니다.
// - RP2040의 blocking queue API는 timeout_ms != 0 인 경우 '무한 대기'가 되므로 동작 차이가 있음.
// - ISR에서 메모리풀 alloc 호출은 안전하지 않습니다(일반적으로 사용 금지).

#include <cstdint>
#include <cstddef>
#include <type_traits>

//
// 플랫폼 자동 감지
//
#if !defined(USE_FREERTOS)
  // Arduino-ESP32 core, ESP-IDF(ESP_PLATFORM) 등에서는 FreeRTOS가 기본으로 제공됩니다.
  #if defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM) || defined(ESP32)
    #define USE_FREERTOS
  #endif
#endif

#if defined(USE_FREERTOS)
  // ESP-IDF 및 Arduino-ESP32 에서는 freertos/ 경로를 사용합니다.
  #if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32)
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"
    #include "freertos/semphr.h"
    #include "freertos/task.h"
  #else
    // 다른 FreeRTOS SDK (예: 일부 RTOS 포팅)에서는 이 경로를 쓸 수 있습니다.
    #include "FreeRTOS.h"
    #include "queue.h"
    #include "semphr.h"
    #include "task.h"
  #endif
#elif defined(ARDUINO_ARCH_RP2040)
  // Raspberry Pi Pico (pico-sdk) 분기
  #include "pico/util/queue.h"
  #include "pico/mutex.h"
  #include "pico/stdlib.h"
#endif

/**
 * UniversalQueue<T>: FreeRTOS 또는 RP2040 듀얼코어에서 안전하게 사용할 수 있는 템플릿 큐
 * FreeRTOS의 xQueue 또는 RP2040의 queue_t를 래핑하여 사용.
 * - trivially copyable 타입만 허용(포인터 타입 포함)
 */
template <typename T>
class UniversalQueue {
  static_assert(std::is_trivially_copyable<T>::value,
                "UniversalQueue supports only trivially copyable types.");

 public:
  explicit UniversalQueue(size_t capacity)
    : _capacity(capacity), _initialized(false)
#if defined(USE_FREERTOS)
    , _queue(nullptr)
#endif
  {
#if defined(USE_FREERTOS)
    _queue = xQueueCreate(static_cast<UBaseType_t>(_capacity), sizeof(T));
    _initialized = (_queue != nullptr);
#elif defined(ARDUINO_ARCH_RP2040)
    queue_init(&_queue, sizeof(T), static_cast<int>(_capacity));
    _initialized = true;
#else
    // No RTOS / unsupported platform: 초기화 실패로 처리.
    _initialized = false;
#endif
  }

  ~UniversalQueue() {
#if defined(USE_FREERTOS)
    if (_queue) {
      vQueueDelete(_queue);
      _queue = nullptr;
    }
#endif
  }

  // non-copyable
  UniversalQueue(const UniversalQueue&) = delete;
  UniversalQueue& operator=(const UniversalQueue&) = delete;

  bool isInitialized() { return _initialized; }

  /**
   * push(item, timeout_ms)
   * - FreeRTOS: timeout_ms 밀리초만큼 대기 (timeout_ms == 0 -> 즉시)
   * - RP2040: timeout_ms == 0 -> try, timeout_ms != 0 -> blocking (무한대기)
   */
  bool push(T& item, uint32_t timeout_ms = 0) {
#if defined(USE_FREERTOS)
    if (!_queue) return false;
    TickType_t ticks = (timeout_ms == 0) ? 0 : pdMS_TO_TICKS(timeout_ms);
    return xQueueSend(_queue, &item, ticks) == pdPASS;
#elif defined(ARDUINO_ARCH_RP2040)
    if (timeout_ms == 0) {
        return queue_try_add(&_queue, &item);  // 반환형 bool
    } else {
      // Pico SDK는 timeout 기능이 없음 -> blocking (무한 대기)
      queue_add_blocking(&_queue, &item);      // 반환형 void
      return true;
    }
#else
    (void)item; (void)timeout_ms;
    return false;
#endif
  }

  /**
   * pop(item, timeout_ms)
   * - FreeRTOS: timeout_ms 밀리초만큼 대기 (timeout_ms == 0 -> 즉시)
   * - RP2040: timeout_ms == 0 -> try, timeout_ms != 0 -> blocking(무한대기)
   */
  bool pop(T& item, uint32_t timeout_ms = 0) {
#if defined(USE_FREERTOS)
    if (!_queue) return false;
    TickType_t ticks = (timeout_ms == 0) ? 0 : pdMS_TO_TICKS(timeout_ms);
    return xQueueReceive(_queue, &item, ticks) == pdPASS;
#elif defined(ARDUINO_ARCH_RP2040)
    if (timeout_ms == 0) {
        return queue_try_remove(&_queue, &item); // bool 반환
    } else {
        queue_remove_blocking(&_queue, &item);   // void 반환
        return true;                             // 성공했다고 가정
    }
#else
    (void)item; (void)timeout_ms;
    return false;
#endif
  }

#if defined(USE_FREERTOS)
  /**
   * ISR-safe variants (FreeRTOS 전용)
   * - ISR에서 사용해야 하는 경우에는 이 함수를 사용하세요.
   * - pxHigherPriorityTaskWoken 포인터를 넣으면, 호출 후 wakeup 플래그를 얻을 수 있음.
   */
  bool pushFromISR(T& item, BaseType_t* pxHigherPriorityTaskWoken = nullptr) {
    if (!_queue) return false;
    BaseType_t xHigher = pdFALSE;
    BaseType_t res = xQueueSendFromISR(_queue, &item, &xHigher);
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = xHigher;
    return (res == pdTRUE) || (res == pdPASS);
  }

  bool popFromISR(T& item, BaseType_t* pxHigherPriorityTaskWoken = nullptr) {
    if (!_queue) return false;
    BaseType_t xHigher = pdFALSE;
    BaseType_t res = xQueueReceiveFromISR(_queue, &item, &xHigher);
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = xHigher;
    return (res == pdTRUE) || (res == pdPASS);
  }
#endif

  bool isEmpty() {
#if defined(USE_FREERTOS)
    if (!_queue) return true;
    return uxQueueMessagesWaiting(_queue) == 0;
#elif defined(ARDUINO_ARCH_RP2040)
    return queue_get_level(&_queue) == 0;
#else
    return true;
#endif
  }

  bool isFull() {
#if defined(USE_FREERTOS)
    if (!_queue) return false;
    return uxQueueSpacesAvailable(_queue) == 0;
#elif defined(ARDUINO_ARCH_RP2040)
    return queue_get_level(&_queue) >= static_cast<int>(_capacity);
#else
    return false;
#endif
  }

  size_t size() {
#if defined(USE_FREERTOS)
    if (!_queue) return 0;
    return static_cast<size_t>(uxQueueMessagesWaiting(_queue));
#elif defined(ARDUINO_ARCH_RP2040)
    return static_cast<size_t>(queue_get_level(&_queue));
#else
    return 0;
#endif
  }

  size_t capacity() { return _capacity; }

 private:
  size_t _capacity;
  bool _initialized;

#if defined(USE_FREERTOS)
  QueueHandle_t _queue;
#elif defined(ARDUINO_ARCH_RP2040)
  queue_t _queue;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////
// MemoryPool
// - 고정 크기 배열 기반 메모리 풀
// - Thread-safe: FreeRTOS -> semaphore, RP2040 -> mutex
// - 주의: ISR에서 alloc() 호출하지 마세요(대부분 안전하지 않음)
//////////////////////////////////////////////////////////////////////////////////////////////

#if defined(USE_FREERTOS)
  // FreeRTOS에서 SemaphoreHandle_t를 사용합니다.
#elif defined(ARDUINO_ARCH_RP2040)
  // pico mutex 사용
#endif

template <typename T, size_t PoolSize>
class MemoryPool {
public:
    MemoryPool() {
        for (size_t i = 0; i < PoolSize; ++i) {
            _freeFlags[i] = true;
        }
#if defined(USE_FREERTOS)
        _mutex = xSemaphoreCreateMutex();
#elif defined(ARDUINO_ARCH_RP2040)
        mutex_init(&_mutex);
#endif
    }

    ~MemoryPool() {
#if defined(USE_FREERTOS)
        if (_mutex) vSemaphoreDelete(_mutex);
#endif
    }

    // non-copyable
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // 할당 (nullptr 반환 가능)
    T* alloc() {
        lock();
        for (size_t i = 0; i < PoolSize; ++i) {
            if (_freeFlags[i]) {
                _freeFlags[i] = false;
                unlock();
                return &_pool[i];
            }
        }
        unlock();
        return nullptr;
    }

    // 반환
    void free(T* ptr) {
        if (!ptr) return;
        lock();
        size_t index = static_cast<size_t>(ptr - &_pool[0]);
        if (index < PoolSize) {
            _freeFlags[index] = true;
        }
        unlock();
    }

    bool isInPool(T* ptr) {
        return ptr >= &_pool[0] && ptr < (&_pool[0] + PoolSize);
    }

    size_t available() {
        size_t count = 0;
        lock();
        for (size_t i = 0; i < PoolSize; ++i) {
            if (_freeFlags[i]) ++count;
        }
        unlock();
        return count;
    }

private:
    T _pool[PoolSize];
    bool _freeFlags[PoolSize];

#if defined(USE_FREERTOS)
    SemaphoreHandle_t _mutex = nullptr;
    void lock() { if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY); }
    void unlock() { if (_mutex) xSemaphoreGive(_mutex); }
#elif defined(ARDUINO_ARCH_RP2040)
    mutex_t _mutex;
    void lock() { mutex_enter_blocking(&_mutex); }
    void unlock() { mutex_exit(&_mutex); }
#else
    void lock() {}
    void unlock() {}
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////

// 미리 할당된 메모리 풀을 사용하는 UniversalQueue 큐.

// - 메모리 안정성 보장 내용
//
// 메모리 할당과 해제를 모두 메모리 풀에서 엄격하게 관리하며,
// 멀티스레드 환경에서 충돌 없이 안전하게 동작하도록 락을 적용해 메모리 관련 에러 발생 가능성을 최소화
//
// 1. 메모리 풀에서만 할당/해제
// allocate()가 성공해서 반환된 포인터만 큐에 넣을 수 있습니다.
// push()에 nullptr를 넣으면 무조건 실패하도록 막아두었습니다.
//
// 2. 큐에 들어간 포인터만 꺼내서 사용
// pop()은 큐에 정상적으로 들어간 포인터만 반환하므로,
// 임의 포인터가 섞일 일이 없습니다.
//
// 3. 큐가 가득 찬 경우 push() 실패 처리
// push()가 실패하면 allocate()로 받은 메모리를 즉시 free()해줍니다.
// 이중 할당 누수 방지합니다.
//
// 4. free() 함수는 반드시 메모리 풀 내부에서 관리하는 포인터에만 작동
// MemoryPool 클래스 내부에서 인덱스 검사로 유효성 검사합니다.
// 잘못된 포인터가 들어올 경우 무시하므로, 오작동을 방지합니다.
//
// 5. 멀티스레드 동기화 지원
// FreeRTOS 환경에서는 뮤텍스(Semaphore)를 사용해 동기화 처리합니다.
// RP2040 듀얼코어 환경에서는 mutex로 동기화합니다.
// 락 덕분에 동시 접근으로 인한 메모리 충돌, 손상 가능성도 방지됩니다.
//
// 6. 힙 메모리 할당이 전혀 없고, 고정 크기 배열 사용
// 메모리 단편화가 없고, 예측 가능한 메모리 사용량 유지합니다.
//
// - 주의 사항 및 권장 사항
//
// 1. allocate()와 free() 호출은 반드시 짝을 이루어야 합니다.
// 할당한 메모리는 반드시 처리가 끝난 후 free()로 반납해야 합니다.
//
// 2. push() 후 큐에 넣은 포인터는 큐에서 꺼내 반드시 처리 후 free()하세요.
// 그렇지 않으면 메모리 누수가 발생합니다.
//
// 3. free() 시 입력 포인터는 메모리 풀 내 포인터여야 합니다.
// 잘못된 포인터로 호출 시 무시되지만, 설계상 그렇게 쓰면 안 됩니다.


//////////////////////////////////////////////////////////////////////////////////////////////
// MemoryPoolQueue<T,PoolSize>
// - MemoryPool을 사용하여 T* 포인터(풀 내 객체의 포인터)를 안전하게 큐에 넣고 뺌
//////////////////////////////////////////////////////////////////////////////////////////////

// #include "UniversalQueue.h"
//
// #include <vector>
// #include <string>
//
// class SimpleClass {
// public:
//     int value;
//     SimpleClass(int v = 0) : value(v) {}
//     int getValue() const { return value; }
// };
//
// // 파라미터가 있을 때 파라미터들을 함께 저장할 이벤트 구조체
// // 벡터나 클래스 객체들을 담을 수도 있다고 하는데,
// // 대신 참조형이라서 당연히 그 안의 값들이 변하면 가져다 쓸 때도 변한 값을 읽게 된다고.
// struct EventBox {
//     int8_t action; // 이벤트로 감지한 액션 번호.
//
//     // 선택적 파라미터
//     int intParam = 0;
//     float floatParam = 0.0f;
//     const char* strParam = nullptr; // 포인터.
//     std::string stringParam;        // std::string도 사용 가능.
//     std::vector<int> dataVector; // 벡터 가능.
//     SimpleClass simpleObj{0};    // 간단한 클래스 객체 가능.
// };
//
// // 큐의 최대 저장 개수. 큐에는 구조체의 포인터(주소값)만 저장된다.
// // MemoryPool을 생성하는 경우, 이 개수만큼의 용량을 가진, EventBox들의 MemoryPool도 함께 생성된다.
// constexpr size_t eventQueueSize = 10;
//
// // 큐에 단순 int8_t를 저장하는 거라면 MemoryPool을 쓸 필요가 없다.
// // 그냥 메모리 풀 없이 쓰는 기본 UniversalQueue를 사용하는 것이 훨씬 더 간단하고 효율적.
// UniversalQueue <int8_t> button1Queue(eventQueueSize); // 버튼1 이벤트 독립 동작 큐
// // UniversalQueue <EventBox*> button2Queue(eventQueueSize); // 버튼2 이벤트 독립 동작 큐
// // 이렇게도 쓸 수 있지만 EventBox의 new, delete를 확실히 해줘야 한다.
// // EventBox* e = new EventBox(); 해서 push. => pop 해서 다 쓰고 반드시 delete e;
//
// // EventBox를 쓸 거라면 이렇게 미리 할당된 MemoryPool을 사용하는 방식이 조금 더 빠르고 안정적이라고 한다.
// // MemoryPool을 쓰는 UniversalQueue.
// MemoryPoolQueue<EventBox, eventQueueSize> button2Queue; // 버튼2 이벤트 독립 동작 큐
// // EventBox* e = button2Queue.allocate(); 해서 push. => pop 해서 다 쓰고 반드시 button2Queue.free(e);
// MemoryPoolQueue <EventBox, eventQueueSize> buttonCombo12Queue; // 버튼1, 2 콤보 이벤트 조합 동작 큐
//
// // 버튼2 이벤트 큐에 담기.
// EventBox* e = button2Queue.allocate(); // MemoryPool에 할당 후 주소값 가져옴.
// if (e) { // 메모리 할당 성공 시에만 수행.
//     e->action = event2;
//     e->intParam = 123;
//     e->floatParam = 3.14f;
//     e->strParam = "bt2!";
//     e->stringParam = "bt2!";
//     e->dataVector = {1, 2, 3, 4, 5};
//     e->simpleObj = SimpleClass(100);
//     if (!button2Queue.push(e)) button2Queue.free(e); // 1번만 정확하게 푸시해준다. 큐가 가득 찼다거나 해서 푸시 실패 시 메모리 해제.
// }
// // 버튼2 독립 동작 큐 처리.
// while (!button2Queue.isEmpty()) {
//     EventBox* e;
//     if (button2Queue.pop(e)) {
//         button2.doIt(e->action);
//         // doIt(e->action);
//         // e 다 쓰고 나면.
//         button2Queue.free(e); // 반드시 1번만 해제해줘야 한다. 반드시 정확하게 해줘야 해서 pop 성공 시 수행.
//     }
// };

template <typename T, size_t PoolSize>
class MemoryPoolQueue { // MemoryPool 사용하는 큐.
public:
    MemoryPoolQueue() : _queue(PoolSize) {}

    // allocate: 풀에서 객체 할당. (nullptr 리턴 가능)
    T* allocate() {
        return _pool.alloc();
    }

    // push: 할당된 객체 포인터를 큐에 넣음. 실패 시 false 반환.
    bool push(T* item, uint32_t timeout_ms = 0) {
        if (!item) return false;
        return _queue.push(item, timeout_ms);
    }

    // pop: 큐에서 꺼내고, 사용 후 객체 소유권 반환 (자동 free 아님) (성공하면 item에 포인터 저장)
    bool pop(T*& item, uint32_t timeout_ms = 0) {
        return _queue.pop(item, timeout_ms);
    }

    // pop 후 사용이 끝난 객체를 메모리 풀로 반환하는 함수. 사용 후 반드시 free()로 반환하기.
    void free(T* item) {
        _pool.free(item);
    }

    bool isEmpty() { return _queue.isEmpty(); }
    bool isFull() { return _queue.isFull(); }
    size_t size() { return _queue.size(); }
    size_t capacity() { return _queue.capacity(); }

    // non-copyable
    MemoryPoolQueue(const MemoryPoolQueue&) = delete;
    MemoryPoolQueue& operator=(const MemoryPoolQueue&) = delete;

private:
    MemoryPool<T, PoolSize> _pool;
    UniversalQueue<T*> _queue;
};

#endif //UNIVERSALQUEUE_H

// 추가설명 (중요 포인트)
//
// 1) ESP32에서의 동작/주의사항
// Arduino-ESP32 코어나 ESP-IDF 환경에서는 FreeRTOS가 기본 제공되므로, 위 코드에서는 자동으로 USE_FREERTOS가 활성화되어 FreeRTOS API(xQueue*, xSemaphore*, pdMS_TO_TICKS 등)를 사용합니다.
// ISR(인터럽트)에서 큐에 넣어야 한다면 pushFromISR / popFromISR 를 사용하세요. 단, 메모리풀에서 할당(alloc)을 ISR에서 하는 건 권장하지 않습니다(동적 락/대기 발생 가능).
// ESP32는 듀얼코어이므로, 한 코어에서 push하고 다른 코어에서 pop하는 것은 FreeRTOS 큐가 안전하게 처리합니다.
//
// 2) RP2040(피코)와의 차이
// RP2040의 queue_add_blocking / queue_remove_blocking API에는 타임아웃(ms) 매개변수가 없습니다. 그래서 timeout_ms != 0일 때는 무한 대기로 구현해 두었습니다 (원래 소스와 동일한 동작). 만약 timeout 동작을 원하시면 별도로 타임아웃 루프를 만들어 sleep_ms()와 queue_try_add를 조합해야 합니다.
//
// 3) 메모리풀 설계 철학
// 힙 할당을 피하고 정적/스택 안전한 고정 크기 풀을 사용함으로써 메모리 단편화를 제거하고 실시간성이 중요한 임베디드에서 예측 가능한 동작을 보장합니다.
// 하지만 PoolSize가 커지면 정적으로 할당되는 메모리도 커진다는 점 유의하세요 (ESP32의 SRAM 한정 등).
//
// 4) 안전성 권고
// 포인터를 큐로 전달할 때 소유권 규칙을 명확히 하세요 (예: MemoryPool에서 할당한 포인터만 큐에 넣기, pop한 쪽에서 반드시 free).
// ISR에서 push하되, pop이 ISR에서 처리되는 방식은 피해주세요 — 복잡하고 위험합니다.
// 복잡한 객체(벡터, string 등)를 큐에 직접 복사하려면 trivially_copyable 제약 때문에 사용할 수 없습니다. 그런 경우에는 MemoryPool으로 객체를 풀에 만들고 T* 포인터를 큐로 전달하세요.
//
// 5) 빌드 관련
// ESP32: 별도 #define 없이 Arduino-ESP32 프로젝트에 추가하면 자동으로 FreeRTOS 분기가 잡힙니다.
// RP2040(earlephilhower arduino-pico 등) 환경에서는 ARDUINO_ARCH_RP2040가 정의되어야 pico 분기가 동작합니다요