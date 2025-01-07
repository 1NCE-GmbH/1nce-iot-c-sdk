/**
 * @file coap_interface_zephyr_utils.h
 * @brief Implements utility functions for CoAP communication on Zephyr OS.
 * @date 07 November 2024
 */


#include "udp_interface.h"

#include "zephyr/net/coap.h"

/**
 * @brief Establishes a connection to a specified CoAP server.
 *
 * @param osNetwork        Pointer to the network interface.
 * @param coap_server      Endpoint structure representing the CoAP server.
 * @return int             0 on successful connection, error code otherwise.
 */
int nce_connect_to_coap_server( os_network_ops_t * osNetwork,
                                OSEndPoint_t coap_server );

/**
 * @brief Creates a CoAP request.
 *
 * @param coap_packet      Pointer to the CoAP packet structure for the request.
 * @param coap_method      CoAP method (e.g., GET, POST) to use in the request.
 * @param uriPath          Optional URI path for the CoAP resource.
 * @param uriQuery         Optional URI query string.
 * @param proxyUri         Optional URI for a CoAP proxy server.
 * @param payload          Payload data to include ins the request.
 * @param content_format   Payload content format.
 * @param payload_len      Length of the payload data.
 * @return int             0 on successful request, error code otherwise.
 */
int nce_coap_request( struct coap_packet * coap_packet,
                      uint8_t coap_method,
                      const char * uriPath,
                      const char * uriQuery,
                      const char * proxyUri,
                      uint8_t content_format,
                      const char * payload,
                      uint16_t payload_len );

/**
 * @brief Prints the payload of a CoAP packet to the console.
 *
 * @param packet           Pointer to the CoAP packet containing the payload.
 */
void print_coap_payload( struct coap_packet * packet );

/**
 * @brief Prints the response code of a CoAP packet to the console.
 *
 * @param packet           Pointer to the CoAP packet containing the response code.
 */
void print_coap_header( struct coap_packet * packet );

/**
 * @brief Checks the CoAP response code and returns whether it is accepted.
 *
 * @param packet          Pointer to the CoAP packet containing the header data.
 * @return bool           Returns `true` if the response code is in the success range (2.xx), or `false` otherwise.
 */
bool check_and_print_coap_response_code( struct coap_packet * packet );
