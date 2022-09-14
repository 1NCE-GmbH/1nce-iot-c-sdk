/**
 * @file tls_interface.h
 * @brief TLS interface definitions to send and receive data over the
 * network via TLS.
 */
#ifndef TLS_INTERFACE_H_
#define TLS_INTERFACE_H_

/**
 * @brief 1NCE onboarding endpoint.
 * @brief 1NCE onboarding port.
 */

/**
 * @brief general network endpoint
 *
 */
struct OSEndPoint
{
    /**
     * @brief The remote EndPoint hostname.
     *
     */
    const char host[ 100 ];

    /**
     * @brief The remote EndPoint port number
     *
     */
    int port;
};
typedef struct OSEndPoint   OSEndPoint_t;

/**
 *
 * @typedef OSNetwork_t
 * @brief The OSNetwork is an incomplete type. An implementation of this
 * interface must define struct OSNetwork for the system requirements.
 * This OSNetwork is passed into the network interface functions.
 */
struct OSNetwork;
typedef struct OSNetwork    * OSNetwork_t;


/**
 *  @brief os_network_ops: The operations to be implemented.
 */
struct os_network_ops
{
    /**
     * @brief Implementation-defined network socket.
     */
    OSNetwork_t os_socket;

    /**
     * @brief Tls interface for connect to server via tls.
     *
     *
     * @param[in] osnetwork Implementation-defined network socket.
     * @param[in] nce_oboarding General network endpoint.
     *
     * @return @return The status of the connection.
     */
    int (* nce_os_tls_connect)( OSNetwork_t osnetwork,
                                OSEndPoint_t nce_oboarding );

    /**
     * @brief Sends data over an established TLS connection.
     *
     *
     * @param[in] pNetworkContext The network context.
     * @param[in] pBuffer Buffer containing the bytes to send.
     * @param[in] bytesToSend Number of bytes to send from the buffer.
     *
     * @return Number of bytes (> 0) sent on success;
     * 0 if the socket times out without sending any bytes;
     * else a negative value to represent error.
     */
    int (* nce_os_tls_send)( OSNetwork_t osnetwork,
                             void * pBuffer,
                             size_t bytesToSend );

    /**
     * @brief Receives data from an established TLS connection.
     *
     * @param[in] osnetwork Implementation-defined network socket.
     * @param[out] pBuffer Buffer to receive bytes into.
     * @param[in] bytesToRecv Number of bytes to receive from the network.
     *
     * @return Number of bytes (> 0) received if successful;
     * 0 if the socket times out without reading any bytes;
     * negative value on error.
     */
    int (* nce_os_tls_recv)( OSNetwork_t osnetwork,
                             void * pBuffer,
                             size_t bytesToRecv );

    /**
     * @brief Gracefully disconnect an established TLS connection.
     *
     * @param[in] osnetwork Implementation-defined network socket.
     */
    int (* nce_os_tls_disconnect)( OSNetwork_t osnetwork );
};

typedef struct os_network_ops os_network_ops_t;



#endif /* ifndef TLS_INTERFACE_H_ */
