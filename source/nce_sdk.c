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
 * @file nce_sdk.c
 * @brief Implements the user-facing functions in nce_sdk.h.
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 01 Mar 2022
 */
#include "nce_sdk.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define nceTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 50000U )


/**
 * @brief Replace matched patterns in a string.
 *
 * @param[in] orig: the target string to be replaced.
 * @param[in] rep: the string pattern to be replaced.
 * @param[in] with: the string pattern to replace.
 *
 * @return the pointer pointing to the new string after being replaced.
 */
static char * str_replace( char * orig,
                           char * rep,
                           char * with );

/*-----------------------------------------------------------*/

/**
 * @brief Process the certificate to match the requirement
 *
 * @param[in] token: the target token to be processed.
 * @param[in] certificate: the new certificate required.
 * @param[in] end: the string to be ended in the certificate.
 * @param[in] endLen: the length of the end.
 *
 * @return The status of the execution result.
 */


static uint8_t nceProcessCertificate( char * token,
                                      char * certificate,
                                      char end[],
                                      int32_t endLen );
/*-----------------------------------------------------------*/

/**
 * @brief re-initialize MQTT connection information
 * from received onboarding response.
 *
 * @return The status of the execution result.
 */
static uint8_t nceReinitConnParams( char * completeResponse );
/*-----------------------------------------------------------*/

/**
 * @brief re-initialize COAP connection information
 * from received onboarding response.
 *
 * @return The status of the execution result.
 */

static uint8_t nceReinitDtlsParams( char * completeResponse );

/*-----------------------------------------------------------*/

static char * str_replace( char * orig,
                           char * rep,
                           char * with )
{
    char * result = NULL;
    char * ins = NULL;
    char * tmp = NULL;
    int lenRep = 0;
    int lenWith = 0;
    int lenFront = 0;
    int count = 0;

    if( !orig || !rep )
    {
        return NULL;
    }

    lenRep = strlen( rep );

    if( lenRep == 0 )
    {
        return NULL;
    }

    if( !with )
    {
        with = "";
    }

    lenWith = strlen( with );
    ins = orig;

    for( count = 0; ( tmp = strstr( ins, rep ) ); ++count )
    {
        ins = tmp + lenRep;
    }

/* from stdlib.h */
    tmp = result = pvPortMalloc( strlen( orig ) + ( lenWith - lenRep ) * count + 1 );

    if( !result )
    {
        return NULL;
    }

    while( count-- )
    {
        ins = strstr( orig, rep );
        lenFront = ins - orig;
        tmp = strncpy( tmp, orig, lenFront ) + lenFront;
        tmp = strcpy( tmp, with ) + lenWith;
        orig += lenFront + lenRep;
    }

    strcpy( tmp, orig );
    return result;
}
/*-----------------------------------------------------------*/

static uint8_t nceProcessCertificate( char * token,
                                      char * certificate,
                                      char end[],
                                      int32_t endLen )
{
    uint8_t status = EXIT_FAILURE;
    char find[] = "\\n";
    char replaceWith[] = "\n";
    char * location = NULL;
    int32_t strSize = 0;
    int32_t offset = 0;
    char * result;

    /* Process certificate. */
    memcpy( certificate, token + 1, strlen( token ) - 1 );
    result = str_replace( certificate, find, replaceWith );
    memcpy( certificate, result, strlen( certificate ) );
    vPortFree( result );
    offset = ( uint64_t ) &certificate[ 0 ];
    location = strstr( certificate, end );
    strSize = ( uint64_t ) location + endLen - offset;

    certificate[ strSize ] = '\0';

    if( ( int64_t ) sizeof( certificate ) < ( strSize + 1 ) )
    {
        LogError( ( "array size is not enough to hold incoming certificate." ) );
        return status;
    }

    LogDebug( ( "\r\n%s", certificate ) );


    LogInfo( ( "Processing Completed." ) );
    status = EXIT_SUCCESS;
    return status;
}

/*-----------------------------------------------------------*/

static uint8_t nceReinitConnParams( char * completeResponse )
{
    uint8_t status = EXIT_FAILURE;

    int i = 0;
    int32_t strSize = 0;
    char endKey[] = "-----END RSA PRIVATE KEY-----";
    int32_t endKeyLen = sizeof( endKey );
    char endCert[] = "-----END CERTIFICATE-----";
    int32_t endCertLen = sizeof( endCert );

    memset( nceThingName, '\0', sizeof( nceThingName ) );
    memset( nceEndpoint, '\0', sizeof( nceEndpoint ) );
    memset( nceExampleTopic, '\0', sizeof( nceExampleTopic ) );
    memset( rootCA, '\0', sizeof( rootCA ) );
    memset( clientCert, '\0', sizeof( clientCert ) );
    memset( prvKey, '\0', sizeof( prvKey ) );

    if( NULL == completeResponse )
    {
        LogError( ( "input argument is null." ) );
        return status;
    }

    /* Get the first token. */
    char * token = strtok( completeResponse, "," );

    strSize = strlen( token );
    token[ strSize - 1 ] = '\0';

    if( ( int32_t ) sizeof( nceThingName ) < strSize )
    {
        LogError( ( "nceThingName array size is not enough to hold incoming thing name." ) );
        return status;
    }

    /* In the token we have now "ICCID" so we add 1 to start from the first number of ICCID */
    memcpy( nceThingName, token + 1, strSize );
    LogDebug( ( "Thing name is: %s.", nceThingName ) );

    /* Walk through other tokens. */
    while( token != NULL )
    {
        token = strtok( NULL, "," );

        if( i == 0 )
        {
            strSize = strlen( token ) - 2;

            if( sizeof( nceEndpoint ) < ( long unsigned int ) ( strSize + 1 ) )
            {
                LogError( ( "nceEndpoint array size is not enough to hold incoming endpoint." ) );
                return status;
            }

            memcpy( nceEndpoint, token + 1, strSize );
            LogDebug( ( "IoTEndpoint is: %s.", nceEndpoint ) );
        }

        if( i == 2 )
        {
            /* Process root.pem. */
            nceProcessCertificate( token, rootCA, endCert, endCertLen );
        }

        if( i == 3 )
        {
            /* Process client_cert.pem. */
            nceProcessCertificate( token, clientCert, endCert, endCertLen );
        }

        if( i == 4 )
        {
            /* Process client_key.pem. */
            nceProcessCertificate( token, prvKey, endKey, endKeyLen );
        }

        i++;
    }

    /* Initialize MQTT topic. */
    strSize = strlen( nceThingName ) + strlen( "/example/topic" ) + 1;

    if( ( int64_t ) sizeof( nceExampleTopic ) < strSize )
    {
        LogError( ( "nceExampleTopic size is not enough to hold new example topic." ) );
        return status;
    }

    sprintf( nceExampleTopic, "%s/example/topic", nceThingName );

    LogInfo( ( "Connection Info is re-initialized." ) );

    status = EXIT_SUCCESS;
    return status;
}
/*-----------------------------------------------------------*/

static uint8_t nceReinitDtlsParams( char * completeResponse )
{
    uint8_t status = EXIT_FAILURE;
    int32_t strSize = 0;

    memset( psk, '\0', sizeof( psk ) );
    memset( pskIdentity, '\0', sizeof( pskIdentity ) );

    if( NULL == completeResponse )
    {
        LogError( ( "input argument is null." ) );
        return status;
    }

    /* Get the first token. */
    char * token = strtok( completeResponse, "," );

    strSize = strlen( token );
    token[ strSize ] = '\0';

    if( ( int64_t ) sizeof( psk ) < strSize )
    {
        LogError( ( "pre-shared key array size is not enough to hold incoming thing name." ) );
        return status;
    }

    /* In the token we have now "ICCID" so we add 1 to start from the first number of ICCID */
    memcpy( psk, token, strSize );
    LogDebug( ( "pre-shared key is: %s.", psk ) );
    /* Walk through next token */
    token = strtok( NULL, "," );
    strSize = strlen( token );

    if( ( int64_t ) sizeof( pskIdentity ) < ( strSize + 1 ) )
    {
        LogError( ( "pskIdentity array size is not enough to hold incoming endpoint." ) );
        return status;
    }

    memcpy( pskIdentity, token, strSize );
    LogDebug( ( "Pre shared key identity is: %s.", pskIdentity ) );
    LogInfo( ( "Connection Info is re-initialized." ) );

    status = EXIT_SUCCESS;
    return status;
}

/*-----------------------------------------------------------*/

TlsTransportStatus_t nce_connect( tls_api * xtls_api )
{
    TlsTransportStatus_t xNetworkStatus = TLS_TRANSPORT_CONNECT_FAILURE;
    NetworkCredentials_t tNetworkCredentials = { 0 };

    if( xtls_api == NULL )
    {
        LogError( ( "Argument cannot be NULL: xtls_api=%p", ( void * ) xtls_api ) );
        xNetworkStatus = TLS_TRANSPORT_INVALID_PARAMETER;
    }
    else if( xtls_api->pNetworkContext == NULL )
    {
        LogError( ( "Invalid parameter: xtls_api->pNetworkContext is NULL" ) );
        xNetworkStatus = TLS_TRANSPORT_INVALID_PARAMETER;
    }
    else if( xtls_api->conn == NULL )
    {
        LogError( ( "Invalid parameter: xtls_api->conn is NULL" ) );
        xNetworkStatus = TLS_TRANSPORT_INVALID_PARAMETER;
    }
    else if( xtls_api->tlssend == NULL )
    {
        LogError( ( "Invalid parameter: xtls_api->send is NULL" ) );
        xNetworkStatus = TLS_TRANSPORT_INVALID_PARAMETER;
    }
    else if( xtls_api->tlsrecv == NULL )
    {
        LogError( ( "Invalid parameter: xtls_api->send is NULL" ) );
        xNetworkStatus = TLS_TRANSPORT_INVALID_PARAMETER;
    }
    else
    {
        LogInfo( ( "Connecting to 1NCE server." ) );

        /* define networkCredentials if needed offload or onload */
        do
        {
            LogInfo( ( "Creating a TLS connection to %s:%u.",
                       ONBOARDING_ENDPOINT,
                       ONBOARDING_PORT ) );
            /* Attempt to create a mutually authenticated TLS connection. */
            xNetworkStatus = xtls_api->conn( xtls_api->pNetworkContext,
                                             ONBOARDING_ENDPOINT,
                                             ONBOARDING_PORT,
                                             &tNetworkCredentials,
                                             nceTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                             nceTRANSPORT_SEND_RECV_TIMEOUT_MS );
        } while( xNetworkStatus != TLS_TRANSPORT_SUCCESS );
    }

    return xNetworkStatus;
}
/*-----------------------------------------------------------*/

uint8_t nce_onboard( tls_api * xtls_api,
                     char * completeResponse,
                     uint8_t mqtt_onboarding )
{
    uint8_t status = EXIT_FAILURE;

    char packetToSent[ 120 ];
    int32_t sentBytes;
    int32_t recvBytes;

    LogInfo( ( "Start 1NCE device onboarding." ) );

    memset( packetToSent, '\0', 120 * sizeof( char ) );

    if( mqtt_onboarding == 1 )
    {
        sprintf( packetToSent, "GET /device-api/onboarding HTTP/1.1\r\n"
                               "Host: %s\r\n"
                               "Accept: text/csv\r\n\r\n", ONBOARDING_ENDPOINT );
    }
    else
    {
        sprintf( packetToSent, "GET /device-api/onboarding/coap HTTP/1.1\r\n"
                               "Host: %s\r\n"
                               "Accept: text/csv\r\n\r\n", ONBOARDING_ENDPOINT );
    }

    LogInfo( ( "Send onboarding request:\r\n%.*s",
               strlen( packetToSent ),
               packetToSent ) );
    /* Send onboarding request. */
    sentBytes = xtls_api->tlssend( xtls_api->pNetworkContext,
                                   &packetToSent,
                                   strlen( packetToSent ) );

    configASSERT( sentBytes > 0 );

    if( sentBytes <= 0 )
    {
        LogError( ( "Failed to send onboarding request." ) );
        return status;
    }

    /* Receive onboarding response. */
    recvBytes = xtls_api->tlsrecv( xtls_api->pNetworkContext,
                                   &part[ 0 ],
                                   RECV_BUFFER_LEN );

    if( recvBytes < 0 )
    {
        LogError( ( "Failed to receive onboarding response." ) );
        return status;
    }

    LogDebug( ( "Received raw response: %d bytes.", recvBytes ) );
    LogDebug( ( "\r\n%.*s", recvBytes, part ) );
    strcat( completeResponse,
            strstr( part, "Express\r\n\r\n" ) + strlen( "Express\r\n\r\n" ) );
    memset( part, ( int8_t ) '\0', sizeof( part ) );

    while( recvBytes == RECV_BUFFER_LEN )
    {
        recvBytes = xtls_api->tlsrecv( xtls_api->pNetworkContext,
                                       &part[ 0 ],
                                       RECV_BUFFER_LEN );

        if( recvBytes < 0 )
        {
            LogError( ( "Failed to receive onboarding response." ) );
            return status;
        }

        LogDebug( ( "Received raw response: %d bytes.", strlen( part ) ) );
        LogDebug( ( "\r\n%.*s", strlen( part ), part ) );
        strcat( completeResponse, part );
        memset( part, ( int8_t ) '\0', sizeof( part ) );
    }

    LogInfo( ( " Onboarding response is received." ) );

    /* Disconnect onboarding TLS connection. */
    xtls_api->disconn( xtls_api->pNetworkContext );


    status = EXIT_SUCCESS;
    return status;
}

/*-----------------------------------------------------------*/

int nce_generate_BCL_payload( char * type,
                              size_t position,
                              uint8_t * packet,
                              uint8_t selector,
                              void * argument,
                              int location,
                              int * bcl_payload_bytelength )
{
    memcpy( packet, &selector, 1 );

    if( type[ position ] == 'U' )
    {
        unsigned char * bytes = ( unsigned char * ) ( ( ( unsigned long ) argument ) & 0xFF );
        memcpy( packet + location, bytes, bcl_payload_bytelength[ position ] );
        location = location + bcl_payload_bytelength[ position ];
    }

    else if( type[ position ] == 'F' )
    {
        unsigned char * bytes = argument;
        memcpy( packet + location, bytes, bcl_payload_bytelength[ position ] );
        location = location + bcl_payload_bytelength[ position ];
    }
    else if( type[ position ] == 'I' )
    {
        unsigned char * bytes = ( unsigned char * ) ( ( ( long ) argument ) & 0xFF );
        memcpy( packet + location, &bytes, bcl_payload_bytelength[ position ] );
        location = location + bcl_payload_bytelength[ position ];
    }
    else if( type[ position ] == 'S' )
    {
        unsigned char * bytes = argument;
        memcpy( packet + location, bytes, bcl_payload_bytelength[ position ] );
        location = location + bcl_payload_bytelength[ position ];
    }

    return location;
}

/*-------------------------------------------------------------------*/

uint8_t nce_sdk( tls_api * xtls_api,
                 uint8_t mqtt )
{
    uint8_t status = EXIT_FAILURE;
    char completeResponse[ 5000 ];
    TlsTransportStatus_t xNetworkStatus;

    memset( completeResponse, '\0', 5000 );
    /* Create TLS connection for onboarding request. */
    xNetworkStatus = nce_connect( xtls_api );

    if( TLS_TRANSPORT_SUCCESS != xNetworkStatus )
    {
        LogError( ( "Failed to connect to 1NCE server." ) );
        return status;
    }

    status = nce_onboard( xtls_api, completeResponse, mqtt );

    if( status == EXIT_FAILURE )
    {
        LogError( ( "Failed to receive onboarding response." ) );
    }

    if( mqtt == 1 )
    {
        /* Re-initialize MQTT connection information with onboarding information. */
        status = nceReinitConnParams( completeResponse );
    }
    else
    {
        /* Re-initialize DTLS connection information with onboarding information. */
        status = nceReinitDtlsParams( completeResponse );
    }

    return status;
}
