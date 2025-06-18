project(platform)

add_library(platform INTERFACE)

target_sources(
  platform
  INTERFACE startup_stm32h750xx.s
            Core/Src/system_stm32h7xx.c
            Core/Src/main.c
            Core/Src/dma.c
            Core/Src/freertos.c
            Core/Src/gpio.c
            Core/Src/i2s.c
            Core/Src/spi.c
            Core/Src/tim.c
            Core/Src/usart.c
            Core/Src/stm32h7xx_it.c
            Core/Src/stm32h7xx_hal_msp.c
            Core/Src/stm32h7xx_hal_timebase_tim.c
)

target_include_directories(platform INTERFACE Core/Inc)

if(SEGGER_SYSVIEW_ENABLED)
  target_compile_options(
    platform
    INTERFACE
      -finstrument-functions
      -finstrument-functions-exclude-file-list=Core/Src/system_stm32h7xx,Core/Src/main,Core/Src/dma,Core/Src/freertos,Core/Src/gpio,Core/Src/i2s,Core/Src/spi,Core/Src/usart,Core/Src/stm32h7xx_it,Core/Src/stm32h7xx_hal_msp,Core/Src/stm32h7xx_hal_timebase_tim
  )
endif()

target_link_options(
  platform INTERFACE -T${CMAKE_CURRENT_LIST_DIR}/STM32H750VBTx_RAM.ld
  -Wl,--no-warn-rwx-segment
)

add_library(STM32H7xx_HAL_conf INTERFACE)
target_include_directories(
  STM32H7xx_HAL_conf INTERFACE Core/Inc # stm32h7xx_hal_conf.h
)

add_library(SeggerSysView_config INTERFACE)
target_link_libraries(
  SeggerSysView_config INTERFACE SeggerSysView_config_STM32H750
)
add_subdirectory(config/SEGGER_SYSVIEW)

add_library(freertos_config INTERFACE)
target_include_directories(
  freertos_config
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/config
            Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)
target_sources(
  freertos_config
  PUBLIC Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
         Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
)
if(SEGGER_SYSVIEW_ENABLED)
  target_compile_options(
    freertos_config
    INTERFACE
      -finstrument-functions-exclude-file-list=Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4,Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port
  )
endif()
target_link_libraries(freertos_config INTERFACE STM32H7xx_HAL ucprof)
add_subdirectory(Middlewares/Third_Party/FreeRTOS)

target_link_libraries(
  platform
  INTERFACE CMSIS
            STM32H7xx_HAL
            ucprof
            freertos_kernel
            CherryUSB
)
