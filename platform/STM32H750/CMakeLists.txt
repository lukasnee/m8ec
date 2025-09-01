include(config.cmake)

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
            USB_HOST/App/usb_host.c
            USB_HOST/Target/usbh_conf.c
            Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c
            Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
            Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
            Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c
            Middlewares/ST/STM32_USB_Host_Library/Class/AUDIO/Src/usbh_audio.c
            Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Src/usbh_cdc.c
)

target_include_directories(
  platform
  INTERFACE Core/Inc
            USB_HOST/App
            USB_HOST/Target
            Middlewares/ST/STM32_USB_Host_Library/Core/Inc
            Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc
            Middlewares/ST/STM32_USB_Host_Library/Class/AUDIO/Inc
)

if(SEGGER_SYSVIEW_ENABLED)
  target_compile_options(
    platform
    INTERFACE
      -finstrument-functions
      -finstrument-functions-exclude-file-list=Core/Src/system_stm32h7xx,Core/Src/main,Core/Src/dma,Core/Src/freertos,Core/Src/gpio,Core/Src/i2s,Core/Src/spi,Core/Src/usart,Core/Src/stm32h7xx_it,Core/Src/stm32h7xx_hal_msp,Core/Src/stm32h7xx_hal_timebase_tim
  )
endif()

target_link_options(
  platform INTERFACE
  -T${PROJECT_SOURCE_DIR}/extern/STM32H7-DevEBox/linker/bl_iram_app.ld
)

target_link_libraries(
  platform
  INTERFACE CMSIS
            STM32H7xx_HAL
            freertos_kernel
            CMSIS_RTOS_V2
            ucprof
)
