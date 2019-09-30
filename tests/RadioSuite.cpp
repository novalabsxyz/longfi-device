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

void gpio_init(LF_Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value){
}

void gpio_write(LF_Gpio_t *obj, bool value){
}

bool gpio_read(LF_Gpio_t *obj){
  return false;
}
void gpio_set_interrupt( LF_Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler){
}
void delay_ms(uint32_t){
}

uint32_t get_random_bits(uint8_t){
  return 0xE0;
}

static Radio_t sx1276;
static Radio_t sx1272;
static Radio_t sx126x; 

static LongFi_t longfi_handle;
BoardBindings_t my_bindings {
  .spi_in_out = spi_in_out,
  .gpio_write = gpio_write,
  .gpio_read = gpio_read,
  .delay_ms = delay_ms,
  .get_random_bits = get_random_bits,
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
