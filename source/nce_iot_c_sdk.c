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
 * @file nce_iot_c_sdk.c
 * @brief Implements the user-facing functions in nce_iot_c_sdk.h.
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 01 Mar 2022
 */

#include "nce_iot_c_sdk.h"
#include <stdlib.h>
#include <stdio.h>
#include "log_interface.h"

#ifdef NCE_DEVICE_AUTHENTICATOR

/**
 * @brief Process the DTLS credential to match the requirement
 *
 * @param[in] packet: the target packet to be processed.
 * @param[in] nceKey: the new DTLS credential required.
 *
 */
static int _get_psk( char * packet,
                     DtlsKey_t * nceKey )
{
    int status = NCE_SDK_PARSING_ERROR;
    char * resp = strstr( packet, "Express\r\n\r\n" ) + strlen( "Express\r\n\r\n" );

    if( resp == NULL )
    {
        NceOSLogError( "ERROR: Packet can't be NULL\n" );
        return status;
    }
    else
    {
        char * p = strtok( resp, "," );

        if( p == NULL )
        {
            NceOSLogError( "ERROR: Parsing Error\n" );
            return status;
        }
        else
        {
            strcpy( nceKey->Psk, p );
        }

        p = strtok( NULL, "," );

        if( p == NULL )
        {
            NceOSLogError( "ERROR: Parsing Error\n" );
            return status;
        }
        else
        {
            strcpy( nceKey->PskIdentity, p );
        }

        return NCE_SDK_SUCCESS;
    }
}

/*-----------------------------------------------------------*/

int os_onboard( os_network_ops_t * osNetwork )
{
    int status = NCE_SDK_CONNECT_ERROR;
    int attempts = 1;

    if( osNetwork == NULL )
    {
        NceOSLogError( "ERROR\n" );
        return status;
    }
    else
    {
        if( osNetwork->os_socket == NULL )
        {
            NceOSLogError( "socket can't be NULL\n" );
            return status;
        }
        else
        {
            NceOSLogInfo( "Connecting to 1NCE onboarding Service\n" );

            do
            {
                NceOSLogInfo( "connect to osNetwork" );
                status = osNetwork->nce_os_tls_connect( osNetwork->os_socket, NceOnboard );
                attempts++;
            } while( status != 0 && attempts < NCE_SDK_CONNECT_ATTEMPTS );
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

int os_auth( os_network_ops_t * osNetwork,
             DtlsKey_t * nceKey )
{
    int status = -1;
    char packet[ 500 ];

    status = os_onboard( osNetwork );

    if( status < 0 )
    {
        NceOSLogError( "ERROR\n" );
        return status;
    }
    else
    {
        NceOSLogInfo( "Start 1NCE device onboarding.\n" );
        memset( packet, '\0', 500 * sizeof( char ) );
        sprintf( packet, "GET /device-api/onboarding/coap HTTP/1.1\r\n"
                         "Host: %s\r\n"
                         "Accept: text/csv\r\n\r\n", NceOnboard.host );
        NceOSLogInfo( "Send Device Authenticator request:\r\n%.*s\n", strlen( packet ), packet );
        status = osNetwork->nce_os_tls_send( osNetwork->os_socket, &packet, strlen( packet ) );

        if( status < 0 )
        {
            NceOSLogError( "Failed to send Device Authenticator request.\n" );
            return NCE_SDK_SEND_ERROR;
        }

        memset( packet, '\0', 500 * sizeof( char ) );
        status = osNetwork->nce_os_tls_recv( osNetwork->os_socket, &packet[ 0 ], 1500 );

        if( status < 0 )
        {
            NceOSLogError( "Failed to receive Device credential.\n" );
            return NCE_SDK_RECEIVE_ERROR;
        }

        status = _get_psk( packet, nceKey );

        if( status < 0 )
        {
            NceOSLogError( "Failed to parse response.\n" );
            return status;
        }

        status = osNetwork->nce_os_tls_disconnect( osNetwork->os_socket );

        if( status < 0 )
        {
            NceOSLogError( "Failed to close socket.\n" );
            return status;
        }
    }

    return status;
}

#endif /* ifdef NCE_DEVICE_AUTHENTICATOR */

/*-----------------------------------------------------------*/

#ifdef NCE_ENERGY_SAVER

int os_energy_save( char * packet,
                    int selector,
                    int num_args,
                    ... )
{
    int location = 1;
    va_list ap;
    Element2byte_gen_t e;
    int i;

    memcpy( packet, &selector, 1 );
    va_start( ap, num_args );

    for( i = 0; i < num_args; i++ )
    {
        e = va_arg( ap, Element2byte_gen_t );
        memcpy( packet + location, &e.value.bytes, e.template_length );
        location += e.template_length;

        if( ( e.type == E_STRING ) && ( location != ( int ) strlen( packet ) ) )
        {
            NceOSLogError( "String Conversion Error.\n" );
            return NCE_SDK_BINARY_PAYLOAD_ERROR;
        }
    }

    va_end( ap );

    if( location == 1 )
    {
        NceOSLogError( "Conversion Error.\n" );
        return NCE_SDK_BINARY_PAYLOAD_ERROR;
    }

    return location;
}

#endif /* ifdef NCE_ENERGY_SAVER */
