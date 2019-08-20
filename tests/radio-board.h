#ifndef __MOCK_ARCH_H__
#define __MOCK_ARCH_H__

#include "mock-radio.h"

Radio_t MockRadioNew(){
    Radio_t radio = {
        RadioInit,
    RadioGetStatus,
    RadioSetModem,
    RadioSetChannel,
    RadioIsChannelFree,
    RadioRandom,
    RadioSetRxConfig,
    RadioSetTxConfig,
    RadioCheckRfFrequency,
    RadioTimeOnAir,
    RadioSend,
    RadioSleep,
    RadioStandby,
    RadioRx,
    RadioStartCad,
    RadioSetTxContinuousWave,
    RadioRssi,
    RadioWrite,
    RadioRead,
    RadioWriteBuffer,
    RadioReadBuffer,
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeupTime,
    RadioIrqProcess,
    // Available on SX126x only
    RadioRxBoosted,
    RadioSetRxDutyCycle
    };
    return radio;
}

/*!
 * \brief Initializes the radio I/Os pins interface
 */
void MockIoInit( void );

/*!
 * \brief Initializes DIO IRQ handlers
 *
 * \param [IN] irqHandlers Array containing the IRQ callback functions
 */
//void MockIoIrqInit( IrqHandler **irqHandlers );

/*!
 * \brief De-initializes the radio I/Os pins interface. 
 *
 * \remark Useful when going in MCU lowpower modes
 */
void MockIoDeInit( void );

/*!
 * \brief Gets the board PA selection configuration
 *
 * \param [IN] channel Channel frequency in Hz
 * \retval PaSelect RegPaConfig PaSelect value
 */
uint8_t MockGetPaSelect( uint32_t channel );

/*!
 * \brief Set the RF Switch I/Os pins in Low Power mode
 *
 * \param [IN] status enable or disable
 */
void MockSetAntSwLowPower( bool status );

/*!
 * \brief Initializes the RF Switch I/Os pins interface
 */
void MockAntSwInit( void );

/*!
 * \brief De-initializes the RF Switch I/Os pins interface 
 *
 * \remark Needed to decrease the power consumption in MCU lowpower modes
 */
void MockAntSwDeInit( void );

/*!
 * \brief Controls the antena switch if necessary.
 *
 * \remark see errata note
 *
 * \param [IN] rxTx [1: Tx, 0: Rx]
 */
void MockSetAntSw( uint8_t rxTx );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
bool MockCheckRfFrequency( uint32_t frequency );


/*!
 * Radio hardware and global parameters
 */
extern Mock_t Mock;

#endif // __Mock_ARCH_H__
