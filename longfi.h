#ifndef __LONGFI_H__
#define __LONGFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "board.h"
#include "radio/radio.h"

//// 
// This section is internal stuff that needs to be declared so that size of LongFiInternal_t is known

typedef enum {
  InternalEvent_None,
  InternalEvent_TxDone,
  InternalEvent_RxDone,
  InternalEvent_TxTimeout,
  InternalEvent_RxTimeout,
  InternalEvent_RxError,
} InternalEvent_t;

typedef struct {
  uint8_t spreading_factor;
  void (*dio_irq_handles[NUM_IRQ_HANDLES])();
  RadioEvents_t radio_events;
  InternalEvent_t cur_event;
  uint8_t * buffer;
  size_t buffer_len;
  uint32_t rx_len;
  uint32_t tx_cnt;
  uint32_t tx_len;
} LongFiInternal_t;

// End of uninteresting section for client
////


typedef enum QualityOfService {
	LONGFI_QOS_0, // YOLO packets out and go to sleep. RX only during network_poll
	LONGFI_QOS_1, // make best effort to get ACKs, provide client notice of mail being available
	LONGFI_QOS_2, // make best effort to get ACKs and pull down mail ASAP
	_MAX = 0xFFFFFFFF
} QualityOfService;

typedef struct  {
	uint32_t oui;
	uint16_t device_id;
} RfConfig_t;

typedef struct {
	Radio_t * radio;
	BoardBindings_t * bindings;
	RfConfig_t config;
} LongFi_t;

LongFi_t longfi_new_handle(BoardBindings_t* bindings, Radio_t* radio, RfConfig_t config);

void longfi_enable_tcxo();
void longfi_init(LongFi_t * handle);
uint32_t longfi_get_random(LongFi_t * handle);

typedef enum ClientEvent {
	ClientEvent_None,
	ClientEvent_TxDone,
	ClientEvent_Rx,
} ClientEvent;

// this will give ownership of a buffer to longfi library
// should it trigger automatic fetch of mail if it remembers it from previous ACK?
// if no, then we need to provide an API for client to do that action specifically
void longfi_set_buf(LongFi_t * handle, uint8_t * buffer, size_t buffer_len);

void longfi_send(LongFi_t * handle, QualityOfService qos, const uint8_t * data, size_t len);

typedef struct RxPacket {
	uint8_t * buf;
	size_t len;
	int16_t rssi;
	int8_t snr;
} RxPacket;

RxPacket longfi_get_rx();

typedef enum RfEvent {
	DIO0,
	DIO1,
	DIO2,
	DIO3,
	DIO4,
	DIO5,
	Timer1,
	Timer2,
	Timer3
} RfEvent;

// to be used by client to loop over process_event
ClientEvent longfi_handle_event(LongFi_t * handle, RfEvent);

void longfi_rf_test(LongFi_t * handle);

extern const struct Radio_s Radio;

#ifdef __cplusplus
}
#endif

#endif // __LONGFI_H__
