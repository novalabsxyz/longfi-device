/*!
 * \file      sx1272-board.c
 *
 * \brief     Target board SX1272 driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include "sx1272.h"
#include "sx1272-board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/* !
 *  Radio hardware and global parameters defined in sx1272.c
 */
extern SX1272_t SX1272;

/*!
 * Antenna switch GPIO pins objects
 */
LF_Gpio_t RadioSwitchCtrl1;
LF_Gpio_t RadioSwitchCtrl2;
LF_Gpio_t RadioPwrAmpCtrl;

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

/*
       PD_2=0  PD_2=1
op PaB  rfo     rfo
0  4.6  18.5    27.0
1  5.6  21.1    28.1
2  6.7  23.3    29.1
3  7.7  25.3    30.1
4  8.8  26.2    30.7
5  9.8  27.3    31.2
6  10.7 28.1    31.6
7  11.7 28.6    32.2
8  12.8 29.2    32.4
9  13.7 29.9    32.9
10 14.7 30.5    33.1
11 15.6 30.8    33.4
12 16.4 30.9    33.6
13 17.1 31.0    33.7
14 17.8 31.1    33.7
15 18.4 31.1    33.7
*/
//                           txpow:   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14  15  16  17  18  19
static const uint8_t PaBTable[20] = { 0, 0, 0, 0, 0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15 };

//                           txpow:  20 21 22 23 24 25 26 27 28 29 30
static const uint8_t RfoTable[11] = { 1, 1, 1, 2, 2, 3, 4, 5, 6, 8, 9 };

void SX1272IoInit( void )
{
}

void SX1272IoIrqInit( DioIrqHandler **irqHandlers )
{
    IoIrqInit(irqHandlers);
}

void SX1272IoDeInit( void )
{
}

void SX1272IoDbgInit( void )
{
}

void SX1272IoTcxoInit( void )
{
    // No TCXO component available on this board design.
}

void SX1272SetBoardTcxo( uint8_t state )
{
    // No TCXO component available on this board design.
#if 0
    if( state == true )
    {
        TCXO_ON( );
        DelayMs( BOARD_TCXO_WAKEUP_TIME );
    }
    else
    {
        TCXO_OFF( );
    }
#endif
}

uint32_t SX1272GetBoardTcxoWakeupTime( void )
{
    return SX1272_BOARD_TCXO_WAKEUP_TIME;
}

void SX1272Reset( void )
{
    // Enables the TCXO if available on the board design
    SX1272SetBoardTcxo( true );

    // Set RESET pin to 1
    //GpioInit( &SX1272.Reset, LF_RADIO_RESET, LF_PIN_OUTPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 1 );

    // Wait 1 ms
    DelayMs( 1 );

    // Configure RESET as input
    //GpioInit( &SX1272.Reset, LF_RADIO_RESET, LF_PIN_INPUT, LF_PIN_PUSH_PULL, LF_PIN_NO_PULL, 1 );

    // Wait 6 ms
    DelayMs( 6 );
}



void SX1272SetRfTxPower( int8_t power )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1272Read( SX1272_REG_PACONFIG );
    paDac = SX1272Read( SX1272_REG_PADAC );

    // get rid of the MIN macro
    if( power < 0 ) {
        power = 0;
    }

    if( power > 19 )
    {
        paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | RF_PACONFIG_PASELECT_RFO;
        paConfig = ( paConfig & SX1272_RFLR_PACONFIG_OUTPUTPOWER_MASK ) | RfoTable[power - 20];
    }
    else
    {
        paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | RF_PACONFIG_PASELECT_PABOOST;
        paConfig = ( paConfig & SX1272_RFLR_PACONFIG_OUTPUTPOWER_MASK ) | PaBTable[power];
    }
    SX1272Write( SX1272_REG_PACONFIG, paConfig );
    SX1272Write( SX1272_REG_PADAC, paDac );
}

void SX1272SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;

        if( status == false )
        {
            SX1272AntSwInit( );
        }
        else
        {
            SX1272AntSwDeInit( );
        }
    }
}

void SX1272AntSwInit( void )
{
}

void SX1272AntSwDeInit( void )
{
}

void SX1272SetAntSw( uint8_t opMode )
{
    switch( opMode )
    {
    case SX1272_RFLR_OPMODE_TRANSMITTER:
        if( ( SX1272Read( SX1272_REG_PACONFIG ) & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
        {
            GpioWrite( &RadioSwitchCtrl1, 1 );
            GpioWrite( &RadioSwitchCtrl2, 0 );
        }
        else
        {
            GpioWrite( &RadioSwitchCtrl1, 0 );
            GpioWrite( &RadioSwitchCtrl2, 1 );
        }
        break;
    case SX1272_RFLR_OPMODE_RECEIVER:
    case SX1272_RFLR_OPMODE_RECEIVER_SINGLE:
    case SX1272_RFLR_OPMODE_CAD:
        GpioWrite( &RadioSwitchCtrl1, 1 );
        GpioWrite( &RadioSwitchCtrl2, 1 );
        break;
    default:
        GpioWrite( &RadioSwitchCtrl1, 0 );
        GpioWrite( &RadioSwitchCtrl2, 0 );
        GpioWrite( &RadioPwrAmpCtrl, 0 );
        break;
    }
}

bool SX1272CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

#if defined( USE_RADIO_DEBUG )
void SX1272DbgPinTxWrite( uint8_t state )
{
    GpioWrite( &DbgPinTx, state );
}

void SX1272DbgPinRxWrite( uint8_t state )
{
    GpioWrite( &DbgPinRx, state );
}
#endif
