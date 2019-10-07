#include <CppUTest/TestHarness.h>

#include "../longfi.h"
#include "../longfiP.h"
#include "../board.h"
#include "../radio/sx1276/sx1276.h"
#include "../radio/sx1272/sx1272.h"
#include "../radio/sx126x/sx126x.h"


RfConfig_t radio_rf_config = {
    .oui = 0xFEED3EAB,
    .device_id = 0xABCD,
};

uint8_t spi_in_out(LF_Spi_t *obj, uint8_t outData){
  return 0x3E;
};


void spi_nss(bool not_select){
};


void delay_ms(uint32_t){
}

uint32_t get_random_bits(uint8_t){
  return 0xE0;
}

void radio_reset(bool){
}

static Radio_t sx1276;
static Radio_t sx1272;
static Radio_t sx126x; 

static LongFi_t longfi_handle;
BoardBindings_t my_bindings {
  .spi_in_out = spi_in_out,
  .spi_nss = spi_nss,
  .reset = radio_reset,
  .delay_ms = delay_ms,
  .get_random_bits = get_random_bits,
  .set_board_tcxo = NULL,
  .set_antenna_pins = NULL,
};

TEST_GROUP(RadioGroup)
{
    void setup()
   {
   }

   void teardown()
   {
      
   }

};

TEST(RadioGroup, SX1276)
{
  sx1276 = SX1276RadioNew();
  longfi_handle.bindings = &my_bindings;
  longfi_handle.radio = &sx1276;
}

TEST(RadioGroup, SX1272)
{
  sx1272 = SX1272RadioNew();
  longfi_handle.bindings = &my_bindings;
}

TEST(RadioGroup, SX126x)
{
  sx126x = SX126xRadioNew();
  longfi_handle.bindings = &my_bindings;
}
