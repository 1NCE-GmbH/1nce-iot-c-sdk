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
 * @file nce_iot_c_sdk.h
 * @brief File containing the necessary information to connect to 1NCE services.
 *
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 1 Mar 2022
 */

#ifndef NCE_IOT_C_SDK_H_
#define NCE_IOT_C_SDK_H_
/* Standard includes. */
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "tls_interface.h"

/**
 * @brief definition of return codes.
 */
enum
{
    NCE_SDK_SUCCESS = 0,              /**< The operation was successful. */
    NCE_SDK_CONNECT_ERROR = -1,       /**< Connection error. */
    NCE_SDK_SEND_ERROR = -2,          /**< Packet sending error. */
    NCE_SDK_RECEIVE_ERROR = -3,       /**< Packet reception error. */
    NCE_SDK_PARSING_ERROR = -4,       /**< Response parsing error. */
    NCE_SDK_BINARY_PAYLOAD_ERROR = -5 /**< Binary payload conversion error. */
};


#ifndef __ZEPHYR__

/**
 * @brief the maximum number of connection attempts to 1NCE OS server.
 */
    #define NCE_SDK_CONNECT_ATTEMPTS    5

/**
 * @brief Enable 1NCE Device Authenticator.
 */
    #define  NCE_DEVICE_AUTHENTICATOR

#endif /* ifndef __ZEPHYR__ */


#ifdef NCE_DEVICE_AUTHENTICATOR

/**
 * @brief Contains the credentials necessary for DTLS connection setup.
 */
typedef struct DtlsKey
{
    /**
     * @brief String representing a trusted secret key for authentication.
     */
    char Psk[ 100 ];

    /**
     * @brief String representing an identity indicated to the server (which key to use).
     */
    char PskIdentity[ 100 ];
} DtlsKey_t;

/**
 * @brief 1NCE onboarding endpoint.
 * @brief 1NCE onboarding port.
 */
static const OSEndPoint_t NceOnboard =
{
    .host = "device.connectivity-suite.cloud",
    .port = 443
};

/**
 * @brief Connect to 1NCE server with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * 1NCE endpoint require TLS Connection
 *
 * @param[in] osNetwork: tls interface object.
 *
 * @return The status of the final connection attempt.
 */
int os_onboard( os_network_ops_t * osNetwork );

/**
 * @brief Request a response from 1nce endpoint
 *
 * @param[in] osNetwork: TLS interface object.
 * @param[in] nceKey: new DTLS credential required.
 *
 * @return The status of the onboarding.
 */
int os_auth( os_network_ops_t * osNetwork,
             DtlsKey_t * nceKey );


#endif /* ifdef NCE_DEVICE_AUTHENTICATOR */

#endif /* ifndef NCE_IOT_C_SDK_H_ */
