/**
 * @file coap_interface_zephyr.c
 * @brief Implements a COAP interface for ZEPHYR OS.
 * @date 07 November 2024
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <modem/lte_lc.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/coap.h>
#include <coap_interface_zephyr.h>
#include "log_interface.h"

LOG_MODULE_DECLARE(NCE_SDK, CONFIG_NCE_SDK_LOG_LEVEL);

int nce_coap_init( coap_packet_t * coap_packet,
                   uint8_t * buffer,
                   uint16_t packet_length,
                   uint8_t coap_version,
                   uint8_t coap_type,
                   uint8_t coap_method )
{
    return coap_packet_init( coap_packet, buffer, packet_length,
                             coap_version, coap_type,
                             COAP_TOKEN_MAX_LEN, coap_next_token(),
                             coap_method, coap_next_id() );
}

int nce_coap_append_option( coap_packet_t * coap_packet,
                            uint16_t option_code,
                            const uint8_t * option_value,
                            uint16_t length )
{
    return coap_packet_append_option( coap_packet, option_code, option_value, length );
}

int nce_coap_append_payload( coap_packet_t * coap_packet,
                             const uint8_t * coap_payload,
                             uint16_t coap_payload_len )
{
    int err = coap_packet_append_payload_marker( coap_packet );

    if( err < 0 )
    {
        NceOSLogError( "[ERR] Unable to append payload marker, err %d\n", err );
        return err;
    }

    return coap_packet_append_payload( coap_packet, ( uint8_t * ) coap_payload,
                                       coap_payload_len );
}


int nce_coap_parse( uint8_t * buffer,
                    coap_packet_t * coap_response,
                    uint16_t len )
{
    return coap_packet_parse( coap_response, buffer, len, NULL, 0 );
}
