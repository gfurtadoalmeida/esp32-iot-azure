cmake_minimum_required(VERSION 3.13)

set(EXTRA_COMPONENT_DIRS "../components")
set(TEST_COMPONENTS "esp32_iot_azure" CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_runner)

