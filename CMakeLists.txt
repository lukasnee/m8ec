cmake_minimum_required(VERSION 4.0)

project(m8ec LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

include(platform/${PLATFORM}/config.cmake)

# Platform independent libraries:
add_subdirectory(extern/ln)
add_subdirectory(extern/ucprof)

# Platform specific libraries:
add_subdirectory(extern/STM32H7-DevEBox)
add_subdirectory(platform/${PLATFORM})
add_subdirectory(extern/ILI9341-STM32-HAL)

# Application:
add_subdirectory(source)
