cmake_minimum_required(VERSION 4.0)

project(m8ec LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

include(platform/${PLATFORM}/config.cmake)

add_compile_options(-fstack-usage -Wall -Wextra -Werror)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_library(ucprof_config INTERFACE)
target_include_directories(
  ucprof_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/config
)

# Platform independent libraries:
add_subdirectory(extern/ln)
add_subdirectory(extern/ucprof)

# Platform specific libraries:
add_subdirectory(extern/STM32H7-DevEBox)
add_subdirectory(platform/${PLATFORM})
add_subdirectory(extern/ILI9341-STM32-HAL)

# Application:
add_subdirectory(source)
