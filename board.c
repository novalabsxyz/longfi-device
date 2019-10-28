#include "board.h"

BoardBindings_t * bindings = NULL;

void
DelayMs(uint32_t ms)
{
    (*bindings->delay_ms)(ms);
}

void
Delay(float s)
{
    DelayMs(s * 1000.0f);
}

void
memcpy1(uint8_t * dst, const uint8_t * src, uint16_t size)
{
    memcpy(dst, src, size);
}

uint16_t
SpiInOut(LF_Spi_t * obj, uint16_t outData)
{
    uint16_t ret = 0;
    ret |= (*bindings->spi_in_out)((uint8_t)outData);
    return ret;
}

void
GpioWrite(LF_Gpio_t * obj, uint32_t value)
{
    (*bindings->spi_nss)((bool)value);
}

uint32_t
GpioRead(LF_Gpio_t * obj)
{
    return 0;
}

void
TimerInit(TimerEvent_t * obj, void (*callback)(void *))
{
}
void
TimerIrqHandler(void)
{
}
void
TimerStart(TimerEvent_t * obj)
{
}
void
TimerStop(TimerEvent_t * obj)
{
}
void
TimerReset(TimerEvent_t * obj)
{
}
void
TimerSetValue(TimerEvent_t * obj, uint32_t value)
{
}
TimerTime_t
TimerGetCurrentTime(void)
{
    return 0;
}
TimerTime_t
TimerGetElapsedTime(TimerTime_t savedTime)
{
    return 0;
}
TimerTime_t
TimerGetFutureTime(TimerTime_t eventInFuture)
{
    return 0;
}
void
TimerLowPowerHandler(void)
{
}
