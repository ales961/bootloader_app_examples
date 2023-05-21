#ifndef HARDWARE_LED_H
#define HARDWARE_LED_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct Led Led;
typedef enum { LED_TURN_ON, LED_TURN_OFF } LedState;



void ledSetStateConstantly(Led *led, LedState state);
void ledSetStateWithDuration(Led *led, LedState state, uint16_t duration);
LedState ledGetState(Led *led);
void ledSetPeriod(Led *led, uint16_t period, uint16_t duration);
Led* ledInit(LedState initState, GPIO_TypeDef* port, uint16_t pin);
void ledUpdateState(Led *led);
uint8_t ledFinishedLightning(Led *led);
uint32_t ledGetLightningTime(const Led *led);


#endif //HARDWARE_LED_H
