#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "switch.h"

#define F_CPU 16000000UL

#define SWITCH_GPIO_LEFT PE4
#define SWITCH_GPIO_RIGHT PE5
/** @brief Bouncing 방지를 위해 클릭 후 잠시 대기하는 시간(us)*/
#define SWITCH_TIME_SHORT ((50) * (1000))
/** @brief 스위치 클릭 이벤트 발생 주기(us)*/
#define SWITCH_TIME_LONG ((200) * (1000))

enum switch_index {
    SWITCH_L = 0,
    SWITCH_R,
    SWITCH_COUNT,
};

struct switch_t {
    const unsigned int gpio;
    uint32_t timer;
    unsigned int state;
} _switch[SWITCH_COUNT] = {
    { .gpio = SWITCH_GPIO_LEFT },
    { .gpio = SWITCH_GPIO_RIGHT },
};

#define SWITCH_STATE_LONG_OFF   0x01    // 0001
#define SWITCH_STATE_SHORT_ON   0x02    // 0010
#define SWITCH_STATE_LONG_ON    0x04    // 0100
#define SWITCH_STATE_SHORT_OFF  0x08    // 1000

volatile static uint32_t time = 0;

SIGNAL(TIMER2_COMP_vect) {
    time += 1;
}

static bool switch_state_machine(struct switch_t *psw) {
    /*
     * 스위치가 눌리거나 떼질 때, 그 값이 한 번에 깔끔하게 변하지 않고 아주 짧은 시간동안 On, Off를 수십~수백번 반복한다.
     * 이를 Bouncing 혹은 Chattering이라 한다. Bouncing의 특징은 일반적인 노이즈와 다르게
     * 버튼을 누르는 동안 혹은 떼는 동안의 아주 짧은 시간동안만 발생하고, 버튼이 제대로 눌려있거나 떨어져있는 동안은 발생하지 않는다는 점이다.
     * 이러한 특성을 고려하여 bouncing을 방지하기 위한 알고리즘을 설계하고자 한다.
     *
     * 우리 보드에는 버튼이 두 개 있으므로 이를 모두 고려해야 한다. 그러나 문제를 간단하게 하기 위해,
     * 일단 버튼이 한 개인 경우를 먼저 생각한 후 버튼이 두 개인 경우를 고려하기로 한다.
     * 1. 버튼이 한참동안 눌리지 않은 상태에서 갑자기 버튼이 눌린다면 버튼이 눌린 것으로 간주할 수 있다.
     * 2. 버튼이 눌린 직후 버튼이 떼졌다고 하더라도, 이는 일시적인 바운싱에 불과하고 실제로 버튼이 떼진 것은 아니라고 간주할 수 있다.
     * 3. 만약 1. 의 사건이 발생한 직후 버튼이 떼지고 다시 눌리지 않은 채로 충분한 시간이 지났다면 이는 버튼이 떼진 것으로 간주할 수 있다.
     * 	    혹은, 버튼이 충분히 오랫동안 눌려있다가 갑자기 떼진 상태라면 이는 버튼을 떼는 동작으로 간주할 수 있다.
     * 4. 버튼이 충분히 오랫동안 눌려있다가 버튼이 떼진 후, 얼마 지나지 않아 버튼이 다시 눌린다고 하더라도, 이는 일시적인 바운싱에 불과하고 다시 버튼이 눌린 것은 아니다.
     *
     * 즉, 이 알고리즘에 기반하여 생각할 때, 버튼의 상태를 다음과 같이 네 가지로 요약할 수 있다.
     * 1. 오랫동안 눌리지 않은 상태 (Long-Off)
     * 2. 눌러진 지 얼마 안 된 상태 (Short-On)
     * 3. 눌러진 지 오래 된 상태 (Long-On)
     * 4. 떼진 지 얼마 안 된 상태 (Short-Off)
     *
     * Long-Off 상태에서 버튼이 눌릴 경우 Short-On상태로 진입한다.
     * Short-On 상태에서는 바운싱을 방지하기 위해 모든 상태 변화를 무시한다.
     * Short-On 상태가 끝나면 Long-On으로 진입하면서 버튼 눌림 이벤트를 발생시킨다.
     * Long-On 상태에서는 버튼이 떼질 경우 Short-Off 상태로 진입한다.
     * Short-Off 상태에서는 바운싱을 방지하기 위해 모든 상태 변화를 무시한다.
     * Short-Off 상태가 끝나면 Long-Off로 진입한다. 떼는 이벤트도 발생시킬 수 있기는 하겠으나, 굳이 필요하지 않으므로 발생시키지 않기로 한다.
     *
     * 한편, 버튼을 오랫동안 누르고 있을 때의 이벤트 발생 조건에 대해 고려하자.
     * 오랫동안 버튼을 누르고 있을 때에는 충분히 긴 기간동안 눌렸다면 이벤트가 한 번 더 발생하면 된다.
     * 그러므로 Long-On 상태에 있을 때 타이머를 하나 둔 후, 만약 그 타이머가 0 이하로 떨어질 경우 이벤트를 발생시키고 다시 타이머를 초기화하면 된다.
     */

    bool clicked = !((PINE & (1 << (psw->gpio)) == 0x00));
    bool click_event = false;

    switch (psw->state) {
        case SWITCH_STATE_LONG_OFF:
            if (clicked) {
                psw->timer = time + SWITCH_TIME_SHORT;
                psw->state = SWITCH_STATE_SHORT_ON;
            }
            break;
        
        case SWITCH_STATE_SHORT_ON:
            if (psw->timer < time) {
                click_event = true;
                psw->timer = time + SWITCH_TIME_LONG;
                psw->state = SWITCH_STATE_LONG_ON;
            }
            break;
        
        case SWITCH_STATE_LONG_ON:
            if (!clicked) {
                psw->timer = time + SWITCH_TIME_SHORT;
                psw->state = SWITCH_STATE_SHORT_OFF;
                break;
            }

            if (psw->timer < time) {
                click_event = true;
                psw->timer = time + SWITCH_TIME_LONG;
            }
            break;
        
        case SWITCH_STATE_SHORT_OFF:
            if (psw->timer < time) {
                psw->state = SWITCH_STATE_LONG_OFF;
            }
            break;
    }

    return click_event;
}

void switch_init(void) {
    for (int i = 0; i < SWITCH_COUNT; i++) {
        struct switch_t *const state = &_switch[i];

        DDRE &= ~(1 << state->gpio);

        state->timer = 0;
        state->state = SWITCH_STATE_LONG_OFF;
    }

    // 스위치 상태 판별에 필요한 timer2 설정
    // Mode : CTC Mode (WGM2 = 10)
    // Period : 1us
    // 1us = OCR2 * (1 / (clk / prescaler))
    // OCR2 = 1us * (clk / prescaler) = 1us * (16MHz / prescaler) = 16 / prescaler
    // persclaer = 1 -> OCR2 = 16 (CS2 = 001)
    TCCR2 |= (1 << WGM21) | (1 << CS20);
    OCR2 = 16;
}

static inline void switch_copy(struct switch_t *dest, struct switch_t *src) {
    dest->state = src->state;
    dest->timer = src->timer;
}

enum switch_event_t switch_read(void) {
    /*
    * 스위치의 눌림 여부를 결정한다
    */
    bool clicked_l = switch_state_machine(&_switch[SWITCH_L]);
    bool clicked_r = switch_state_machine(&_switch[SWITCH_R]);

    /*
    * 다음으로 버튼이 두 개인 경우를 고려하자.
    * 버튼 두 개를 동시에 누를 경우, 버튼 두 개가 동시에 이벤트가 발생해야 한다.
    * 그런데 위의 알고리즘을 따른다면 양쪽 버튼을 아주 짧은 시간 간격을 두고 누른다 하더라도 제대로 동시 입력으로 간주되지 않을 것이다.
    * 물론 그 차이가 1ms이하라면 그런 문제가 발생하지 않겠지만, 이는 거의 불가능하다.
    * 이를 해결하기 위해서는 한쪽 버튼이 Long-On으로 넘어갈 때 다른 쪽 버튼이 Short-On 상태에 있다면 강제로 Long-On상태로 넘겨버리면 된다.
    */
    if (clicked_l && (_switch[SWITCH_R].state & SWITCH_STATE_SHORT_ON)) {
        switch_copy(&_switch[SWITCH_R], &_switch[SWITCH_L]);
        clicked_r = true;
    }
    if (clicked_r && (_switch[SWITCH_L].state & SWITCH_STATE_SHORT_ON)) {
        switch_copy(&_switch[SWITCH_L], &_switch[SWITCH_R]);
        clicked_l = true;
    }

    enum switch_event_t ret = 0;
    if (clicked_l) {
        ret |= SWITCH_EVENT_LEFT;
    }
    if (clicked_r) {
        ret |= SWITCH_EVENT_RIGHT;
    }

    return ret;

}