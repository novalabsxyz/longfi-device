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

uint16_t spi_in_out(Spi_t *obj, uint16_t outData){
  return 0x3E;
};
void gpio_init(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value){

}
void gpio_write(Gpio_t *obj, uint32_t value){

}

uint32_t gpio_read(Gpio_t *obj){
  return 0;
}
void gpio_set_interrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler){

}
void delay_ms(uint32_t){

}

static Radio_t sx1276;
static Radio_t sx1272;
static Radio_t sx126x; 

static LongFi_t longfi_handle;
static BoardBindings_t my_bindings {
  .spi_in_out = spi_in_out,
  .gpio_init = gpio_init,
  .gpio_write = gpio_write,
  .gpio_read = gpio_read,
  .gpio_set_interrupt = gpio_set_interrupt,
  .delay_ms = delay_ms,
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
