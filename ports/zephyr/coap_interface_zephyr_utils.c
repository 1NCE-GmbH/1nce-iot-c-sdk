/**
 * @file coap_interface_zephyr_utils.c
 * @brief Implements utility functions for CoAP communication on Zephyr OS.
 * @date 07 November 2024
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_interface.h"
#include <modem/lte_lc.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/coap.h>
#include "nce_iot_c_sdk.h"
#include <udp_interface.h>
#include <network_interface_zephyr.h>
#include <coap_interface_zephyr.h>
#include <coap_interface_zephyr_utils.h>

#define MAX_COAP_MSG_LEN           1024
#define COAP_CODE_CLASS_SIZE       32
#define COAP_SUCCESS_CODE_CLASS    2

LOG_MODULE_DECLARE( NCE_SDK, CONFIG_NCE_SDK_LOG_LEVEL );

int nce_connect_to_coap_server( os_network_ops_t * osNetwork,
                                OSEndPoint_t coap_server )
{
    int err = NCE_SDK_SUCCESS;

    err = nce_os_connect( osNetwork->os_socket, coap_server );

    if( err )
    {
        NceOSLogError( "[ERR] Failed to Connect to CoAP server, Host: %s, Port: %d\n", coap_server.host, coap_server.port );
        return err;
    }

    return err;
}

int nce_coap_request( struct coap_packet * coap_packet,
                      uint8_t coap_method,
                      const char * uriPath,
                      const char * uriQuery,
                      const char * proxyUri,
                      uint8_t content_format,
                      const char * payload,
                      uint16_t payload_len )
{
    uint8_t * data = ( uint8_t * ) k_malloc( MAX_COAP_MSG_LEN );

    if( !data )
    {
        return -ENOMEM;
    }

    int r = nce_coap_init( coap_packet, data, MAX_COAP_MSG_LEN, COAP_VERSION_1, COAP_TYPE_CON, COAP_METHOD_POST );

    if( r < 0 )
    {
        NceOSLogError( "[ERR] Failed to init CoAP message\n" );
        goto end;
    }

    /* Append URI Path if provided */
    if( uriPath != NULL )
    {
        r = nce_coap_append_option( coap_packet, COAP_OPTION_URI_PATH, uriPath, strlen( uriPath ) );

        if( r < 0 )
        {
            NceOSLogError( "[ERR] Unable to add URI Path option (%s) to request\n", uriPath );
            goto end;
        }
    }

    /* Append content format */
    r = nce_coap_append_option( coap_packet, COAP_OPTION_CONTENT_FORMAT, &content_format, sizeof( content_format ) );

    if( r < 0 )
    {
        NceOSLogError( "[ERR] Unable to add content Format option to request\n" );
        goto end;
    }

    /* Append URI Query if provided */
    if( uriQuery != NULL )
    {
        r = nce_coap_append_option( coap_packet, COAP_OPTION_URI_QUERY, uriQuery, strlen( uriQuery ) );

        if( r < 0 )
        {
            NceOSLogError( "[ERR] Unable to add URI Query option (%s) to request\n", uriQuery );
            goto end;
        }
    }

    /* Append Proxy URI if provided */
    if( proxyUri != NULL )
    {
        r = nce_coap_append_option( coap_packet, COAP_OPTION_PROXY_URI, proxyUri, strlen( proxyUri ) );

        if( r < 0 )
        {
            NceOSLogError( "[ERR] Unable to add Proxy URI option (%s) to request\n", proxyUri );
            goto end;
        }
    }

    if( payload != NULL )
    {
        r = nce_coap_append_payload( coap_packet, ( uint8_t * ) payload, payload_len );

        if( r < 0 )
        {
            NceOSLogError( "[ERR] Unable to append payload (length: %d)\n", payload_len );
            goto end;
        }
    }

end:
    k_free( data );
    return r;
}

void print_coap_payload( struct coap_packet * packet )
{
    uint16_t payload_len;
    const uint8_t * payload = coap_packet_get_payload( packet, &payload_len );

    NceOSLogDebug( "[DBG] CoAP Payload len: %d\n", payload_len );

    if( payload_len > 0 )
    {
        NceOSLogInfo( "[INF] CoAP Payload: %s\n", payload );
    }
}

bool check_and_print_coap_response_code( struct coap_packet * packet )
{
    uint8_t code_class = coap_header_get_code( packet ) / COAP_CODE_CLASS_SIZE;
    uint8_t code_detail = coap_header_get_code( packet ) % COAP_CODE_CLASS_SIZE;

    /* Check if the response code is in the success range (2.xx) */
    bool success = ( code_class == COAP_SUCCESS_CODE_CLASS );

    NceOSLogInfo( "[INF] Response Code: %d.%02d, Success: %s\n",
                  code_class, code_detail, success ? "true" : "false" );

    return success;
}

void print_coap_header( struct coap_packet * packet )
{
    NceOSLogInfo( "[INF] CoAP Header:\n" );
    NceOSLogInfo( "[INF] Version: %d\n", coap_header_get_version( packet ) );
    NceOSLogInfo( "[INF] Type: %s\n", coap_header_get_type( packet ) == COAP_TYPE_CON ? "CON" : "NON" );
    check_and_print_coap_response_code( packet );
    NceOSLogInfo( "[INF] Message ID: %u\n", coap_header_get_id( packet ) );
}
