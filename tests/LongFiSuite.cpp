#include <CppUTest/TestHarness.h>

#include "../longfi.h"
#include "../longfiP.h"

struct RfConfig rf_config = {
    .always_on = false,
    .network_poll = 1000,
    .oui = 0xFEED3EAB,
    .device_id = 0xABCD,
};


TEST_GROUP(LongFiGroup)
{
    void setup()
   {
      longfi_init(rf_config);
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
    longfi_send(LONGFI_QOS_0, test_data, sizeof(test_data));
    
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

TEST(LongFiGroup, MultipleFragmentPacket)
{
    uint8_t test_data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 
        0x19, 0x20};

    longfi_send(LONGFI_QOS_0, test_data, sizeof(test_data));
    
    // first packet is 32 bytes
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
        //if (i!= 6) {
            BYTES_EQUAL(control_data1[i], send_buffer[i]);
        //}
    }
    // push an event into the driver
    longfi_handle_event(DIO0);

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

#include "../board.h"
#include "../radio/radio.h"
#include "../radio/sx1276/sx1276.h"
#include <cstdio>

extern "C" {

    static RadioEvents_t *RadioEvents;

    void TestOnDio0Irq(void){
        RadioEvents->TxDone();
    }
    void TestOnDio1Irq(void){

    }
    void TestOnDio2Irq(void){

    }
    void TestOnDio3Irq(void){

    }
    void TestOnDio4Irq(void){

    }

    void TestOnDio5Irq( void ){
        FAIL("dio5 should not fire!");
    }


    DioIrqHandler *DioIrq[] = { TestOnDio0Irq, TestOnDio1Irq,
                                TestOnDio2Irq, TestOnDio3Irq,
                                TestOnDio4Irq, TestOnDio5Irq };

    void SX1276Write( uint8_t addr, uint8_t data ){
    }

    uint8_t SX1276Read( uint8_t addr ){
        return 0;
    }

    uint32_t SX1276Random( void ){
        return 0xE0;
    }

    void SX1276SetChannel( uint32_t freq ){

    }

    void SX1276Init( RadioEvents_t *events ){
        RadioEvents = events;
        SX1276IoIrqInit(DioIrq);

    }

    void SX1276SetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev, 
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool FreqHopOn,
                        uint8_t HopPeriod, bool iqInverted, uint32_t timeout ){

    }

    void SX1276SetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                         bool iqInverted, bool rxContinuous ){

    }

    void SX1276SetRx( uint32_t timeout ){

    }

    void SX1276Send( uint8_t *buffer, uint8_t size ){
        send_length = size;
        send_buffer = buffer;

        // debug prints
        printf("\r\n");
        for (int i = 0; i < size; i++){
            printf("%x ", buffer[i]);
        }
        printf("\r\n");
    }

    bool SX1276IsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh ){
        return true;
    }
}
