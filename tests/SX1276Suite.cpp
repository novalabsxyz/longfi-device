#include <CppUTest/TestHarness.h>

#include "../longfi.h"
#include "../longfiP.h"
#include "../board.h"
#include "../radio/sx1276/sx1276.h"

RfConfig_t sx1276_rf_config = {
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
void gpio_set_interrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler){

}
void delay_ms(uint32_t){

}

static Radio_t sx1276 = SX1276RadioNew(); 
static LongFi_t longfi_handle;
static BoardBindings_t my_bindings {
  .spi_in_out = spi_in_out,
  .gpio_init = gpio_init,
  .gpio_write = gpio_write,
  .gpio_set_interrupt = gpio_set_interrupt,
  .delay_ms = delay_ms,
};






TEST_GROUP(Sx1276Group)
{
    void setup()
   {
    longfi_handle.radio = &sx1276;
    longfi_handle.config = sx1276_rf_config;
    longfi_handle.bindings = &my_bindings;
    //longfi_init(&longfi_handle);
   }

   void teardown()
   {
      
   }

};

TEST(Sx1276Group, SingleFragmentPacket)
{
  // can't actually exercise the code much without it getting stuck
    // uint8_t test_data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    //     0x07, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 
    //     0x19, 0x20};
    // longfi_send(&longfi_handle, LONGFI_QOS_0, test_data, sizeof(test_data));
    // longfi_handle_event(&longfi_handle, DIO0);
}
