#ifndef HARDWARE_BUTTON_H
#define HARDWARE_BUTTON_H

#include <stdint.h>

#include "stm32f4xx_hal.h"

#define SHORT_CLICK 50

typedef void (*buttonActionListener)();

void buttonSetOnClick(buttonActionListener actionListener);
void buttonInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void buttonUpdateState();
void buttonEnable();
void buttonDisable();
uint8_t buttonIsEnabled();

#endif //HARDWARE_BUTTON_H
