/**
 * @file network_interface_zephyr.h
 * @brief Network interface definitions to send and receive data over the
 * network via UDP in ZEPHYR OS.
 */


#include "udp_interface.h"

/**
 * @typedef OSNetwork_t
 */
struct OSNetwork
{
    int os_socket;
};

/**
 * @brief Establishes a Network connection to a specified endpoint.
 *
 * @param osnetwork        The network interface instance to use.
 * @param nce_onboarding   The endpoint structure representing the target server.
 * @return int             0 on success, error code otherwise.
 */
int nce_os_connect( OSNetwork_t osnetwork,
                    OSEndPoint_t nce_oboarding );

/**
 * @brief Sends data over an established Network connection.
 *
 * @param osnetwork        The network interface instance to use.
 * @param pBuffer          Pointer to the data buffer to send.
 * @param bytesToSend      Number of bytes to send from the buffer.
 * @return int             Number of bytes successfully sent, or error code on failure.
 */
int nce_os_send( OSNetwork_t osnetwork,
                 void * pBuffer,
                 size_t bytesToSend );

/**
 * @brief Receives data over an established Network connection.
 *
 * @param osnetwork        The network interface instance to use.
 * @param pBuffer          Pointer to the buffer where received data will be stored.
 * @param bytesToRecv      Number of bytes to receive into the buffer.
 * @return int             Number of bytes successfully received, or error code on failure.
 */
int nce_os_recv( OSNetwork_t osnetwork,
                 void * pBuffer,
                 size_t bytesToRecv );


/**
 * @brief Closes an active Network connection.
 *
 * @param osnetwork        The network interface instance to close.
 * @return int             0 on successful disconnection, error code otherwise.
 */
int nce_os_disconnect( OSNetwork_t osnetwork );
