#
# Copyright (c) 2022 1NCE
# 1NCE IoT C SDK (ZEPHYR OS Module)
#

set(NCE_SDK_DIR ${ZEPHYR_CURRENT_MODULE_DIR})
zephyr_library()
zephyr_include_directories(${NCE_SDK_DIR}/source/include)
zephyr_include_directories(${NCE_SDK_DIR}/source/interface)
zephyr_library_sources(
	${NCE_SDK_DIR}/source/nce_iot_c_sdk.c
)

zephyr_compile_definitions_ifdef(CONFIG_NCE_DEVICE_AUTHENTICATOR NCE_DEVICE_AUTHENTICATOR)
zephyr_compile_definitions_ifdef(CONFIG_NCE_ENERGY_SAVER NCE_ENERGY_SAVER)
zephyr_compile_definitions(
    NCE_SDK_CONNECT_ATTEMPTS=${CONFIG_NCE_SDK_CONNECT_ATTEMPTS})
