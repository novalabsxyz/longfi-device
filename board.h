#ifndef _BOARD_BINDINGS
#define _BOARD_BINDINGS

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_IRQ_HANDLES 6

/*!
 * Hardware IO IRQ callback function definition
 */
typedef void ( DioIrqHandler )( void*);

typedef struct {
    void* Instance;
    void* port;
} LF_Gpio_t;

typedef struct {
	void* Instance;
} LF_SPI_HandleTypeDef;

typedef struct {
    LF_SPI_HandleTypeDef Spi;
    LF_Gpio_t Mosi;
    LF_Gpio_t Miso;
    LF_Gpio_t Sclk;
    LF_Gpio_t Nss;
} LF_Spi_t;

typedef enum
{
	LF_RADIO_ANT_SWITCH_LF,        //SX1276 board
	LF_RADIO_ANT_SWITCH_HF,        //SX1276 board /* TODO: Are all these different names
    LF_RADIO_ANT_SWITCH_POWER,     //SX126x board /*       but for the same function??
    LF_RADIO_SWITCH_CTRL1,         //SX1272 board
    LF_RADIO_SWITCH_CTRL2,         //SX1272 board
    LF_RADIO_PWRAMP_CTRL,          //SX1272 board
    LF_RADIO_NSS,
    LF_RADIO_BUSY,
    LF_RADIO_DEVICE_SEL,
    LF_RADIO_DIO_0,
    LF_RADIO_DIO_1,
    LF_RADIO_DIO_2,
    LF_RADIO_DIO_3,
    LF_RADIO_DIO_4,
    LF_RADIO_DIO_5,
    LF_RADIO_RESET,
    // Not connected
    LF_NC = (int)0xFFFFFFFF
}PinNames;


typedef enum
{
    LF_PIN_INPUT = 0,
    LF_PIN_OUTPUT,
    LF_PIN_ALTERNATE_FCT,
    LF_PIN_ANALOGIC
} PinModes;

typedef enum
{
    LF_PIN_PUSH_PULL = 0,
    LF_PIN_OPEN_DRAIN
} PinConfigs;

typedef enum
{
    LF_PIN_NO_PULL = 0,
    LF_PIN_PULL_UP,
    LF_PIN_PULL_DOWN
} PinTypes;

void GpioInit( LF_Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value );
void GpioWrite( LF_Gpio_t *obj, uint32_t value );
uint32_t GpioRead( LF_Gpio_t *obj );


typedef void( IrqHandler )( void* );
typedef void( GpioIrqHandler )( void* );

typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE
} IrqModes;

typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 0,
    IRQ_LOW_PRIORITY,
    IRQ_MEDIUM_PRIORITY,
    IRQ_HIGH_PRIORITY,
    IRQ_VERY_HIGH_PRIORITY
} IrqPriorities;


void GpioSetInterrupt( LF_Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler);


/*!
 * \brief Timer object description
 */
typedef struct TimerEvent_s
{
    uint32_t Timestamp;         //! Current timer value
    uint32_t ReloadValue;       //! Timer delay value
    bool IsRunning;             //! Is the timer currently running
    void ( *Callback )( void ); //! Timer IRQ callback function
    struct TimerEvent_s *Next;  //! Pointer to the next Timer object.
}TimerEvent_t;

#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif


void IoIrqInit(IrqHandler* irq_handlers[NUM_IRQ_HANDLES]);

void TimerInit( TimerEvent_t *obj, void ( *callback )( void * context) );
void TimerIrqHandler( void );
void TimerStart( TimerEvent_t *obj );
void TimerStop( TimerEvent_t *obj );
void TimerReset( TimerEvent_t *obj );
void TimerSetValue( TimerEvent_t *obj, uint32_t value );
TimerTime_t TimerGetCurrentTime( void );
TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime );
TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture );
void TimerLowPowerHandler( void );


void DelayMs( uint32_t ms );
void Delay( float s );

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size );

uint16_t SpiInOut( LF_Spi_t *obj, uint16_t outData );


// Above is everything that SX127x's depend on
// Rather then requiring statically linked functions
// We will change to runtime loaded
#include "radio/radio.h"
typedef struct Radio_s Radio_t;

typedef struct {
	uint16_t (*spi_in_out)(LF_Spi_t *obj, uint16_t outData);
	void (*gpio_init)(LF_Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value);
	void (*gpio_write)(LF_Gpio_t *obj, uint32_t value);
    uint32_t (*gpio_read)(LF_Gpio_t *obj);
	void (*gpio_set_interrupt)( LF_Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler);
	void (*delay_ms)(uint32_t);
} BoardBindings_t;

extern BoardBindings_t * bindings;

#ifdef __cplusplus
}
#endif

#endif /* _EMBEDDED_HAL_BINDINGS */

/* ----- */
