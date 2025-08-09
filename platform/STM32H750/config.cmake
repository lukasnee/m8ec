include_guard(GLOBAL)

add_library(STM32H7xx_HAL_conf INTERFACE)
target_include_directories(
  STM32H7xx_HAL_conf
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Core/Inc # stm32h7xx_hal_conf.h
)

add_library(freertos_config INTERFACE)
target_include_directories(
  freertos_config INTERFACE ${CMAKE_CURRENT_LIST_DIR}/config
)
# target_link_libraries(freertos_config INTERFACE STM32H7xx_HAL ucprof)
set(FREERTOS_PORT
    GCC_ARM_CM7
    CACHE STRING ""
)
set(FREERTOS_HEAP 4)

if(SEGGER_SYSVIEW_ENABLED)
  add_library(SeggerSysView_config STATIC)
  target_sources(
    SeggerSysView_config
    PRIVATE config/SEGGER_SYSVIEW/SEGGER_SYSVIEW_Config_FreeRTOS.c
            config/SEGGER_SYSVIEW/SEGGER_SYSVIEW_FreeRTOS.c
  )
  target_include_directories(
    SeggerSysView_config PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config/SEGGER_SYSVIEW
  )
  target_link_libraries(SeggerSysView_config PRIVATE freertos_kernel)
  target_compile_options(
    SeggerSysView_config
    PRIVATE
      -finstrument-functions
      -finstrument-functions-exclude-file-list=SEGGER_SYSVIEW_FreeRTOS,SEGGER_SYSVIEW_Config_FreeRTOS
  )
else()
  add_library(SeggerSysView_config INTERFACE)
endif()
