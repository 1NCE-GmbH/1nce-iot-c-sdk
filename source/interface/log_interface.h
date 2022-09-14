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

#else 

/**
 *  @brief Define the abbreviated logging macros.
 */
    #define NceOSLogInfo( format, ... )
    #define NceOSLogDebug( format, ... )
    #define NceOSLogError( format, ... )
    #define NceOSLogWarn( format, ... )

#endif /* ifdef FREERTOS */
#endif /* ifndef LOG_INTERFACE_H_ */
