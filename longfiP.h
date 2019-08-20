#include "radio/radio.h"
#include "longfi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RADIO_1                                     920600000
#define RADIO_2                                     916600000
#define FREQ_SPACING                                   200000
#define LONGFI_NUM_UPLINK_CHANNELS                          8

const uint32_t uplink_channel_map[LONGFI_NUM_UPLINK_CHANNELS] = {
  RADIO_1 - FREQ_SPACING*2,
  RADIO_1 - FREQ_SPACING,
  RADIO_1,
  RADIO_2 - FREQ_SPACING*2,
  RADIO_2 - FREQ_SPACING,
  RADIO_2,
  RADIO_2 + FREQ_SPACING,
  RADIO_2 + FREQ_SPACING*2
};

#define NUM_SF        (4)
typedef enum {
  LongFi_SF7 = 0,
  LongFi_SF8 = 1,
  LongFi_SF9 = 2,
  LongFi_SF10 = 3,
} LongFiSpreading_t;

const uint32_t payload_per_fragment[NUM_SF] = {
  24,24,24,24
};

const uint32_t fragments_per_channel[NUM_SF] = {
  5, 2, 1, 1
};

#define TX_OUTPUT_POWER                             22        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       (10)         // [SF7..SF12]
#define LORA_CODINGRATE                             (1)         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        (8)         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         (5)         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  (false)
#define LORA_IQ_INVERSION_ON                        (false)

#pragma pack(push, 1)
// if first byte is 0, is single fragment packet_header
typedef struct {
  uint8_t packet_id;  // 0    must be zero
  uint32_t oui;       // 1:4
  uint16_t device_id; // 5:6
  uint16_t mac;       // 7:8
} packet_header_t;

// if second byte is 0, is multi-fragment packet_header
typedef struct {
  uint8_t packet_id;      // 0    must be non-zero
  uint8_t fragment_num;   // 1    must be zero (byte)
  uint8_t num_fragments;  // 2    must be non-zero
  uint32_t oui;           // 3:6
  uint16_t device_id;     // 7:8
  uint16_t mac;           // 9:10
} packet_header_multiple_fragments_t;

// else (first and second byte, non-zero), is packet fragment
typedef struct {
  uint8_t packet_id;        // 0    must be non-zero
  uint8_t fragment_num;     // 1    must be non-zero
  uint16_t mac;             // 2:3  
} fragment_header_t;
#pragma pack(pop)

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 128 // Define the payload size here

/*
 *  Functions implemented here that SX1276 will call
 *  
 *  As far as I can tell, these are only called from SX1276 when:
 *      LongFi.dio_irq_handles are dispatched
 *      Timeout callbacks are dispatched
 *  With the event refactoring here, these callbacks will only be called from `helium_rf_handle_event`
 */
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );

/*
 * Private helper for handling internal events
 */
ClientEvent _handle_internal_event(LongFi_t * handle);

/*
 * Private helper for counting bytes
 */
size_t payload_bytes_in_single_fragment_packet();
size_t payload_bytes_in_first_fragment_of_many();
size_t payload_bytes_in_subsequent_fragments();



#ifdef __cplusplus
}
#endif