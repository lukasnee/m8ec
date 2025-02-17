cmake_minimum_required(VERSION 3.16)

project(m8ec)

enable_language(CXX C ASM)

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
add_subdirectory(extern/ILI9341-STM32-HAL)
add_subdirectory(extern/lua)

add_subdirectory(source)
