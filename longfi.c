
#include <string.h> 
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "longfiP.h"

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

int16_t RssiValue = 0;
int8_t SnrValue = 0;

/*!
 * Given the constraints of the SX1276 API, we have to use static struct
 * It sets up the constraint of only one instance per system for now
 */
static LongFi_t LongFi;


void longfi_enable_tcxo(){
  SX1276Write( REG_OCP, ( RF_OCP_ON | RF_OCP_TRIM_120_MA ) );
  SX1276Write( REG_TCXO, ( SX1276Read( REG_TCXO ) & RF_TCXO_TCXOINPUT_MASK ) | RF_TCXO_TCXOINPUT_ON );
  SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RF_OPMODE_MASK ) | RF_OPMODE_SLEEP );
}

void longfi_init(struct RfConfig config) {
  // save config in static struct
  LongFi.config = config;

  switch LORA_SPREADING_FACTOR {
    case 10:
    LongFi.spreading_factor = 3;
    break;
    case 9:
    LongFi.spreading_factor = 2;
    break;
    case 8:
    LongFi.spreading_factor = 1;
    break;
    case 7:
    LongFi.spreading_factor = 0;
    break;
    default:
    LongFi.spreading_factor = 2;
  }

  // configure sx1276 radio events with local helium functions
  LongFi.radio_events.TxDone = OnTxDone;
  LongFi.radio_events.RxDone = OnRxDone;
  LongFi.radio_events.TxTimeout = OnTxTimeout;
  LongFi.radio_events.RxTimeout = OnRxTimeout;
  LongFi.radio_events.RxError = OnRxError;

  // this function calls TimerInits and SX1276IoIrqInit, which are implemented here
  SX1276Init( &LongFi.radio_events );

  SX1276SetChannel( frequency_table[0] );

  SX1276SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  
  SX1276SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                 LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                 LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}

void longfi_set_buf(uint8_t * buffer, size_t buffer_len){
  LongFi.buffer = buffer;
  LongFi.buffer_len = (buffer!=NULL) ? buffer_len : 0;
}

void longfi_rx(){
  SX1276SetRx(0);
}

#define MIN(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    (void) (&_x == &_y);    \
    _x < _y ? _x : _y; })


// number of bytes in a fragment
size_t payload_bytes_in_single_fragment_packet(){
  return payload_per_fragment[LongFi.spreading_factor] - sizeof(packet_header_t);
}

// number of bytes in a fragment
size_t payload_bytes_in_first_fragment_of_many(){
  return payload_per_fragment[LongFi.spreading_factor] - sizeof(packet_header_multiple_fragments_t);
}

// number of bytes in a fragment
size_t payload_bytes_in_subsequent_fragments(){
  return payload_per_fragment[LongFi.spreading_factor] - sizeof(fragment_header_t);
}

void longfi_rf_test(){
  uint8_t things[1];
  SX1276SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  SX1276SetChannel(910000000);
  SX1276Send(things, (uint8_t) sizeof(things));
}

void _send_random(uint8_t * data, size_t len){
  uint32_t frequency; 
  bool free_to_transmit = false;
  while (!free_to_transmit){
    uint32_t random = SX1276Random()>>16;
    frequency = frequency_table[random%LONGFI_NUM_UPLINK_CHANNELS];
    free_to_transmit = SX1276IsChannelFree(MODEM_LORA, frequency, -65);
  }
  SX1276SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                               LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                               LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                               true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  SX1276SetChannel(frequency);
  SX1276Send(data, len);
}

void longfi_send(const uint8_t * data, size_t len){
  uint32_t num_fragments;
  size_t payload_consumed = 0;
  uint8_t packet_id = 0;
  size_t num_bytes_copy;

  if (len < payload_bytes_in_single_fragment_packet()){
    num_fragments = 1;
  } else {
    uint32_t remaining_len = len - payload_bytes_in_first_fragment_of_many();
    num_fragments = 1 + remaining_len / payload_bytes_in_subsequent_fragments();

    // if there was remainder, we need a final fragment
    if (remaining_len%payload_bytes_in_subsequent_fragments() != 0){
      num_fragments += 1;
    }
  }

  // copy in short header for single fragment packets
  if (num_fragments <= 1) {
    packet_header_t pheader  = {
      .oui = LongFi.config.oui,
      .device_id = LongFi.config.device_id,
      .packet_id = 0, //packet_id means no fragments
      .mac = 0xEFFE,
    };
    memcpy(Buffer, &pheader, sizeof(packet_header_t));
    num_bytes_copy = MIN(len, payload_bytes_in_single_fragment_packet());
    LongFi.tx_len = sizeof(packet_header_t);
  } else {
    // cannot allow packet_id = 0
    while (packet_id == 0) {
      packet_id = (uint8_t) SX1276Random();
    }
    packet_header_multiple_fragments_t pheader  = {
      .oui = LongFi.config.oui,
      .device_id = LongFi.config.device_id,
      .packet_id = packet_id,
      .fragment_num = 0x00,
      .num_fragments = num_fragments,
      .mac = 0xEFFE,
    };
    memcpy(Buffer, &pheader, sizeof(packet_header_multiple_fragments_t));
    num_bytes_copy = MIN(len, payload_bytes_in_first_fragment_of_many());
    LongFi.tx_len = sizeof(packet_header_multiple_fragments_t);
  }

  // copy the necessary amount of payload  
  memcpy(&Buffer[LongFi.tx_len], data, num_bytes_copy);
  payload_consumed += num_bytes_copy;
  LongFi.tx_len += num_bytes_copy;
  // initialize tx_cnt with current len, as first transmit will be this
  LongFi.tx_cnt = LongFi.tx_len;

  for(uint32_t cnt_fragments = 1; cnt_fragments < num_fragments; cnt_fragments++) {
    fragment_header_t fheader  = {
      .packet_id = packet_id,
      .fragment_num = cnt_fragments,
      .mac = 0xEFFE,
    };
    memcpy(&Buffer[LongFi.tx_len], &fheader, sizeof(fragment_header_t));
    LongFi.tx_len += sizeof(fragment_header_t);
    num_bytes_copy = MIN(len - payload_consumed, payload_bytes_in_subsequent_fragments());
    memcpy(&Buffer[LongFi.tx_len], &data[payload_consumed], num_bytes_copy);
    LongFi.tx_len += num_bytes_copy;
    payload_consumed+= num_bytes_copy;
  };

  _send_random(Buffer, LongFi.tx_cnt);
}

RxPacket longfi_get_rx(){
  RxPacket rx = {
    .buf = LongFi.buffer,
    .len = LongFi.rx_len,
    .rssi = RssiValue,
    .snr = SnrValue,
  };
  LongFi.buffer = NULL;
  LongFi.buffer_len = 0;
  return rx;
}

ClientEvent longfi_handle_event(RfEvent event){

  LongFi.cur_event = InternalEvent_None;

  switch (event) {
    case DIO0:
      (*LongFi.dio_irq_handles[0])();
      break;
    case DIO1:
      (*LongFi.dio_irq_handles[1])();
      break;
    case DIO2:
      (*LongFi.dio_irq_handles[2])();
      break;
    case DIO3:
      (*LongFi.dio_irq_handles[3])();
      break;
    case DIO4:
      (*LongFi.dio_irq_handles[4])();
      break;
    case DIO5:
      (*LongFi.dio_irq_handles[5])();
      break;
    case Timer1:
      // TODO: needs to dispatch the callback stashed from TimerInit
      break;
    case Timer2:
      // TODO: needs to dispatch the callback stashed from TimerInit
      break;
    case Timer3:
      // TODO: needs to dispatch the callback stashed from TimerInit
      break;
    default:
      break;
  }

  return _handle_internal_event(LongFi.cur_event);

}

ClientEvent _handle_internal_event(InternalEvent_t event){
  switch (event) {
    case InternalEvent_None:
      return ClientEvent_None;
      break;
    case InternalEvent_TxDone:
      // could trigger pending Tx'es here
      if (LongFi.tx_len == LongFi.tx_cnt){
        return ClientEvent_TxDone;
      } else {
        uint8_t len = MIN(
          payload_per_fragment[LongFi.spreading_factor],
          LongFi.tx_len - LongFi.tx_cnt
        );
        _send_random(&Buffer[LongFi.tx_cnt], len);
        LongFi.tx_cnt += len;
        return ClientEvent_None;
      }      
      break;
    case InternalEvent_RxDone:
      // could trigger pending Tx'es here
      return ClientEvent_Rx;
      break;
    case InternalEvent_TxTimeout:
      // potential internal retry logic
      break;
    case InternalEvent_RxTimeout:
      // potential internal retry logic
      break;
    case InternalEvent_RxError:
      // potential internal retry logic
      break;
    default:
      break;
  }
  return ClientEvent_None;
}

void SX1276IoIrqInit(irq_ptr irq_handlers[NUM_IRQ_HANDLES]){
  for(uint32_t i=0; i<NUM_IRQ_HANDLES; i++){
    LongFi.dio_irq_handles[i] = irq_handlers[i]; 
  }
}


void OnTxDone( void )
{
    LongFi.cur_event = InternalEvent_TxDone;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
  LongFi.cur_event = InternalEvent_RxDone;
  uint8_t rx_len = (uint8_t) MIN( (size_t) size, LongFi.buffer_len);
  LongFi.rx_len = rx_len;
  memcpy(LongFi.buffer, payload, rx_len);
  RssiValue = rssi;
  SnrValue = snr;
}

void memcpy1(const uint8_t *dst, const uint8_t *src, uint16_t size){
  memcpy((void*) dst, (void*) src, size);
}

void OnTxTimeout( void )
{
  LongFi.cur_event = InternalEvent_TxTimeout;
}

void OnRxTimeout( void )
{
  LongFi.cur_event = InternalEvent_RxTimeout;
}

void OnRxError( void )
{
  LongFi.cur_event = InternalEvent_RxTimeout;
}


