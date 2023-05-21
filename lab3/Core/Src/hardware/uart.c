#include "hardware/uart.h"

#include "usart.h"

uint8_t usartHardwareTransmitBuffer;
uint8_t usartHardwareReceiveBuffer;

uint8_t uartHardwareTransmitPolling(const uint8_t data, const uint16_t timeOut) {
    return HAL_UART_Transmit(&huart6, &data, 1, timeOut) == HAL_OK;
}

uint8_t uartHardwareReceivePolling(uint8_t * const data, const uint16_t timeOut) {
    return HAL_UART_Receive(&huart6, data, 1, timeOut) == HAL_OK;
}

void uartHardwareDisableInterrupts() {
    HAL_UART_Abort(&huart6);
    HAL_NVIC_DisableIRQ(USART6_IRQn);
}


