/**
 * @file memfault_interface_zephyr.h
 * @brief Implements Memfault chunk sending via 1NCE COAP Proxy for ZEPHYR OS.
 * @date 07 November 2024
 *
 */


#define PROXY_HOST             "coap.proxy.os.1nce.com"

#ifdef CONFIG_NCE_SDK_ENABLE_DTLS
    #define PROXY_PORT         5684
#else
    #define PROXY_PORT         5683
#endif /* ifdef CONFIG_NCE_SDK_ENABLE_DTLS */

#define RECEIVE_BUFFER_SIZE    128

/**
 * @brief Send Memfault data with retries.
 *
 * This function attempts to send Memfault data via the `prv_os_memfault_try_send` function.
 * If sending fails, it will retry for a maximum of `NCE_SDK_MEMFAULT_ATTEMPTS`.
 *
 * @return int 0 on success, negative error code on failure.
 */
int os_memfault_send( void );
