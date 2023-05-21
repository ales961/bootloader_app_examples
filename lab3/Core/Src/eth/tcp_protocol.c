#include <eth/tcp_protocol.h>
#include "eth/lwip_tcp.h"
#include "string.h"
#include "hardware/buzzer.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p) {
	WRITE_REG(IWDG->KR, 0x0000AAAAU);
	if (p == NULL || p->len <= 0 || p->payload == NULL) return "handle error";
	uint8_t header;
	memcpy(&header, p->payload, 1);
	switch (header) {
		case NOTE_A:
			return playA();
		case NOTE_B:
			return playB();
		case NOTE_C:
			return playC();
		case NOTE_D:
			return playD();
		case NOTE_E:
			return playE();
		case NOTE_F:
			return playF();
		case NOTE_G:
			return playG();
		case UP_OCT:
			return upOctave();
		case DOWN_OCT:
			return downOctave();
		case UP_DUR:
			return upDuration();
		case DOWN_DUR:
			return downDuration();
		case PLAY_ALL:
			return playAll();
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
