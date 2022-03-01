/**
 * @file tls_interface.h
 * @brief TLS interface definitions to send and receive data over the
 * network via TLS.
 */
#ifndef TLS_INTERFACE_H_
#define TLS_INTERFACE_H_

#include <stdio.h>
#include <stdint.h>

/*-----------------------------------------------------------*/
#ifndef USING_MBEDTLS

/**
 * @brief TLS Connect / Disconnect return status.
 */
    typedef enum TlsTransportStatus
    {
        TLS_TRANSPORT_SUCCESS = 0,         /**< Function successfully completed. */
        TLS_TRANSPORT_INVALID_PARAMETER,   /**< At least one parameter was invalid. */
        TLS_TRANSPORT_INSUFFICIENT_MEMORY, /**< Insufficient memory required to establish connection. */
        TLS_TRANSPORT_INVALID_CREDENTIALS, /**< Provided credentials were invalid. */
        TLS_TRANSPORT_HANDSHAKE_FAILED,    /**< Performing TLS handshake with server failed. */
        TLS_TRANSPORT_INTERNAL_ERROR,      /**< A call to a system API resulted in an internal error. */
        TLS_TRANSPORT_CONNECT_FAILURE      /**< Initial connection to the server failed. */
    } TlsTransportStatus_t;


/**
 *
 * @typedef NetworkContext_t
 * @brief The NetworkContext is an incomplete type. An implementation of this
 * interface must define struct NetworkContext for the system requirements.
 * This context is passed into the network interface functions.
 */
/* @[define_networkcontext] */
    struct NetworkContext;
    typedef struct NetworkContext NetworkContext_t;
/* @[define_networkcontext] */

/**
 * @brief Contains the credentials necessary for tls connection setup for onload certificate.
 */
    typedef struct NetworkCredentials
    {
        /**
         * @brief To use ALPN, set this to a NULL-terminated list of supported
         * protocols in decreasing order of preference.
         *
         * See [this link]
         * (https://aws.amazon.com/blogs/iot/mqtt-with-tls-client-authentication-on-port-443-why-it-is-useful-and-how-it-works/)
         * for more information.
         */
        const char ** pAlpnProtos;

        /**
         * @brief Disable server name indication (SNI) for a TLS session.
         */
        long disableSni;

        /**
         * @brief String representing a trusted server root certificate.
         */
        const uint8_t * pRootCa;

        /**
         * @brief Size associated with #NetworkCredentials.pRootCa.
         */
        size_t rootCaSize;

        /**
         * @brief String representing the client certificate.
         *
         */
        const uint8_t * pClientCert;

        /**
         * @brief Size associated with #NetworkCredentials.pClientCert.
         */
        size_t clientCertSize;

        /**
         * @brief String representing the client certificate's private key.
         */
        const uint8_t * pPrivateKey;

        /**
         * @brief Size associated with #NetworkCredentials.pPrivateKey.
         */
        size_t privateKeySize;
    } NetworkCredentials_t;
#endif /* ifndef USING_MBEDTLS */

/**
 * @brief Tls interface for connect to server via tls.
 *
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pHostName The hostname of the remote endpoint.
 * @param[in] port The destination port.
 * @param[in] pNetworkCredentials Credentials for the TLS connection.
 * @param[in] receiveTimeoutMs Receive socket timeout.
 * @param[in] sendTimeoutMs Send socket timeout.
 *
 * @return #TLS_TRANSPORT_SUCCESS, #TLS_TRANSPORT_INSUFFICIENT_MEMORY, #TLS_TRANSPORT_INVALID_CREDENTIALS,
 * #TLS_TRANSPORT_HANDSHAKE_FAILED, #TLS_TRANSPORT_INTERNAL_ERROR, or #TLS_TRANSPORT_CONNECT_FAILURE.
 */
typedef TlsTransportStatus_t (* nce_TLS_Connect)( NetworkContext_t * pNetworkContext,
                                                  const char * pHostName,
                                                  uint16_t port,
                                                  const NetworkCredentials_t * pNetworkCredentials,
                                                  uint32_t receiveTimeoutMs,
                                                  uint32_t sendTimeoutMs );

/**
 * @brief Gracefully disconnect an established TLS connection.
 *
 * @param[in] pNetworkContext Network context.
 */
typedef void (* nce_TLS_Disconnect)( NetworkContext_t * pNetworkContext );

/**
 * @brief Receives data from an established TLS connection.
 *
 * @param[in] pNetworkContext The Network context.
 * @param[out] pBuffer Buffer to receive bytes into.
 * @param[in] bytesToRecv Number of bytes to receive from the network.
 *
 * @return Number of bytes (> 0) received if successful;
 * 0 if the socket times out without reading any bytes;
 * negative value on error.
 */
typedef int32_t (* nce_TLS_recv)( NetworkContext_t * pNetworkContext,
                                  void * pBuffer,
                                  size_t bytesToRecv );

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
typedef int32_t (* nce_TLS_send)( NetworkContext_t * pNetworkContext,
                                  const void * pBuffer,
                                  size_t bytesToSend );

/**
 *  @brief tls_api: The API to be implemented.
 */
typedef struct tls_api
{
    /**
     * @brief TLS connect interface.
     */
    nce_TLS_Connect conn;

    /**
     * @brief TLS disconnect interface.
     */
    nce_TLS_Disconnect disconn;

    /**
     * @brief TLS send interface.
     */
    nce_TLS_send tlssend;

    /**
     * @brief TLS receive interface.
     */
    nce_TLS_recv tlsrecv;

    /**
     * @brief Implementation-defined network context.
     */
    NetworkContext_t * pNetworkContext;
} tls_api;

#endif /* ifndef TLS_INTERFACE_H_ */
