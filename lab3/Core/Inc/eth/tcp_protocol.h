#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <eth/lwip_tcp.h>
#include "stdbool.h"

/* Maximum allowed errors (user defined). */
#define X_MAX_ERRORS ((uint8_t)3u) //TODO max errors

/* Bytes defined by the protocol. */
#define NOTE_A    ((uint8_t)0x01u)
#define NOTE_B    ((uint8_t)0x02u)
#define NOTE_C    ((uint8_t)0x03u)
#define NOTE_D    ((uint8_t)0x04u)
#define NOTE_E    ((uint8_t)0x05u)
#define NOTE_F    ((uint8_t)0x06u)
#define NOTE_G    ((uint8_t)0x07u)
#define UP_OCT    ((uint8_t)0x10u)
#define DOWN_OCT  ((uint8_t)0x11u)
#define UP_DUR    ((uint8_t)0x20u)
#define DOWN_DUR  ((uint8_t)0x21u)
#define PLAY_ALL  ((uint8_t)0x35u)
#define BOOT	  ((uint8_t)0xFFu)  /* Go to bootloader */

typedef enum {
  CODE,
  DATA
} transfer_status;

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p);

#endif /* INC_PROTOCOL_H_ */
