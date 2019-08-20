#include "board.h"

BoardBindings_t * bindings = NULL;


void DelayMs( uint32_t ms )
{
	(*bindings->delay_ms)(ms);
}

void Delay( float s )
{
    DelayMs( s * 1000.0f );
}


void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size ){
	memcpy(dst, src, size);
}

void assert_param(bool expr){}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
	return (*bindings->spi_in_out)(obj, outData);
}

void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value ){
	(*bindings->gpio_init)(obj, pin, mode, config, type, value);
}

void GpioWrite( Gpio_t *obj, uint32_t value ){
	(*bindings->gpio_write)(obj, value);
}

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler){
	(*bindings->gpio_set_interrupt)(obj, irqMode, irqPriority, irqHandler);
}

void TimerInit( TimerEvent_t *obj, void ( *callback )( void*) ){
}
void TimerIrqHandler( void ){
}
void TimerStart( TimerEvent_t *obj ){
}
void TimerStop( TimerEvent_t *obj ){
}
void TimerReset( TimerEvent_t *obj ){
}
void TimerSetValue( TimerEvent_t *obj, uint32_t value) {
}
TimerTime_t TimerGetCurrentTime( void ){
	return 0;
}
TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime ){
	return 0;
}
TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture ){
	return 0;
}
void TimerLowPowerHandler( void ){
}

void board_set_bindings(
    BoardBindings_t * bindings,
    uint16_t (*spi_in_out)(Spi_t *obj, uint16_t outData),
    void (*gpio_init)(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value),
    void (*gpio_write)(Gpio_t *obj, uint32_t value),
    void (*gpio_set_interrupt)( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler),
    void (*delay_ms)(uint32_t)
    ){
	bindings->spi_in_out = spi_in_out;
	bindings->gpio_init = gpio_init;
	bindings->gpio_write = gpio_write;
	bindings->gpio_set_interrupt = gpio_set_interrupt;
	bindings->delay_ms = delay_ms;
}