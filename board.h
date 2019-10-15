#ifndef _BOARD_BINDINGS
#define _BOARD_BINDINGS

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUM_IRQ_HANDLES 6

    /*!
     * Hardware IO IRQ callback function definition
     */
    typedef void(DioIrqHandler)(void *);

    typedef struct
    {
        void * Instance;
        void * port;
    } LF_Gpio_t;

    typedef struct
    {
        void * Instance;
    } LF_SPI_HandleTypeDef;

    typedef struct
    {
        LF_SPI_HandleTypeDef Spi;
        LF_Gpio_t            Nss;
    } LF_Spi_t;

    void     GpioWrite(LF_Gpio_t * obj, uint32_t value);
    uint32_t GpioRead(LF_Gpio_t * obj);

    typedef enum AntPinsMode_t
    {
        AntModeTx,
        AntModeRx,
        AntModeSleep,
        _AntModeMax = 0xFFFFFFFF // force 32-bit value
    } AntPinsMode_t;

    typedef void(IrqHandler)(void *);
    typedef void(GpioIrqHandler)(void *);

    /*!
     * \brief Timer object description
     */
    typedef struct TimerEvent_s
    {
        uint32_t Timestamp;         //! Current timer value
        uint32_t ReloadValue;       //! Timer delay value
        bool     IsRunning;         //! Is the timer currently running
        void (*Callback)(void);     //! Timer IRQ callback function
        struct TimerEvent_s * Next; //! Pointer to the next Timer object.
    } TimerEvent_t;

#ifndef TimerTime_t
    typedef uint32_t TimerTime_t;
#endif

    void IoIrqInit(IrqHandler * irq_handlers[NUM_IRQ_HANDLES]);

    void        TimerInit(TimerEvent_t * obj, void (*callback)(void * context));
    void        TimerIrqHandler(void);
    void        TimerStart(TimerEvent_t * obj);
    void        TimerStop(TimerEvent_t * obj);
    void        TimerReset(TimerEvent_t * obj);
    void        TimerSetValue(TimerEvent_t * obj, uint32_t value);
    TimerTime_t TimerGetCurrentTime(void);
    TimerTime_t TimerGetElapsedTime(TimerTime_t savedTime);
    TimerTime_t TimerGetFutureTime(TimerTime_t eventInFuture);
    void        TimerLowPowerHandler(void);

    void DelayMs(uint32_t ms);
    void Delay(float s);

    void memcpy1(uint8_t * dst, const uint8_t * src, uint16_t size);

    uint16_t SpiInOut(LF_Spi_t * obj, uint16_t outData);

// Above is everything that SX127x's depend on
// Rather then requiring statically linked functions
// We will change to runtime loaded
#include "radio/radio.h"
    typedef struct Radio_s Radio_t;

    typedef struct
    {
        // must provide for drivers to work
        uint8_t (*spi_in_out)(LF_Spi_t * obj, uint8_t outData);
        void (*spi_nss)(bool sel);
        void (*reset)(bool enable);
        void (*delay_ms)(uint32_t);
        uint32_t (*get_random_bits)(uint8_t);
        bool (*busy_pin_status)(); // only necessary for SX126x
                                   // optional board control
        // if external PA, this callback allows you to reduce power
        // of SX12xx and to do any PA switching
        uint8_t (*reduce_power)(uint8_t);
        uint8_t (*set_board_tcxo)(
            bool enable); // to control power supply TCXO (wake/sleep)
        void (*set_antenna_pins)(
            AntPinsMode_t mode,
            uint8_t       power); // to control antenna pins for TX/RX/Sleep

    } BoardBindings_t;

    extern BoardBindings_t * bindings;

#ifdef __cplusplus
}
#endif

#endif /* _EMBEDDED_HAL_BINDINGS */

/* ----- */
