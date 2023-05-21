#include "hardware/led.h"

#include <malloc.h>

struct Led {
    LedState state;
    uint32_t startTime;
    int32_t duration;
    int32_t period;
    GPIO_TypeDef* port;
    uint16_t pin;
};

static void ledHardwareSet(Led *led);
static void ledToggleState(Led *led);

struct Led* ledInit(LedState initState, GPIO_TypeDef* port, uint16_t pin) {
    Led *led = (Led *) malloc(sizeof(Led));
    *led = (Led) { .state = initState, .startTime = 0, .duration = 0, .period = 0, .port = port, .pin = pin };
    ledSetStateConstantly(led, initState);
    return led;
}

void ledUpdateState(Led *led) {
    if (led->duration == 0 && led->period == 0) {
        return;
    }
    uint32_t time = HAL_GetTick();
    uint16_t delta = time - led->startTime;
    if (led->period > 0 && delta >= led->period) {
        led->duration -= led->period;
        if (led->duration <= 0) {
        	ledSetStateConstantly(led, LED_TURN_OFF);
            led->period = 0;
            led->duration = 0;
        } else {
            ledToggleState(led);
            led->startTime = HAL_GetTick();
        }
    } else if (time - led->startTime >= led->duration) {
        ledToggleState(led);
        led->duration = 0;
    }
}

uint32_t ledGetLightningTime(const Led * const led) {
    if (led->state != LED_TURN_ON)
        return 0;
    return HAL_GetTick() - led->startTime;
}

void ledSetStateConstantly(Led *led, LedState state) {
    led->state = state;
    led->duration = 0;
    led->period = 0;
    ledHardwareSet(led);
}

void ledSetPeriod(Led *led, uint16_t period, uint16_t duration) {
    led->state = LED_TURN_ON;
    led->period = period;
    led->duration = duration;
    led->startTime = HAL_GetTick();
    ledHardwareSet(led);
}

void ledSetStateWithDuration(Led *led, LedState state, uint16_t duration) {
    led->state = state;
    led->period = 0;
    led->duration = duration;
    led->startTime = HAL_GetTick();
    ledHardwareSet(led);
}


LedState ledGetState(Led *led) {
    return led->state;
}

uint8_t ledFinishedLightning(Led *led) {
    return led->duration == 0 && led->period == 0;
}

static void ledToggleState(Led *led) {
    led->state = led->state == LED_TURN_ON ? LED_TURN_OFF : LED_TURN_ON;
    ledHardwareSet(led);
}

static void ledHardwareSet(Led *led) {
    HAL_GPIO_WritePin(led->port, led->pin, led->state == LED_TURN_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
