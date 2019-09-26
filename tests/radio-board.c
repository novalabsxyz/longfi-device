#include "board.h"
#include "radio.h"
#include "radio-board.h"
#include "mock-radio.h"
/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Antenna switch GPIO pins objects
 */
LF_Gpio_t AntSwitchLf;
LF_Gpio_t AntSwitchHf;

void MockIoInit( void )
{
    GpioInit( &Mock.Spi.Nss, LF_RADIO_NSS, LF_PIN_OUTPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 1 );

    GpioInit( &Mock.DIO0, LF_RADIO_DIO_0, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
    GpioInit( &Mock.DIO1, LF_RADIO_DIO_1, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
    GpioInit( &Mock.DIO2, LF_RADIO_DIO_2, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
    GpioInit( &Mock.DIO3, LF_RADIO_DIO_3, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
    GpioInit( &Mock.DIO4, LF_RADIO_DIO_4, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
    GpioInit( &Mock.DIO5, LF_RADIO_DIO_5, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
}

void MockIoIrqInit( IrqHandler **irqHandlers )
{
    GpioSetInterrupt( &Mock.DIO0, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[0] );
    GpioSetInterrupt( &Mock.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[1] );
    GpioSetInterrupt( &Mock.DIO2, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[2] );
    GpioSetInterrupt( &Mock.DIO3, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[3] );
    GpioSetInterrupt( &Mock.DIO4, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[4] );
    GpioSetInterrupt( &Mock.DIO5, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[5] );
}

void MockIoDeInit( void )
{
    GpioInit( &Mock.Spi.Nss, LF_RADIO_NSS, LF_PIN_OUTPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 1 );

    GpioInit( &Mock.DIO0, LF_RADIO_DIO_0, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
    GpioInit( &Mock.DIO1, LF_RADIO_DIO_1, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
    GpioInit( &Mock.DIO2, LF_RADIO_DIO_2, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
    GpioInit( &Mock.DIO3, LF_RADIO_DIO_3, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
    GpioInit( &Mock.DIO4, LF_RADIO_DIO_4, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
    GpioInit( &Mock.DIO5, LF_RADIO_DIO_5, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 0 );
}

uint8_t MockGetPaSelect( uint32_t channel )
{
    if( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

void MockSetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;
    
        if( status == false )
        {
            MockAntSwInit( );
        }
        else
        {
            MockAntSwDeInit( );
        }
    }
}

void MockAntSwInit( void )
{
    GpioInit( &AntSwitchLf, LF_RADIO_ANT_SWITCH_LF, LF_PIN_OUTPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 1 );
    GpioInit( &AntSwitchHf, LF_RADIO_ANT_SWITCH_HF, LF_PIN_OUTPUT, LF_PIN_PUSH_PULL, LF_PIN_PULL_UP, 0 );
}

void MockAntSwDeInit( void )
{
    GpioInit( &AntSwitchLf, LF_RADIO_ANT_SWITCH_LF, LF_PIN_OUTPUT, LF_PIN_OPEN_DRAIN, LF_PIN_NO_PULL, 0 );
    GpioInit( &AntSwitchHf, LF_RADIO_ANT_SWITCH_HF, LF_PIN_OUTPUT, LF_PIN_OPEN_DRAIN, LF_PIN_NO_PULL, 0 );
}

void MockSetAntSw( uint8_t rxTx )
{
    if( rxTx != 0 ) // 1: TX, 0: RX
    {
        GpioWrite( &AntSwitchLf, 0 );
        GpioWrite( &AntSwitchHf, 1 );
    }
    else
    {
        GpioWrite( &AntSwitchLf, 1 );
        GpioWrite( &AntSwitchHf, 0 );
    }
}

bool MockCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
