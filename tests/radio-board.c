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
}

void MockAntSwDeInit( void )
{
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
