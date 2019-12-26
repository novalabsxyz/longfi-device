#ifndef __LONGFI_H__
#define __LONGFI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "board.h"
#include "lfc/lfc.h"
#include "radio/radio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


    /*!
     * Authentication modes
     */
    typedef enum
    {
        PresharedKey128,
	_LongFiAuthModeMax = 0xFFFFFFFF // fore 32-bit value
        // TODO: ECDH
    } LongFiAuthMode_t;

    /*!
     * Configure device with necessary network routing information
     */
    typedef struct
    {
        uint32_t         oui;       // organizations unique identifier
        uint16_t         device_id; // device identifier within organization
        LongFiAuthMode_t auth_mode;
    } LongFiConfig_t;

    /*!
     * Configure
     */
    union LongFiAuthCallbacks {
        const uint8_t * preshared_key;
        // other auth types will have other callbacks
    };

    /*!
     * LongFi handler for library
     */
    typedef struct
    {
        Radio_t * radio; // pointer to struct of SX12XX radio functions
        BoardBindings_t *
                                  bindings; // pointer to struct of system bindings, defined in board.h
        LongFiConfig_t            config;
        union LongFiAuthCallbacks auth_cb;
        struct lfc                lfc;
    } LongFi_t;

    /*!
     * \brief  Constructor for LongFi handle
     *
     * \param [IN] bindings  System bindings for radio driver
     * \param [IN] radio     Handle to radio struct
     * \param [IN] config    RF Network Configuration
     */
    LongFi_t longfi_new_handle(BoardBindings_t *         bindings,
                               Radio_t *                 radio,
                               LongFiConfig_t            config,
                               union LongFiAuthCallbacks auth_cb);

    /*!
     * \brief  Run time initialization of library
     *
     * \param [IN] handle
     */
    void longfi_init(LongFi_t * handle);

    /*!
     * Events to be handle by client
     */
    typedef enum ClientEvent_t
    {
        ClientEvent_None,   // this is a non-event, no handling required
        ClientEvent_TxDone, // the full transmit is complete (1 or more fragments)
        ClientEvent_Rx,     // a full packet was received
    } ClientEvent_t;

    /*!
     * \brief Provide ownership of a buffer to the library. Sets maximum transmit and receive
     *
     * \param [IN] handle
     * \param [IN] buffer        A pointer to the memory
     * \param [IN] buffer_len    Size of memory in bytes
     */
    void longfi_set_buf(LongFi_t * handle, uint8_t * buffer, size_t buffer_len);


    /*!
     * \brief  Dispatches a packet to the protocol. It is not safe to use this
     * function again without waiting for a ClientEvent_TxDone
     *
     * \param [IN] handle
     * \param [IN] buffer        A pointer to the memory
     * \param [IN] buffer_len    Size of memory in bytes
     */
    void longfi_send(LongFi_t * handle, const uint8_t * data, size_t len);

    /*!
     * The structure of a received packet
     */
    typedef struct RxPacket_t
    {
        uint8_t * buf;
        size_t    len;
        int16_t   rssi;
        int8_t    snr;
    } RxPacket_t;

    /*!
     * \brief Gets a downlink packet. This is to be called after a ClientEvent_Rx
     *
     * \retval RxPacket_t returns a received packet, including ownership
     * Give a buffer back to LongFi using longfi_set_buf to send or receive again
     */
    RxPacket_t longfi_get_rx();

    /*!
     * These are system generated events that the client must collect and push
     * into longfi_handle_event. All the DIO events are pin interrupts
     */
    typedef enum RfEvent_t
    {
        RFE_DIO0,   // TxDone or Rx
        RFE_DIO1,   // unimplemented
        RFE_DIO2,   // unimplemented
        RFE_DIO3,   // unimplemented
        RFE_DIO4,   // unimplemented
        RFE_DIO5,   // unimplemented
        RFE_Timer1, // unimplemented
        RFE_Timer2, // unimplemented
        RFE_Timer3  // unimplemented
    } RfEvent_t;

    /*!
     * \brief To be used by client in a low-priorty loop, feeding events into the library
     *
     * \param [IN] LongFi_t
     * \param [IN] RfEvent_t   A system generated event
     *
     * \retval ClientEvent_t should be handled by a client
     */
    ClientEvent_t longfi_handle_event(LongFi_t * handle, RfEvent_t rf_event);

    /*!
     * \brief Continuously sends a byte byte (0xAB) at 910MHz.
     * Useful for setting of RF hardware test
     *
     * \param [IN] LongFi_t
     */
    void longfi_rf_test(LongFi_t * handle);

    extern const struct Radio_s Radio;

#ifdef __cplusplus
}
#endif

#endif // __LONGFI_H__
