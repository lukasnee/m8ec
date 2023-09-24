
project(firmware)

enable_language(CXX C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(ROOT ${CMAKE_SOURCE_DIR})

set(CMAKE_AR /usr/bin/arm-none-eabi-ar)

set(PLATFORM_STM32F4xx_INCLUDE
    Core/Inc
    USB_HOST/App
    USB_HOST/Target
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    Middlewares/ST/STM32_USB_Host_Library/Core/Inc
    Middlewares/ST/STM32_USB_Host_Library/Class/AUDIO/Inc
    Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc
    Middlewares/ST/STM32_USB_Host_Library/Class/HID/Inc
    Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc
    Middlewares/ST/STM32_USB_Host_Library/Class/MTP/Inc
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/CMSIS/Include
    ../extern/ILI9341-STM32-HAL
    ../platform/STM32F4xx/include
)

set(PLATFORM_STM32F4xx_SRC
    startup_stm32f411xe.s
    Core/Src/main.c
    Core/Src/freertos.c
    Core/Src/stm32f4xx_it.c
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/stm32f4xx_hal_timebase_tim.c
    USB_HOST/App/usb_host.c
    USB_HOST/Target/usbh_conf.c
    USB_HOST/Target/usbh_platform.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    Core/Src/system_stm32f4xx.c
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c
    Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
    Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
    Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c
    Middlewares/ST/STM32_USB_Host_Library/Class/AUDIO/Src/usbh_audio.c
    Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Src/usbh_cdc.c
    Middlewares/ST/STM32_USB_Host_Library/Class/HID/Src/usbh_hid.c
    Middlewares/ST/STM32_USB_Host_Library/Class/HID/Src/usbh_hid_keybd.c
    Middlewares/ST/STM32_USB_Host_Library/Class/HID/Src/usbh_hid_mouse.c
    Middlewares/ST/STM32_USB_Host_Library/Class/HID/Src/usbh_hid_parser.c
    Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c
    Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c
    Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c
    Middlewares/ST/STM32_USB_Host_Library/Class/MTP/Src/usbh_mtp.c
    Middlewares/ST/STM32_USB_Host_Library/Class/MTP/Src/usbh_mtp_ptp.c
    Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ../extern/ILI9341-STM32-HAL/ILI9341/ili9341_font.c
    ../extern/ILI9341-STM32-HAL/ILI9341/ili9341_gfx.c
    ../extern/ILI9341-STM32-HAL/ILI9341/ili9341.c
    ../platform/STM32F4xx/src/Keys.cpp
)

# TODO option to select platform
set(PLATFORM_INCLUDE ${PLATFORM_STM32F4xx_INCLUDE} )
set(PLATFORM_SRC ${PLATFORM_STM32F4xx_SRC})

add_executable(${PROJECT_NAME} 
    ${PLATFORM_SRC}
    ../extern/freertos-addons/c++/Source/cmutex.cpp
    ../extern/freertos-addons/c++/Source/cqueue.cpp
    ../extern/freertos-addons/c++/Source/csemaphore.cpp
    ../extern/freertos-addons/c++/Source/cthread.cpp
    ../extern/fonas/src/fonas.cpp
    ../extern/fonas/src/EventDrivenReaderWriter.cpp
    ../extern/fonas/src/EventDrivenWriter.cpp
    ../extern/fonas/src/StreamBuffer.cpp
    ../source/m8ec.cpp
    ../source/m8_protocol.cpp
    ../source/slip.c
    ../source/periph/Uart.cpp
    ../source/periph/Uart1.cpp
    ../source/periph/UsbCdc.cpp
    ../source/KeysThread.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F411xE
    -DSTM32F4
    -DCPP_FREERTOS_NO_EXCEPTIONS
    -DCPP_FREERTOS_NO_CPP_STRINGS
    -DFONAS
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PLATFORM_INCLUDE}
    ../extern/freertos-addons/c++/Source/include
    ../extern/fonas/include
    ../include
)

set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${MCU_FLAGS}
    -fstack-usage
    -Wall
    -Wextra
    -fdata-sections
    -ffunction-sections

    # -fno-use-cxa-atexit
    # -fno-threadsafe-statics
    # -fno-rtti
    
    -fno-exceptions # big change in code size and performace

    # -c -x assembler-with-cpp 

    # For good debug experience, Arm recommends -O1 rather than -O0. When using 
    # -O1, the compiler performs certain optimizations, but the structure of the 
    # generated code is still close to the source code.
    # -g0	no debug information
    # -g1	minimal debug information
    # -g	default debug information
    # -g3	maximal debug information

    $<$<CONFIG:Debug>:-ggdb -Og -DDEBUG>

    # -O3 compiler tries to reduce code size and execution time, O3 is highest standard optimization.
    # -Os Optimize for size. -Os enables all -O2 optimizations except those that often increase code size
    $<$<CONFIG:Release>:-Ofast>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_CURRENT_LIST_DIR}/STM32F411CEUx_FLASH.ld
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fsingle-precision-constant

    -u _printf_float # enables floating point in printf
    
    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
    --specs=nosys.specs 
    -static
    -Wl,--gc-sections

    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--cref
    -Wl,--print-memory-usage
)

target_link_libraries(${PROJECT_NAME}
)

# Create hex file and print executable size
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-objdump -S ${PROJECT_NAME} > ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.dump
    COMMAND arm-none-eabi-nm ${PROJECT_NAME} -C -n -S -s > ${PROJECT_NAME}.address-sort.nm
    COMMAND arm-none-eabi-nm ${PROJECT_NAME} -C -S -s --size-sort > ${PROJECT_NAME}.size-sort.nm
    COMMAND arm-none-eabi-size ${PROJECT_NAME}
)
