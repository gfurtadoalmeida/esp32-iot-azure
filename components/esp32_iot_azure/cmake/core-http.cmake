### CORE HTTP
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/coreHTTP/CMakeLists.txt

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED)
    message("Core HTTP: added")

    set(CORE_HTTP_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/coreHTTP/source)

    file(GLOB CORE_HTTP_SOURCES
         ${CORE_HTTP_PATH}/*.c
         ${CORE_HTTP_PATH}/dependency/3rdparty/http_parser/http_parser.c
    )

    set(CORE_HTTP_INCLUDE_DIRS
        ${CORE_HTTP_PATH}/include
        ${CORE_HTTP_PATH}/interface
        ${CORE_HTTP_PATH}/dependency/3rdparty/http_parser
    )

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        set_source_files_properties(
        ${CORE_HTTP_PATH}/core_http_client.c
            PROPERTIES COMPILE_FLAGS
            -Wno-stringop-truncation
        )
    endif()
endif()