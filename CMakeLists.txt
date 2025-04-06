cmake_minimum_required(VERSION 4.0)

project(m8ec
LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
  -fstack-usage
  -Wall
  -Wextra
  -fno-exceptions # big change in code size and performace
  # https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
  # https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
  -g
  $<$<CONFIG:Debug>:-gdwarf-2>
  $<$<CONFIG:Debug>:-Og>
  $<$<CONFIG:Release>:-O1>
)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_library(ucprof_config INTERFACE)
target_include_directories(
  ucprof_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/config
)

add_subdirectory(platform/${PLATFORM})
add_subdirectory(extern/W25Q64_STM32H750VB-DevEBox)

add_subdirectory(extern/ucprof)
add_subdirectory(extern/freertos-addons)
add_subdirectory(extern/fonas)

set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL true)
set(LV_CONF_BUILD_DISABLE_EXAMPLES true)
set(LV_CONF_BUILD_DISABLE_DEMOS true)
add_library(lvgl_config INTERFACE)
target_compile_options(lvgl_config INTERFACE -Wno-unused-parameter) # remove
                                                                    # noise
target_include_directories(
  lvgl_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/source/config # lv_conf.h
)
target_link_libraries(lvgl_config INTERFACE freertos_kernel)
add_subdirectory(extern/lvgl)
target_link_libraries(lvgl PUBLIC lvgl_config)

add_subdirectory(source)
