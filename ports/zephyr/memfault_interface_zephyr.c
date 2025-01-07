/**
 * @file memfault_interface_zephyr.c
 * @brief Implements Memfault chunk sending via 1NCE COAP Proxy for ZEPHYR OS.
 * @date 07 November 2024
 *
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include "log_interface.h"
#include <stdbool.h>
#include <modem/lte_lc.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/coap.h>
#include <network_interface_zephyr.h>
#include <coap_interface_zephyr.h>
#include "memfault/core/data_packetizer.h"
#include "nce_iot_c_sdk.h"
#include "memfault_interface_zephyr.h"
#include "coap_interface_zephyr_utils.h"


LOG_MODULE_DECLARE( NCE_SDK, CONFIG_NCE_SDK_LOG_LEVEL );

K_MUTEX_DEFINE( os_memfault_send_mutex );

struct coap_packet proxy_response, proxy_request;

static const OSEndPoint_t proxyEndpoint =
{
    .host = PROXY_HOST,
    .port = PROXY_PORT
};

struct OSNetwork OSNetwork = { .os_socket = 0 };
os_network_ops_t osNetwork =
{
    .os_socket             = &OSNetwork,
    .nce_os_udp_connect    = nce_os_connect,
    .nce_os_udp_send       = nce_os_send,
    .nce_os_udp_recv       = nce_os_recv,
    .nce_os_udp_disconnect = nce_os_disconnect
};

/**
 * @brief Try sending Memfault data chunks over CoAP.
 *
 * This function retrieves Memfault data chunks and sends them to
 * 1NCE CoAP proxy server. If no data is available, it returns early.
 *
 * @return NCE_SDK_SUCCESS (0) on success, negative error code on failure.
 */
int prv_os_memfault_try_send( void )
{
    int err = NCE_SDK_SUCCESS;
    bool data_available = true;
    uint8_t memfault_buffer[ CONFIG_NCE_SDK_MEMFAULT_BUFFER_SIZE ];
    size_t memfault_buffer_len = sizeof( memfault_buffer );
    char receive_buffer[ RECEIVE_BUFFER_SIZE ];
    size_t receive_buffer_len = sizeof( receive_buffer );

    /* Check if data is available */
    if( !memfault_packetizer_data_available() )
    {
        NceOSLogInfo( "[INF] There is no data to be sent\n" );
        return NCE_SDK_SUCCESS;
    }

    /* Connect to CoAP server */
    err = nce_connect_to_coap_server( &osNetwork, proxyEndpoint );

    if( err )
    {
        NceOSLogError( "[ERR] Failed to connect to CoAP server, err %d\n", err );
        return err;
    }

    NceOSLogInfo( "[INF] Connected to CoAP server, Host: %s, Port: %d\n", proxyEndpoint.host, proxyEndpoint.port );

    /* Prepare to send Memfault chunks over CoAP */
    while( data_available )
    {
        /* Retrieve the next Memfault chunk */
        memfault_buffer_len = sizeof( memfault_buffer );

        data_available = memfault_packetizer_get_chunk( memfault_buffer, &memfault_buffer_len );

        if( !data_available )
        {
            NceOSLogInfo( "[INF] No more chunks to send\n" );
            goto end;
        }

        /* Create a CoAP Post request containing the chunk */
        err = nce_coap_request( &proxy_request, COAP_METHOD_POST, NULL, NULL,
                                CONFIG_NCE_SDK_MEMFAULT_PROXY_URI, COAP_CONTENT_FORMAT_APP_OCTET_STREAM,
                                memfault_buffer, memfault_buffer_len );

        if( err < 0 )
        {
            memfault_packetizer_abort();
            NceOSLogError( "[ERR] Unable to create CoAP request \n" );
            goto end;
        }

        /* Send the chunk over CoAP */
        err = nce_os_send( osNetwork.os_socket, proxy_request.data, proxy_request.offset );

        if( err < 0 )
        {
            memfault_packetizer_abort();
            NceOSLogError( "[ERR] Unable to send CoAP packet\n" );
            NceOSLogError( "[ERR] CoAP error code %d\n", err );
            goto end;
        }

        NceOSLogInfo( "[INF] Sent %zu bytes\n", memfault_buffer_len );

        /* Receive the CoAP response */
        memset( receive_buffer, '\0', receive_buffer_len * sizeof( char ) );
        int bytes_received = nce_os_recv( osNetwork.os_socket, receive_buffer, receive_buffer_len );

        if( bytes_received < 0 )
        {
            NceOSLogError( "[ERR] Unable to get CoAP response\n" );
            NceOSLogError( "[ERR] CoAP error code %d\n", bytes_received );
            err = bytes_received;
            goto end;
        }

        /* Parse and print the CoAP response */
        err = nce_coap_parse( receive_buffer, &proxy_response, bytes_received );

        if( err < 0 )
        {
            memfault_packetizer_abort();
            NceOSLogError( "[ERR] Unable to parse CoAP response\n" );
            NceOSLogError( "[ERR] CoAP error code %d\n", err );
            goto end;
        }

        bool success = check_and_print_coap_response_code( &proxy_response );

        if( !success )
        {
            memfault_packetizer_abort();
            /* Show a warning if the response code is not in the success range (2.xx) */
            NceOSLogError( "[ERR] Server did not accept the packet (non-success response).\n" );
            err = NCE_SDK_SERVER_RESPONSE_ERROR;
            goto end;
        }
    }

end:
    /* Close the Connection */
    nce_os_disconnect( osNetwork.os_socket );
    return err;
}

int os_memfault_send( void )
{
    if( k_mutex_lock( &os_memfault_send_mutex, K_NO_WAIT ) != 0 )
    {
        NceOSLogInfo( "[INFO] Another Memfault send request is in progress\n" );
        return NCE_SDK_SUCCESS;
    }

    int res = NCE_SDK_SUCCESS;
    int retry_count = 0;

    while( retry_count < CONFIG_NCE_SDK_MEMFAULT_ATTEMPTS )
    {
        res = prv_os_memfault_try_send();

        if( res == NCE_SDK_SUCCESS )
        {
            goto end;
        }

        NceOSLogError( "[ERR] Failed to send Memfault data, err %d\n", res );

        if( res == NCE_SDK_SERVER_RESPONSE_ERROR )
        {
            NceOSLogError( "[ERR] Aborting transmission. Please make sure that Memfault Plugin is enabled in 1NCE OS\n" );
            goto end;
        }

        retry_count++;
        /* Delay between attempts */
        k_sleep( K_SECONDS( CONFIG_NCE_SDK_MEMFAULT_ATTEMPT_DELAY_SECONDS ) );
        NceOSLogInfo( "[INF] Retrying ... (Attempt %d/%d)\n", retry_count, CONFIG_NCE_SDK_MEMFAULT_ATTEMPTS );
    }

    if( res != 0 )
    {
        NceOSLogError( "[ERR] Failed to send Memfault data after %d attempts\n", retry_count );
    }

end:
    k_mutex_unlock( &os_memfault_send_mutex );
    return res;
}
