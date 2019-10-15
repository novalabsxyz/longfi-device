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
static uint8_t selected_power = 0;

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
    if(status) {
        if( bindings->set_antenna_pins!= NULL ){
            (*bindings->set_antenna_pins)(AntModeSleep, 0);
        }
    }
    RadioIsActive = status;
}

void SX1276AntSwInit( void )
{
}

void SX1276AntSwDeInit( void )
{
}

void SX1276SetAntSw( uint8_t opMode )
{

    if( bindings->set_antenna_pins!= NULL ){
        switch( opMode )
        {
        case SX1276_RFLR_OPMODE_SLEEP:
            (*bindings->set_antenna_pins)(AntModeSleep, 0);
            break;
        case SX1276_RFLR_OPMODE_TRANSMITTER:
            (*bindings->set_antenna_pins)(AntModeTx, selected_power);
            break;
        default:
            (*bindings->set_antenna_pins)(AntModeRx, 0);
            break;
        }
    }

}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    return true;
}

void SX1276Reset( )
{
    // if user has given board_tcxo pointer
    // enable it
    if(bindings->set_board_tcxo!=NULL){
        (*bindings->reset)(true);
        (*bindings->delay_ms)(1);
        (*bindings->reset)(false);
        uint8_t osc_setup_time = (*bindings->set_board_tcxo)(true);
        (*bindings->delay_ms)(osc_setup_time);
        SX1276EnableTcxo();
    };

    // reset required, even after TCXO enabling routine
    (*bindings->reset)(true);
    (*bindings->delay_ms)(1);
    (*bindings->reset)(false);
}


void SX1276SetRfTxPower( int8_t power ){
    if( bindings->reduce_power!= NULL ){
        power -= (*bindings->reduce_power)(power);
    }
    power = power;
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
