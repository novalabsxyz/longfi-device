#include <string.h>
#include "longfiP.h"
#include "board.h"

uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

int16_t RssiValue = 0;
int8_t SnrValue = 0;

static LongFiInternal_t internal;

LongFi_t longfi_new_handle(BoardBindings_t* bindings, Radio_t* radio, RfConfig_t config){
  LongFi_t handle = {
    .radio = radio,
    .config = config,
    .bindings = bindings,
  };
  return handle;
}

void longfi_init(LongFi_t * handle) {

  // store pointer to internal context for callback definitions
  bindings = handle->bindings;

  switch LORA_SPREADING_FACTOR {
    case 10:
    internal.spreading_factor = 3;
    break;
    case 9:
    internal.spreading_factor = 2;
    break;
    case 8:
    internal.spreading_factor = 1;
    break;
    case 7:
    internal.spreading_factor = 0;
    break;
    default:
    internal.spreading_factor = 2;
  }

  // configure sx1276 radio events with local helium functions
  internal.radio_events.TxDone = OnTxDone;
  internal.radio_events.RxDone = OnRxDone;
  internal.radio_events.TxTimeout = OnTxTimeout;
  internal.radio_events.RxTimeout = OnRxTimeout;
  internal.radio_events.RxError = OnRxError;

  // this function calls TimerInits and handle->radio->IoIrqInit, which are implemented here
  handle->radio->Init( &internal.radio_events );
  
  handle->radio->SetChannel( uplink_channel_map[0] );

  handle->radio->SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  
  handle->radio->SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                 LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                 LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}

void longfi_set_buf(LongFi_t * handle, uint8_t * buffer, size_t buffer_len){
  internal.buffer = buffer;
  internal.buffer_len = (buffer!=NULL) ? buffer_len : 0;
}

void longfi_rx(__attribute__((unused))  LongFi_t * handle){
  handle->radio->Rx(0);
}

#define MIN(x,y) ({ \
    typeof(x) _x = (x);     \
    typeof(y) _y = (y);     \
    (void) (&_x == &_y);    \
    _x < _y ? _x : _y; })


// number of bytes in a fragment
size_t payload_bytes_in_single_fragment_packet(){
  return payload_per_fragment[internal.spreading_factor] - sizeof(packet_header_t);
}

// number of bytes in a fragment
size_t payload_bytes_in_first_fragment_of_many(){
  return payload_per_fragment[internal.spreading_factor] - sizeof(packet_header_multiple_fragments_t);
}

// number of bytes in a fragment
size_t payload_bytes_in_subsequent_fragments(){
  return payload_per_fragment[internal.spreading_factor] - sizeof(fragment_header_t);
}

void longfi_rf_test(__attribute__((unused)) LongFi_t * handle){
  uint8_t things[1];
  handle->radio->SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  handle->radio->SetChannel(910000000);
  handle->radio->Send(things, (uint8_t) sizeof(things));
}

uint32_t longfi_get_random(LongFi_t * handle){
 return handle->radio->Random();
}

void _send_random(LongFi_t * handle, uint8_t * data, size_t len){
  uint32_t frequency; 
  bool free_to_transmit = false;
  while (!free_to_transmit){
    uint32_t random = handle->radio->Random()>>16;
    frequency = uplink_channel_map[random%LONGFI_NUM_UPLINK_CHANNELS];
    free_to_transmit = handle->radio->IsChannelFree(MODEM_LORA, frequency, -65, 0);
  }
  handle->radio->SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                               LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                               LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                               true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
  handle->radio->SetChannel(frequency);
  handle->radio->Send(data, len);
}

#include <stdio.h>

void longfi_send(LongFi_t * handle, __attribute__((unused))  QualityOfService qos, const uint8_t * data, size_t len){
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
      .oui = handle->config.oui,
      .device_id = handle->config.device_id,
      .packet_id = 0, //packet_id means no fragments
      .mac = 0xEFFE,
    };
    memcpy(Buffer, &pheader, sizeof(packet_header_t));
    num_bytes_copy = MIN(len, payload_bytes_in_single_fragment_packet());
    internal.tx_len = sizeof(packet_header_t);
  } else {
    // cannot allow packet_id = 0
    while (packet_id == 0) {
      packet_id = (uint8_t) handle->radio->Random();
    }
    packet_header_multiple_fragments_t pheader  = {
      .oui = handle->config.oui,
      .device_id = handle->config.device_id,
      .packet_id = packet_id,
      .fragment_num = 0x00,
      .num_fragments = num_fragments,
      .mac = 0xEFFE,
    };
    memcpy(Buffer, &pheader, sizeof(packet_header_multiple_fragments_t));
    num_bytes_copy = MIN(len, payload_bytes_in_first_fragment_of_many());
    internal.tx_len = sizeof(packet_header_multiple_fragments_t);
  }

  // copy the necessary amount of payload  
  memcpy(&Buffer[internal.tx_len], data, num_bytes_copy);
  payload_consumed += num_bytes_copy;
  internal.tx_len += num_bytes_copy;
  // initialize tx_cnt with current len, as first transmit will be this
  internal.tx_cnt = internal.tx_len;
  for(uint32_t cnt_fragments = 1; cnt_fragments < num_fragments; cnt_fragments++) {
    fragment_header_t fheader  = {
      .packet_id = packet_id,
      .fragment_num = cnt_fragments,
      .mac = 0xEFFE,
    };
    memcpy(&Buffer[internal.tx_len], &fheader, sizeof(fragment_header_t));
    internal.tx_len += sizeof(fragment_header_t);
    num_bytes_copy = MIN(len - payload_consumed, payload_bytes_in_subsequent_fragments());
    memcpy(&Buffer[internal.tx_len], &data[payload_consumed], num_bytes_copy);
    internal.tx_len += num_bytes_copy;
    payload_consumed+= num_bytes_copy;
  };
  _send_random(handle, Buffer, internal.tx_cnt);
}

RxPacket longfi_get_rx(__attribute__((unused))  LongFi_t * handle){
  RxPacket rx = {
    .buf = internal.buffer,
    .len = internal.rx_len,
    .rssi = RssiValue,
    .snr = SnrValue,
  };
  internal.buffer = NULL;
  internal.buffer_len = 0;
  return rx;
}

ClientEvent longfi_handle_event(LongFi_t * handle, RfEvent event){
  internal.cur_event = InternalEvent_None;

  switch (event) {
    case DIO0:
      (*(internal.dio_irq_handles[0]))();
      break;
    case DIO1:
      (*internal.dio_irq_handles[1])();
      break;
    case DIO2:
      (*internal.dio_irq_handles[2])();
      break;
    case DIO3:
      (*internal.dio_irq_handles[3])();
      break;
    case DIO4:
      (*internal.dio_irq_handles[4])();
      break;
    case DIO5:
      (*internal.dio_irq_handles[5])();
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

  return _handle_internal_event(handle);

}

ClientEvent _handle_internal_event(LongFi_t * handle){
  switch (internal.cur_event) {
    case InternalEvent_None:
      return ClientEvent_None;
      break;
    case InternalEvent_TxDone:
      // if all the tx data is sent, we're done!
      if(internal.tx_len == internal.tx_cnt){
        return ClientEvent_TxDone;
      } 
      // else we are sending another fragment
      else {
        uint8_t len = MIN(
          payload_per_fragment[internal.spreading_factor],
          internal.tx_len - internal.tx_cnt
        );
        _send_random(handle, &Buffer[internal.tx_cnt], len);
        internal.tx_cnt += len;
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

// each sx12xx board definition can invoke this
void IoIrqInit(IrqHandler* irq_handlers[NUM_IRQ_HANDLES]){
  for(uint32_t i=0; i<NUM_IRQ_HANDLES; i++){
     internal.dio_irq_handles[i] = irq_handlers[i]; 
  }
}

void OnTxDone( void )
{
  internal.cur_event = InternalEvent_TxDone;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
  internal.cur_event = InternalEvent_RxDone;
  uint8_t rx_len = (uint8_t) MIN( (size_t) size, internal.buffer_len);
  internal.rx_len = rx_len;
  memcpy(internal.buffer, payload, rx_len);
  RssiValue = rssi;
  SnrValue = snr;
}


void OnTxTimeout( void )
{
  internal.cur_event = InternalEvent_TxTimeout;
}

void OnRxTimeout( void )
{
  internal.cur_event = InternalEvent_RxTimeout;
}

void OnRxError( void )
{
  internal.cur_event = InternalEvent_RxTimeout;
}


