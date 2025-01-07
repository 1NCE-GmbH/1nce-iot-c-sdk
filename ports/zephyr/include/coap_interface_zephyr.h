/**
 * @file coap_interface_zephyr.h
 * @brief Implements a COAP interface for ZEPHYR OS.
 * @date 07 November 2024
 */

#include "zephyr/net/coap.h"

struct coap_packet;
typedef struct coap_packet coap_packet_t;

/**
 * @brief Structure representing a COAP response.
 */
typedef struct coap_response
{
    uint8_t response_code;
    uint8_t version;
    uint8_t type;
    uint16_t msg_id;
} coap_response_t;

/**
 * @brief Initializes a COAP packet with specified parameters.
 *
 * @param coap_packet      Pointer to the COAP packet to initialize.
 * @param buffer           Buffer to store the packet data.
 * @param packet_length    Length of the packet.
 * @param coap_version     COAP version to use.
 * @param coap_type        COAP message type (e.g., confirmable, non-confirmable).
 * @param coap_method      COAP method (e.g., GET, POST).
 * @return int             0 on success, error code otherwise.
 */
int nce_coap_init( coap_packet_t * coap_packet,
                   uint8_t * buffer,
                   uint16_t packet_length,
                   uint8_t coap_version,
                   uint8_t coap_type,
                   uint8_t coap_method );


/**
 * @brief Appends an option to a COAP packet.
 *
 * @param coap_packet      Pointer to the COAP packet.
 * @param option_code      Code of the COAP option to append.
 * @param option_value     Value of the COAP option.
 * @param length           Length of the option value.
 * @return int             0 on success, error code otherwise.
 */
int nce_coap_append_option( coap_packet_t * coap_packet,
                            uint16_t option_code,
                            const uint8_t * option_value,
                            uint16_t length );

/**
 * @brief Appends a payload to a COAP packet.
 *
 * @param coap_packet      Pointer to the COAP packet.
 * @param coap_payload     Payload data to append.
 * @param coap_payload_len Length of the payload data.
 * @return int             0 on success, error code otherwise.
 */
int nce_coap_append_payload( coap_packet_t * coap_packet,
                             const uint8_t * coap_payload,
                             uint16_t coap_payload_len );

/**
 * @brief Parses a buffer into a COAP packet.
 *
 * @param buffer           Buffer containing the COAP packet data.
 * @param coap_packet      Pointer to the COAP packet structure to populate.
 * @param length           Length of the buffer.
 * @return int             0 on success, error code otherwise.
 */
int nce_coap_parse( uint8_t * buffer,
                    coap_packet_t * coap_packet,
                    uint16_t length );
