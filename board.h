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
} Gpio_t;

typedef struct {
	void* Instance;
} SPI_HandleTypeDef;

typedef struct {
    SPI_HandleTypeDef Spi;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
} Spi_t;

typedef enum
{
	RADIO_ANT_SWITCH_LF,        //SX1276 board
	RADIO_ANT_SWITCH_HF,        //SX1276 board /* TODO: Are all these different names
    RADIO_ANT_SWITCH_POWER,     //SX126x board /*       but for the same function??
    RADIO_SWITCH_CTRL1,         //SX1272 board
    RADIO_SWITCH_CTRL2,         //SX1272 board
    RADIO_PWRAMP_CTRL,          //SX1272 board
    RADIO_NSS,
    RADIO_BUSY,
    RADIO_DEVICE_SEL,
    RADIO_DIO_0,
    RADIO_DIO_1,
    RADIO_DIO_2,
    RADIO_DIO_3,
    RADIO_DIO_4,
    RADIO_DIO_5,
    RADIO_RESET,
    // Not connected
    NC = (int)0xFFFFFFFF
}PinNames;


typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
} PinModes;

typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
} PinConfigs;

typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
} PinTypes;

void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value );
void GpioWrite( Gpio_t *obj, uint32_t value );
uint32_t GpioRead( Gpio_t *obj );


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


void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler);


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

void assert_param(bool expr);

uint16_t SpiInOut( Spi_t *obj, uint16_t outData );


// Above is everything that SX127x's depend on
// Rather then requiring statically linked functions
// We will change to runtime loaded
#include "radio/radio.h"
typedef struct Radio_s Radio_t;

typedef struct {
	uint16_t (*spi_in_out)(Spi_t *obj, uint16_t outData);
	void (*gpio_init)(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value);
	void (*gpio_write)(Gpio_t *obj, uint32_t value);
    uint32_t (*gpio_read)(Gpio_t *obj);
	void (*gpio_set_interrupt)( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler);
	void (*delay_ms)(uint32_t);
} BoardBindings_t;

extern BoardBindings_t * bindings;

#ifdef __cplusplus
}
#endif

#endif /* _EMBEDDED_HAL_BINDINGS */

/* ----- */
