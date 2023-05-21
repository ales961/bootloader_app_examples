#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <eth/lwip_tcp.h>
#include "stdbool.h"

/* Maximum allowed errors (user defined). */
#define X_MAX_ERRORS ((uint8_t)3u) //TODO max errors

/* Bytes defined by the protocol. */
#define INFO      ((uint8_t)0x01u)  /* Get traffic light info */
#define BTN       ((uint8_t)0x02u)  /* Change button mode */
#define INTR      ((uint8_t)0x05u)  /* Change interruption mode */
#define REDT      ((uint8_t)0x07u)  /* Change red timeout */
#define BOOT	  ((uint8_t)0xFFu)  /* Go to bootloader */

typedef enum {
  CODE,
  DATA
} transfer_status;

char* handle_packet(struct tcp_pcb* tpcb, struct pbuf* p);

#endif /* INC_PROTOCOL_H_ */
