#include <eth/tcp_protocol.h>
#include "eth/lwip_tcp.h"
#include "string.h"
#include "traffic_light.h"
#include "hardware/button.h"
#include "command.h"
#include "menu.h"
#include "usart.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

static uint32_t durations[3] = {
        [RED] = 6000,
        [YELLOW] = 1500,
        [GREEN] = 1500
};

static char * getTrafficLightInfo();
static char * changeButtonMode(const uint8_t intArg);
static char * changeInterruptionMode(const uint8_t onOffStatus);
static char * changeRedTimeout(const uint8_t intArg);

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p) {
	WRITE_REG(IWDG->KR, 0x0000AAAAU);
	if (p == NULL || p->len <= 0 || p->payload == NULL) return "handle error";
	uint8_t header;
	uint8_t* data = NULL;
	uint16_t length = p->len - 1;
	memcpy(&header, p->payload, 1);
	if (length > 0) {
		data = malloc(sizeof(uint8_t) * length);
		memcpy(data, p->payload + 1, length);
	}
	char* msg;
	switch (header) {
	    case INFO:
	    	return getTrafficLightInfo();
	    case BTN:
	    	msg = changeButtonMode((uint8_t) data[0]);
	    	free(data);
	    	return msg;
	    case INTR:
	    	msg = changeInterruptionMode((uint8_t) data[0]);
	    	free(data);
	    	return msg;
	    case REDT:
	    	msg = changeRedTimeout((uint8_t) data[0]);
	    	free(data);
	    	return msg;
		case BOOT:
			HAL_FLASH_Unlock();
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)0x08104000, 0);
			HAL_FLASH_Lock();
			NVIC_SystemReset();
			return "";
		default:
			return "No such command\n";
	}
}

static char cmdBuf[128];
static char * getTrafficLightInfo() {
    char *bufLast = cmdBuf;
    uint8_t isBlinking = trafficLightIsBlinking();
    char *currentColor = colorGetName(trafficLightGetColor());
    bufLast += sprintf(bufLast, "Light: %s%s\n",
            isBlinking ? "blinking " : "",
            currentColor
    );
    bufLast += sprintf(bufLast, "Button mode: mode %" PRIu32 " (%s)\n",
                       buttonIsEnabled() + 1,
                       buttonIsEnabled() ? "enabled" : "disabled"
    );
    bufLast += sprintf(bufLast, "Red timeout: %" PRIu32 " s\n",
                       durations[RED] / (uint32_t) 1000
    );
    bufLast += sprintf(bufLast, "Interrupts mode: %s\n\r",
                       uartIsInterruptionEnabled()  ? "I" : "P"
    );
    return cmdBuf;
}

static char * changeButtonMode(const uint8_t intArg) {
    if (intArg == 2) {
    	buttonEnable();
        return "Enable button\n";
    } else if (intArg == 1) {
        buttonDisable();
        return "Disable button\n";
    } else {
        return "The int argument must be 1 or 2\n";
    }
}

static char * changeInterruptionMode(const uint8_t onOffStatus) {
    if (onOffStatus == 0) {
        uartEnableInterruption();
        return "Enable int\n";
    } else if (onOffStatus == 1) {
        uartDisableInterruption();
        return "Disable int\n";
    } else {
        return "The argument must be 1 or 2\n";
    }
}

static char * changeRedTimeout(const uint8_t intArg) {
    durations[RED] = intArg * 1000;
    trafficLightSetDuration(RED, durations[RED]);
    return "OK\n";
}
