#include "unity.h"
#include "nce_iot_c_sdk.h"

/* Sample socket ID */
#define SAMPLE_TLS_SOCKET    0

/* Sample responses */
char SAMPLE_RESPONSE_SUCCESS[] = "Express\r\n\r\npreSharedKey,KEY,clientIdentity,ID";
char SAMPLE_RESPONSE_FAILURE[] = "Express\r\n\r\nUnauthorized";


/* Sample Network definitions */
struct OSNetwork
{
    int os_socket;
};

struct OSNetwork xOSNetwork = { .os_socket = 0 };

DtlsKey_t nceKey = { 0 };

/**
 * @brief Mocked tls connect returning a socket id.
 */
int tls_connect_mock_success( OSNetwork_t osnetwork,
                              OSEndPoint_t nce_oboarding )
{
    osnetwork->os_socket = SAMPLE_TLS_SOCKET;
    return 0;
}

/**
 * @brief Mocked tls connect returning an error.
 */
int tls_connect_mock_failure( OSNetwork_t osnetwork,
                              OSEndPoint_t nce_oboarding )
{
    return -1;
}

/**
 * @brief Mocked tls send returning the number of bytes to be sent.
 */
int tls_send_mock( OSNetwork_t osnetwork,
                   void * pBuffer,
                   size_t bytesToSend )
{
    return bytesToSend;
}

/**
 * @brief Mocked tls recv returning SUCCESS response from the server.
 */
int tls_recv_mock_success( OSNetwork_t osnetwork,
                           void * pBuffer,
                           size_t bytesToRecv )
{
    bytesToRecv = strlen( SAMPLE_RESPONSE_SUCCESS );
    memcpy( pBuffer, SAMPLE_RESPONSE_SUCCESS, strlen( SAMPLE_RESPONSE_SUCCESS ) );
    return bytesToRecv;
}

/**
 * @brief Mocked tls recv returning FAILURE response from the server.
 */
int tls_recv_mock_failure( OSNetwork_t osnetwork,
                           void * pBuffer,
                           size_t bytesToRecv )
{
    bytesToRecv = strlen( SAMPLE_RESPONSE_FAILURE );
    memcpy( pBuffer, SAMPLE_RESPONSE_FAILURE, strlen( SAMPLE_RESPONSE_FAILURE ) );
    return bytesToRecv;
}


/**
 * @brief Mocked tls disconnect returning success.
 */
int tls_disconnect_mock( OSNetwork_t osnetwork )
{
    return 0;
}

void setUp( void )
{
}


void tearDown( void )
{
}


/**
 * @brief Test 1 ( successful authentication ).
 */
void test_os_auth_success_response( void )
{
    os_network_ops_t osNetwork =
    {
        .os_socket             = &xOSNetwork,
        .nce_os_tls_connect    = tls_connect_mock_success,
        .nce_os_tls_send       = tls_send_mock,
        .nce_os_tls_recv       = tls_recv_mock_success,
        .nce_os_tls_disconnect = tls_disconnect_mock
    };

    TEST_ASSERT_EQUAL_INT( os_auth( &osNetwork, &nceKey ), NCE_SDK_SUCCESS );
}

/**
 * @brief Test 2 ( failed authentication due to connection failure ).
 */
void test_os_auth_connect_failure( void )
{
    os_network_ops_t osNetwork =
    {
        .os_socket             = &xOSNetwork,
        .nce_os_tls_connect    = tls_connect_mock_failure,
        .nce_os_tls_send       = tls_send_mock,
        .nce_os_tls_recv       = tls_recv_mock_failure,
        .nce_os_tls_disconnect = tls_disconnect_mock
    };

    TEST_ASSERT_EQUAL_INT( os_auth( &osNetwork, &nceKey ), NCE_SDK_CONNECT_ERROR );
}

/**
 * @brief Test 3 ( failed authentication due to missing security credentials ).
 */
void test_os_auth_failure_response( void )
{
    os_network_ops_t osNetwork =
    {
        .os_socket             = &xOSNetwork,
        .nce_os_tls_connect    = tls_connect_mock_success,
        .nce_os_tls_send       = tls_send_mock,
        .nce_os_tls_recv       = tls_recv_mock_failure,
        .nce_os_tls_disconnect = tls_disconnect_mock
    };

    TEST_ASSERT_EQUAL_INT( os_auth( &osNetwork, &nceKey ), NCE_SDK_PARSING_ERROR );
}


/**
 * @brief Test 4 ( successful binary conversion ).
 */
void test_os_energy_save_success( void )
{
    Element2byte_gen_t battery_level = { .type = E_INTEGER, .value.i = 99, .template_length = 1 };
    Element2byte_gen_t signal_strength = { .type = E_INTEGER, .value.i = 84, .template_length = 1 };
    Element2byte_gen_t software_version = { .type = E_STRING, .value.s = "2.2.1", .template_length = 5 };
    uint8_t selector = 1;
    char pcTransmittedString[ 50 ];

    memset( pcTransmittedString, '\0', 50 );
    int expected_location = 8;

    TEST_ASSERT_EQUAL_INT( os_energy_save( pcTransmittedString, selector, 3, battery_level, signal_strength, software_version ), expected_location );
}

/**
 * @brief Test 5 ( invalid binary conversion - the provided string size is not equal to the template length - ).
 */
void test_os_energy_save_failure( void )
{
    Element2byte_gen_t software_version = { .type = E_STRING, .value.s = "1.1", .template_length = 5 };

    uint8_t selector = 1;
    char pcTransmittedString[ 50 ];

    memset( pcTransmittedString, '\0', 50 );

    TEST_ASSERT_EQUAL_INT( os_energy_save( pcTransmittedString, selector, 2, software_version ), NCE_SDK_BINARY_PAYLOAD_ERROR );
}
