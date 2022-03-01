/*
 * MIT License
 *
 * Copyright (c) 2022 1NCE
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file nce_sdk.h
 * @brief File containing the necessary information to connect to 1NCE services.
 *
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 1 Mar 2022
 */

#ifndef NCE_SDK_H_
#define NCE_SDK_H_
/* Standard includes. */
#include <string.h>
#include "tls_interface.h"
#include <assert.h>
#include <stdbool.h>



/**
 * @brief 1NCE onboarding endpoint.
 */
#define ONBOARDING_ENDPOINT    "device.connectivity-suite.cloud"

/**
 * @brief 1NCE onboarding port.
 */
#define ONBOARDING_PORT        ( 443U )


/**
 * @brief 1NCE translation service (mask of payload in given template).
 * @note U: unsigned integer.
 * @note S: Stiring.
 * @note I: Integer.
 * @note F: Float.
 */
#define BCL_PAYLOAD    "SFI"

/*
 * Get device information from 1NCE server including:
 * 1. thingName
 * 2. AWS IoT Core endpoint
 * 3. AWS IoT Core root CA
 * 4. Device X.509 certificate
 * 5. Device private key
 * 6. psk
 * 7. psk Identity
 */

/*-----------------------------------------------------------*/

/**
 * @brief diagnostic information to be written to the standard error file.
 *
 */
#define configASSERT            assert

/**
 * @brief Size of buffer to keep received data.
 */
#define RECV_BUFFER_LEN         ( 1500U )

/**
 * @brief Size of buffer to keep local strings.
 */
#define MAX_LOCAL_STR_LEN       ( 100U )

/**
 * @brief Size of buffer to keep certificate.
 */
#define MAX_CERT_LEN            ( 2000U )

/**
 * @brief Size of buffer to keep key.
 */
#define MAX_KEY_LEN             ( 3000U )

/**
 * @brief Size of the range request from 1nce onboarding service.
 */
#define democonfigRANGE_SIZE    ( 1000U )

/*-----------------------------------------------------------*/

/**
 * @brief thing name acquired from onboarding response.
 */
char nceThingName[ MAX_LOCAL_STR_LEN ];

/**
 * @brief AWS IoT endpoint acquired from onboarding response.
 */
char nceEndpoint[ MAX_LOCAL_STR_LEN ];

/**
 * @brief new topic name starting with thing name.
 */
char nceExampleTopic[ MAX_LOCAL_STR_LEN + 20 ];

/**
 * @brief root CA acquired from onboarding response.
 */
char rootCA[ MAX_CERT_LEN ];

/**
 * @brief client certificate acquired from onboarding response.
 */
char clientCert[ MAX_CERT_LEN ];

/**
 * @brief device private key acquired from onboarding response.
 */
char prvKey[ MAX_KEY_LEN ];

/**
 *  @brief A buffer to temporarily save received data.
 */
char part[ RECV_BUFFER_LEN ];

/**
 * @brief Pre-shared key acquired from onboarding response for DTLS connection.
 */
char psk[ MAX_LOCAL_STR_LEN ];

/**
 * @brief identity acquired from onboarding response  for DTLS connection.
 */
char pskIdentity[ MAX_LOCAL_STR_LEN ];



/*-----------------------------------------------------------*/

/**
 * @brief Connect to 1NCE server with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[in] xtls_api: tls interface object.
 *
 * @return The status of the final connection attempt.
 */
TlsTransportStatus_t nce_connect( tls_api * xtls_api );

/*-----------------------------------------------------------*/

/**
 * @brief request a response from 1nce endpoint
 *
 * @param[in] xtls_api: tls interface object.
 * @param[in] completeResponse: Response received from 1nce endpoint.
 * @param[in] mqtt_onboarding: type of onboarding mqtt/coap.
 *
 * @return The status of the onboarding.
 */
uint8_t nce_onboard( tls_api * xtls_api,
                     char * completeResponse,
                     uint8_t mqtt_onboarding );
/*-----------------------------------------------------------*/

/**
 * @brief Translation service feature: translates message to binary payload with a given template in 1NCE Portal.
 *
 * @param[in] type: Mask of Payload.
 * @param[in] position: position of value in given template in 1NCE Portal.
 * @param[in] packet: packet to send to the endpoint
 * @param[in] selector: select data used in control statement.
 * @param[in] argument: value to translate to binary
 * @param[in] location: location in memory
 * @param[in] bcl_payload_bytelength: table of values represents the length of the chunk in bytes
 *
 * @note: bcl_payload_bytelenth: must be the same in the given template.
 *
 *
 * @return location in memory.
 */
int nce_generate_BCL_payload( char * type,
                              size_t position,
                              uint8_t * packet,
                              uint8_t selector,
                              void * argument,
                              int location,
                              int * bcl_payload_bytelength );

/*-----------------------------------------------------------*/

/**
 * @brief initialize connection information
 * to MQTT / COAP.
 *
 * @param[in] xtls_api: tls interface object.
 * @param[in] mqtt: type of onboarding wanted mqtt/coap.
 *
 * @return The status of the execution result.
 */

uint8_t nce_sdk( tls_api * xtls_api,
                 uint8_t mqtt );



/**
 * @brief Macro that is called for logging "Error" level
 * messages.
 *
 * To enable error level logging, this macro should be mapped to the
 * application-specific logging implementation that supports error logging.
 *
 * @note This logging macro is called with parameters wrapped in
 * double parentheses to be ISO C89/C90 standard compliant. For a reference
 * POSIX implementation of the logging macros, refer to core_mqtt_config.h files, and the
 * logging-stack in demos folder of the
 * [AWS IoT Embedded C SDK repository](https://github.com/aws/aws-iot-device-sdk-embedded-C).
 *
 * <b>Default value</b>: Error logging is turned off, and no code is generated for calls
 * to the macro on compilation.
 */
#ifndef LogError
    #define LogError( message )
#endif

/**
 * @brief Macro that is for logging "Warning" level
 * messages.
 *
 * To enable warning level logging, this macro should be mapped to the
 * application-specific logging implementation that supports warning logging.
 *
 * @note This logging macro is called with parameters wrapped in
 * double parentheses to be ISO C89/C90 standard compliant. For a reference
 * POSIX implementation of the logging macros, refer to core_mqtt_config.h files, and the
 * logging-stack in demos folder of the
 * [AWS IoT Embedded C SDK repository](https://github.com/aws/aws-iot-device-sdk-embedded-C/).
 *
 * <b>Default value</b>: Warning logs are turned off, and no code is generated for calls
 * to the macro on compilation.
 */
#ifndef LogWarn
    #define LogWarn( message )
#endif

/**
 * @brief Macro that is called for logging "Info" level
 * messages.
 *
 * To enable info level logging, this macro should be mapped to the
 * application-specific logging implementation that supports info logging.
 *
 * @note This logging macro is called with parameters wrapped in
 * double parentheses to be ISO C89/C90 standard compliant. For a reference
 * POSIX implementation of the logging macros, refer to core_mqtt_config.h files, and the
 * logging-stack in demos folder of the
 * [AWS IoT Embedded C SDK repository](https://github.com/aws/aws-iot-device-sdk-embedded-C/).
 *
 * <b>Default value</b>: Info logging is turned off, and no code is generated for calls
 * to the macro on compilation.
 */
#ifndef LogInfo
    #define LogInfo( message )
#endif

/**
 * @brief Macro that is called for logging "Debug" level
 * messages.
 *
 * To enable debug level logging, this macro should be mapped to the
 * application-specific logging implementation that supports debug logging.
 *
 * @note This logging macro is called with parameters wrapped in
 * double parentheses to be ISO C89/C90 standard compliant. For a reference
 * POSIX implementation of the logging macros, refer to core_mqtt_config.h files, and the
 * logging-stack in demos folder of the
 * [AWS IoT Embedded C SDK repository](https://github.com/aws/aws-iot-device-sdk-embedded-C/).
 *
 * <b>Default value</b>: Debug logging is turned off, and no code is generated for calls
 * to the macro on compilation.
 */
#ifndef LogDebug
    #define LogDebug( message )
#endif

#if !defined( pvPortMalloc ) || !defined( vPortFree )
    #include <stdlib.h>
    /** @brief Allocate RAM Memory */
    #define pvPortMalloc( size )    malloc( size )
    /** @brief Free RAM Memory */
    #define vPortFree( ptr )        free( ptr )
#endif
#endif /* ifndef NCE_SDK_H_ */
