/**
 * @file network_interface_zephyr.c
 * @brief Implements network interface and optional DTLS (Datagram Transport Layer Security) support
 *        for communication in Zephyr OS.
 *
 * This file provides the implementation of functions for setting up and managing
 * Network connection (via UDP), sending and receiving data, and configuring DTLS (if enabled).
 * It interacts with Zephyr's networking stack to provide transport layer communication.
 *
 * @author Hatim Jamali & Mohamed Abdelmaksoud
 * @date 07 November 2024
 */

#ifndef NCE_SDK_H_
    #include <nce_iot_c_sdk.h>
#endif

#include <zephyr/kernel.h>
#include <stdio.h>
#include <modem/lte_lc.h>
#include <zephyr/posix/arpa/inet.h>
#include <zephyr/posix/netdb.h>
#include <zephyr/posix/sys/socket.h>
#include <zephyr/posix/poll.h>
#include "log_interface.h"
#include <network_interface_zephyr.h>

#ifdef CONFIG_NCE_SDK_ENABLE_DTLS
    #if !defined( CONFIG_NCE_SDK_DTLS_SECURITY_TAG ) || ( CONFIG_NCE_SDK_DTLS_SECURITY_TAG < 0 )
        #error "CONFIG_NCE_SDK_DTLS_SECURITY_TAG is not set. Please define it in prj.conf to specify the DTLS security tag for 1NCE CoAP server."
    #endif
    #include <zephyr/net/tls_credentials.h>
    const sec_tag_t dtls_sec_tag[] =
    {
        CONFIG_NCE_SDK_DTLS_SECURITY_TAG,
    };
    #define NCE_SDK_DTLS_PORT    5684
#endif /* ifdef NCE_SDK_ENABLE_DTLS */

LOG_MODULE_DECLARE( NCE_SDK, CONFIG_NCE_SDK_LOG_LEVEL );

/* Sample Network definition */
struct OSNetwork xOSNetwork = { .os_socket = 0 };

#ifdef CONFIG_NCE_SDK_ENABLE_DTLS

/**
 * @brief Configure DTLS socket.
 *
 * @return int 0 on success, negative error code on failure.
 */

    int prv_dtls_setup( int fd )
    {
        int err;
        int verify;
        int role;

        /* enums for DTLS peer verification and client mode */
        enum
        {
            NONE = 0,
            OPTIONAL = 1,
            REQUIRED = 2,
        };

        enum
        {
            CLIENT = 0,
            SERVER = 1,
        };


        /* Set up DTLS peer verification */
        verify = NONE;
        err = setsockopt( fd, SOL_TLS, TLS_PEER_VERIFY, &verify, sizeof( verify ) );

        if( err )
        {
            NceOSLogError( "[ERR] Failed to setup peer verification, err %d\n", errno );
            return err;
        }

        /* Set up DTLS client mode */
        role = CLIENT;
        err = setsockopt( fd, SOL_TLS, TLS_DTLS_ROLE, &role, sizeof( role ) );

        if( err )
        {
            NceOSLogError( "[ERR] Failed to setup DTLS role, err %d\n", errno );
            return err;
        }

        /* Set up DTLS timeout */
        int dtls_timeo = CONFIG_NCE_SDK_DTLS_HANDSHAKE_TIMEOUT_SECONDS;

        err = setsockopt( fd, SOL_TLS, TLS_DTLS_HANDSHAKE_TIMEO, &dtls_timeo, sizeof( dtls_timeo ) );

        if( err )
        {
            NceOSLogWarn( "[WRN] Failed to setup DTLS HandTimout, err %d\n", errno );
        }

        /* Associate the socket with the security tag */
        err = setsockopt( fd, SOL_TLS, TLS_SEC_TAG_LIST, dtls_sec_tag,
                          sizeof( dtls_sec_tag ) );

        if( err )
        {
            NceOSLogError( "[ERR] Failed to setup TLS sec tag, err %d\n", errno );
            return err;
        }

        NceOSLogInfo( "[INF] DTLS Socket is configured successfully\n" );
        return err;
    }
#endif /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */

int nce_os_connect( OSNetwork_t osnetwork,
                    OSEndPoint_t endpoint )
{
    int socket_num;
    int err;
    struct zsock_addrinfo * addr;
    struct zsock_addrinfo hints =
    {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_DGRAM
    };

    #ifdef CONFIG_NCE_SDK_ENABLE_DTLS
        if( endpoint.port == NCE_SDK_DTLS_PORT )
        {
            socket_num = socket( AF_INET, SOCK_DGRAM, IPPROTO_DTLS_1_2 );
        }
        else
        {
            socket_num = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
        }
    #else /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */
        socket_num = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    #endif /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */

    if( socket_num < 0 )
    {
        NceOSLogError( "[ERR] Failed to create socket, err %d\n", errno );
        return -errno;
    }

    /* Configure Send & Receive timeouts */
    struct timeval send_timeo =
    {
        .tv_sec  = CONFIG_NCE_SDK_SEND_TIMEOUT_SECONDS,
        .tv_usec = 0,
    };

    err = setsockopt( socket_num, SOL_SOCKET, SO_SNDTIMEO, &send_timeo, sizeof( send_timeo ) );

    if( err )
    {
        NceOSLogWarn( "[WRN] Failed to set socket send timeout, errno %d", errno );
    }

    struct timeval recv_timeo =
    {
        .tv_sec  = CONFIG_NCE_SDK_RECV_TIMEOUT_SECONDS,
        .tv_usec = 0,
    };

    err = setsockopt( socket_num, SOL_SOCKET, SO_RCVTIMEO, &recv_timeo, sizeof( recv_timeo ) );

    if( err )
    {
        NceOSLogWarn( "[WRN] Failed to set socket receive timeout, errno %d", errno );
    }

    #if defined( CONFIG_NCE_SDK_ENABLE_DTLS )
        /* Setup DTLS socket options */
        if( endpoint.port == NCE_SDK_DTLS_PORT )
        {
            err = prv_dtls_setup( socket_num );

            if( err )
            {
                NceOSLogError( "[ERR] Failed to Configure DTLS!, err %d\n", err );
                return err;
            }
        }
    #endif /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */

    err = getaddrinfo( endpoint.host,
                       NULL, &hints, &addr );

    if( err )
    {
        NceOSLogError( "[ERR] Failed to resolve address, err %d\n", err );
        return err;
    }

    NceOSLogDebug( "[DBG] getaddrinfo status: %d\n", err );

    osnetwork->os_socket = socket_num;

    ( ( struct sockaddr_in * ) addr->ai_addr )->sin_port = htons( endpoint.port );

    size_t peer_addr_size = sizeof( struct sockaddr_in );

    err = connect( socket_num, addr->ai_addr,
                   peer_addr_size );

    if( err )
    {
        NceOSLogError( "[ERR] Failed to Connect to 1NCE Endpoint\n" );
        #if defined( CONFIG_NCE_SDK_ENABLE_DTLS )
            if( endpoint.port == NCE_SDK_DTLS_PORT )
            {
                err = NCE_SDK_DTLS_CONNECT_ERROR;
            }
        #endif /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */
        return err;
    }

    NceOSLogDebug( "[DBG] Socket Connect: %d\n", err );

    freeaddrinfo( addr );
    return err;
}

int nce_os_send( OSNetwork_t osnetwork,
                 void * pBuffer,
                 size_t bytesToSend )
{
    int flags = 0;
    int ret;

    ret = send( osnetwork->os_socket, pBuffer, bytesToSend, flags );

    NceOSLogDebug( "[DBG] Socket Send: %d\n", ret );

    return ret;
}

int nce_os_recv( OSNetwork_t osnetwork,
                 void * pBuffer,
                 size_t bytesToRecv )
{
    int flags = 0;
    int ret;

    ret = recv( osnetwork->os_socket, pBuffer, bytesToRecv, flags );

    NceOSLogDebug( "[DBG] Socket Receive: %d\n", ret );

    return ret;
}

int nce_os_disconnect( OSNetwork_t osnetwork )
{
    int err;

    err = close( osnetwork->os_socket );

    NceOSLogDebug( "[DBG] Socket Disconnect: %d\n", err );

    return err;
}
