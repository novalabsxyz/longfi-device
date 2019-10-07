#include <CppUTest/TestHarness.h>

#include "../board.h"
#include "../longfi.h"
#include "../longfiP.h"
#include "radio-board.h"

RfConfig_t longfi_rf_config = {
    .oui = 0xFEED3EAB,
    .device_id = 0xABCD,
};


static Radio_t radio = MockRadioNew();
static LongFi_t longfi_handle;
// implemented in RadioSuite.cpp
extern BoardBindings_t my_bindings;

TEST_GROUP(LongFiGroup)
{
    void setup()
   {
    longfi_handle.bindings = &my_bindings;

    longfi_handle.radio = &radio;
    longfi_handle.config = longfi_rf_config;
    longfi_init(&longfi_handle);
   }

   void teardown()
   {
      
   }

};


TEST(LongFiGroup, PayloadBytesInSingleFragmentPacket)
{
    UNSIGNED_LONGS_EQUAL(15, payload_bytes_in_single_fragment_packet());
}


TEST(LongFiGroup, PayloadBytesInFirstFragmentOfMany)
{
    UNSIGNED_LONGS_EQUAL(13, payload_bytes_in_first_fragment_of_many());
}

TEST(LongFiGroup, PayloadBytesInSubsequentFragments)
{

    UNSIGNED_LONGS_EQUAL(20, payload_bytes_in_subsequent_fragments());
}


uint8_t send_length = 0;
uint8_t * send_buffer = 0;

TEST(LongFiGroup, SingleFragmentPacket)
{
    uint8_t test_data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    longfi_send(&longfi_handle, test_data, sizeof(test_data));
    
    // assert that the packet sent is equal to test_data plus packet_header
    LONGS_EQUAL(
        (sizeof(test_data) + sizeof(packet_header_t)),
        send_length
    );

    uint8_t control_data[] = {
        // packet ID is 0 when there are no fragments 
        0x00, 
        // OUI (little E)
        0xAB, 0x3E, 0xED, 0xFE, 
        // device ID (little E)
        0xCD, 0xAB, 
        // MAC
        0xFE, 0xEF,
        // payload
        0xDE, 0xAD, 0xBE, 0xEF
    };


    for(uint i=0; i<send_length; i++){
        BYTES_EQUAL(control_data[i], send_buffer[i]);
    }
}

TEST(LongFiGroup, TwoFragmentPacket)
{
    uint8_t test_data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 
        0x19, 0x20};

    longfi_send(&longfi_handle, test_data, sizeof(test_data));
    
    // first packet is 24 bytes
    LONGS_EQUAL(
        24,
        send_length
    );

    uint8_t control_data1[] = {
        // packet ID 
        0xE0, 
        // fragment number
        0x00,
        // num fragments 
        0x02,
        // OUI (little E)
        0xAB, 0x3E, 0xED, 0xFE, 
        // device ID (little E)
        0xCD, 0xAB, 
        // MAC
        0xFE, 0xEF,
        // payload
        0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09
    };

    for(uint i=0; i<send_length; i++){
        BYTES_EQUAL(control_data1[i], send_buffer[i]);
    }
    // push an event into the driver
    longfi_handle_event(&longfi_handle, DIO0);

    uint8_t control_data2[] = {
        // packet ID 
        0xE0, 
        // fragment number
        0x01, 
        // MAC
        0xFE, 0xEF,
        // payload
        0x0a, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20
    };

    // first packet is 32 bytes
    LONGS_EQUAL(
        sizeof(fragment_header_t) + 12,
        send_length
    );

    for(uint i=0; i<send_length; i++){
        BYTES_EQUAL(control_data2[i], send_buffer[i]);
    }
}


#include "radio.h"
#include "mock-radio.h"

Mock_t Mock;

extern "C" {

static RadioEvents_t *radio_events;

void TestOnDio0Irq(void* context){
    radio_events->TxDone();
}
void TestOnDio1Irq(void* context){
    FAIL("dio1 should not fire!");

}
void TestOnDio2Irq(void* context){
    FAIL("dio2 should not fire!");

}
void TestOnDio3Irq(void* context){
    FAIL("dio3 should not fire!");

}
void TestOnDio4Irq(void* context){
    FAIL("dio4 should not fire!");
}

void TestOnDio5Irq(void* context){
    FAIL("dio5 should not fire!");
}

DioIrqHandler *DioIrqMock[] = { TestOnDio0Irq, TestOnDio1Irq,
                            TestOnDio2Irq, TestOnDio3Irq,
                            TestOnDio4Irq, TestOnDio5Irq };



void RadioInit( RadioEvents_t *events ){
    radio_events = events;
    IoIrqInit(DioIrqMock);
}

RadioState_t RadioGetStatus( void ){
    return RF_IDLE;
}

void RadioSetModem( RadioModems_t modem ){}

void RadioSetChannel( uint32_t freq ){}

bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime ){
    return true;
}

uint32_t RadioRandom( void ){
    return 0xE0;
}

void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                          uint32_t datarate, uint8_t coderate,
                          uint32_t bandwidthAfc, uint16_t preambleLen,
                          uint16_t symbTimeout, bool fixLen,
                          uint8_t payloadLen,
                          bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                          bool iqInverted, bool rxContinuous ){}

void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                          uint32_t bandwidth, uint32_t datarate,
                          uint8_t coderate, uint16_t preambleLen,
                          bool fixLen, bool crcOn, bool FreqHopOn,
                          uint8_t HopPeriod, bool iqInverted, uint32_t timeout ){}

bool RadioCheckRfFrequency( uint32_t frequency ){ return true; }

uint32_t RadioTimeOnAir( RadioModems_t modem, uint8_t pktLen ) { return 10; } 
void RadioSend( uint8_t *buffer, uint8_t size ){
    send_length = size;
    send_buffer = buffer;

    // debug prints
    printf("\r\n");
    for (int i = 0; i < size; i++){
        printf("%x ", buffer[i]);
    }
    printf("\r\n");
}

void RadioSleep( void ){}

void RadioStandby( void ){}

void RadioRx( uint32_t timeout ){}

void RadioStartCad( void ){}

void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time ){}

int16_t RadioRssi( RadioModems_t modem ) { return -120; }

void RadioWrite( uint16_t addr, uint8_t data ){}

uint8_t RadioRead( uint16_t addr ){ return 0x5e; }

void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size ){}

void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size ){}

void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max ){}

void RadioSetPublicNetwork( bool enable ){}

uint32_t RadioGetWakeupTime( void ) { return 5; }

void RadioIrqProcess( void ) {}

void RadioRxBoosted( uint32_t timeout ) {}

void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime ){}
}