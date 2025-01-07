#
# Copyright (c) 2024 1NCE
# 1NCE IoT C SDK (ZEPHYR OS Module)
#


set(NCE_SDK_ROOT ../..)

zephyr_library()
zephyr_include_directories(${NCE_SDK_ROOT}/source/include)
zephyr_include_directories(${NCE_SDK_ROOT}/source/interface)
zephyr_library_sources(
	${NCE_SDK_ROOT}/source/nce_iot_c_sdk.c
)

zephyr_compile_definitions_ifdef(CONFIG_NCE_DEVICE_AUTHENTICATOR NCE_DEVICE_AUTHENTICATOR)
zephyr_compile_definitions_ifdef(CONFIG_NCE_ENERGY_SAVER NCE_ENERGY_SAVER)

if(CONFIG_NCE_MEMFAULT_INTERFACE)
set(COAP_DIR ${ZEPHYR_BASE}/include/zephyr/net)
set(MEMFAULT_SDK_DIR ${ZEPHYR_BASE}/../modules/lib/memfault-firmware-sdk)
zephyr_include_directories(${MEMFAULT_SDK_DIR}/components/include/)
zephyr_include_directories(${MEMFAULT_SDK_DIR}/components/include/core)
zephyr_include_directories(${COAP_DIR})
zephyr_library_sources(memfault_interface_zephyr.c)
endif()

zephyr_compile_definitions(
    NCE_SDK_ATTEMPTS=${CONFIG_NCE_SDK_ATTEMPTS})
zephyr_compile_definitions(
    NCE_SDK_MAX_STRING_SIZE=${CONFIG_NCE_SDK_MAX_STRING_SIZE})
            
zephyr_library_sources_ifdef(CONFIG_NCE_SDK_NETWORK_INTERFACE network_interface_zephyr.c)
zephyr_library_sources_ifdef(CONFIG_NCE_SDK_COAP_INTERFACE coap_interface_zephyr.c)
zephyr_library_sources_ifdef(CONFIG_NCE_SDK_COAP_INTERFACE coap_interface_zephyr_utils.c)
zephyr_include_directories(include)
