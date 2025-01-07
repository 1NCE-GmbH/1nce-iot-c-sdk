#ifndef LOG_INTERFACE_H_
#define LOG_INTERFACE_H_

/**
 *  @brief implementation of logging macros for FreeRTOS.
 */
#ifdef FREERTOS
    #include "FreeRTOSConfig.h"
void vLoggingPrintf( const char * pcFormat,
                     ... );

    #define NceOSLogInfo( format, ... )     vLoggingPrintf( format, ## __VA_ARGS__ )
    #define NceOSLogDebug( format, ... )    vLoggingPrintf( format, ## __VA_ARGS__ )
    #define NceOSLogError( format, ... )    vLoggingPrintf( format, ## __VA_ARGS__ )
    #define NceOSLogWarn( format, ... )     vLoggingPrintf( format, ## __VA_ARGS__ )

#elif defined( __ZEPHYR__ )
    #include <zephyr/logging/log.h>

/**
 *  @brief implementation of logging macros for ZEPHYR OS.
 */
    #define NceOSLogInfo( ... )     LOG_INF( __VA_ARGS__ )
    #define NceOSLogDebug( ... )    LOG_DBG( __VA_ARGS__ )
    #define NceOSLogError( ... )    LOG_ERR( __VA_ARGS__ )
    #define NceOSLogWarn( ... )     LOG_WRN( __VA_ARGS__ )

#elif defined( ARDUINO )

/**
 *  @brief implementation of logging macros for Arduino.
 */
    #define NceOSLogInfo( ... )         printf( __VA_ARGS__ )
    #define NceOSLogError( ... )        printf( __VA_ARGS__ )
    #define NceOSLogWarn( ... )         printf( __VA_ARGS__ )
    #ifdef NCE_SDK_LOG_LEVEL_DEBUG
        #define NceOSLogDebug( ... )    printf( __VA_ARGS__ )
    #else
        #define NceOSLogDebug( format, ... )
    #endif

#else /* ifdef FREERTOS */

/**
 *  @brief Define the abbreviated logging macros.
 */
    #define NceOSLogInfo( format, ... )
    #define NceOSLogDebug( format, ... )
    #define NceOSLogError( format, ... )
    #define NceOSLogWarn( format, ... )

#endif /* ifdef FREERTOS */
#endif /* ifndef LOG_INTERFACE_H_ */
