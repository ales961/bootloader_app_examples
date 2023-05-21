#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "hardware/Led.h"

typedef enum { RED = 0, YELLOW = 1, GREEN = 2 } Color;

void trafficLightUpdate();
void trafficLightSetShortRed(uint32_t duration);
void trafficLightSetDuration(Color color, uint32_t duration);
Color trafficLightGetColor();
uint8_t trafficLightIsBlinking();
void trafficLightStart();
void trafficLightStop();
void trafficLightInit(
		GPIO_TypeDef* red_port, uint16_t red_pin,
		GPIO_TypeDef* yellow_port, uint16_t yellow_pin,
		GPIO_TypeDef* green_port, uint16_t green_pin
);
char * colorGetName(Color color);

#endif //TRAFFICLIGHT_H
