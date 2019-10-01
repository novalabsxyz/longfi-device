/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX1276 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "sx1276.h"
#include "sx1276-board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Antenna switch GPIO pins objects
 */
LF_Gpio_t AntSwitchLf;
LF_Gpio_t AntSwitchHf;

void SX1276IoInit( void )
{
}

void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
    IoIrqInit(irqHandlers);
}

void SX1276IoDeInit( void )
{
}

uint8_t SX1276GetPaSelect( uint32_t channel )
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

void SX1276SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;
    
        if( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

void SX1276AntSwInit( void )
{
}

void SX1276AntSwDeInit( void )
{
}

void SX1276SetAntSw( uint8_t rxTx )
{

}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    return true;
}

void SX1276Reset( )
{
}


void SX1276SetRfTxPower( int8_t power ){
    // uint8_t paConfig, paDac;

    // if( power < -4 )
    // {
    //     power = -4;
    // }
    // if( power > 20 )
    // {
    //     power = 20;
    // }

    // if( power > 15 )
    // {
    //     if( power > 17 )
    //     {
    //         paConfig = ( RF_PACONFIG_PASELECT_PABOOST | ( power - 5 ) );
    //         paDac = RF_PADAC_20DBM_ON;
    //     }
    //     else
    //     {
    //         paConfig = ( RF_PACONFIG_PASELECT_PABOOST | ( power - 2 ) );
    //         paDac = RF_PADAC_20DBM_OFF;
    //     }
    // }
    // else
    // {
    //     if( power > 0 )
    //     {
    //         paConfig = ( RF_PACONFIG_PASELECT_RFO | ( 7 << 4 ) | ( power ) );
    //         paDac = RF_PADAC_20DBM_OFF;
    //     }
    //     else
    //     {
    //         paConfig = ( RF_PACONFIG_PASELECT_RFO | ( 0 << 4 ) | ( power + 4 ) );
    //         paDac = RF_PADAC_20DBM_OFF;
    //     }
    // }

    // SX1276Write( REG_PACONFIG, paConfig );

    //Pa maximum power
    SX1276Write(SX1276_REG_PACONFIG, 0xFF);
    
    //150% LNA current & maximum gain
    SX1276Write(SX1276_REG_LNA, 0x23);

    //PaDac 20dBm on PA_BOOST
    SX1276Write(SX1276_REG_PADAC, 0x9F);
    
    //Set RegOcp 240ma
    SX1276Write(SX1276_REG_OCP, 0x3B);

}

void SX1276SetBoardTcxo(bool enable){}

uint32_t SX1276GetBoardTcxoWakeupTime( void ){
    return 0;
}
