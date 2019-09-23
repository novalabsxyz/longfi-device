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

typedef enum QualityOfService {
	LONGFI_QOS_0, // send packets with no ACK
	LONGFI_QOS_1, // wait for ACK (unimplemented)
	LONGFI_QOS_2, // (unimplemented)
	_MAX = 0xFFFFFFFF //force 32-bit value
} QualityOfService;

typedef struct  {
	uint32_t oui; // organizations unique identifier
	uint16_t device_id; // device identifier within organization
} RfConfig_t;

typedef struct {
	Radio_t * radio; // pointer to struct of SX12XX radio functions
	BoardBindings_t * bindings; // pointer to struct of system bindings, defined in board.h
	RfConfig_t config;
} LongFi_t;

LongFi_t longfi_new_handle(BoardBindings_t* bindings, Radio_t* radio, RfConfig_t config);

// initializes the library before usage
void longfi_init(LongFi_t * handle);

// enables Temperature Compensated External Oscillator
void longfi_enable_tcxo(LongFi_t * handle);

// these are the events to be handled by the client
typedef enum ClientEvent {
	ClientEvent_None, // this is a non-event, no handling required
	ClientEvent_TxDone, // the full transmit is complete (1 or more fragments)
	ClientEvent_Rx, // a full packet was received
} ClientEvent;

// this will give ownership of a buffer to longfi library
// this should determine max size of transmit/receive BUT currently a static buffer in longfi.c does
void longfi_set_buf(LongFi_t * handle, uint8_t * buffer, size_t buffer_len);

// client can asyncronously dispatch a send
// it is not safe to use this function again without have received ClientEvent_TxDone
void longfi_send(LongFi_t * handle, QualityOfService qos, const uint8_t * data, size_t len);

// received packets are returned to the client this way
// buf is the pointer to the buffer configured in longfi_set_buf
typedef struct RxPacket {
	uint8_t * buf;
	size_t len;
	int16_t rssi;
	int8_t snr;
} RxPacket;

// this returns the received packet
// (currently there is no user API for receiving packets)
RxPacket longfi_get_rx();

// these are system generated events that the client must collect and push into longfi_handle_event
// all the DIO events are pin interrupts
typedef enum RfEvent {
	DIO0, // TxDone or Rx
	DIO1, // unimplemented
	DIO2, // unimplemented
	DIO3, // unimplemented
	DIO4, // unimplemented
	DIO5, // unimplemented
	Timer1, // unimplemented
	Timer2, // unimplemented
	Timer3 // unimplemented
} RfEvent;

// to be used by client to loop over process_event
// run at a low priority
ClientEvent longfi_handle_event(LongFi_t * handle, RfEvent);

// sends a byte (0xAB) at 910MHz - useful for setting of RF hardware test
void longfi_rf_test(LongFi_t * handle);

// returns a random 32-bit number based on RSSI readings
// beware: this puts the radio to sleep after getting readings
uint32_t longfi_get_random(LongFi_t * handle);

extern const struct Radio_s Radio;

#ifdef __cplusplus
}
#endif

#endif // __LONGFI_H__
