add_executable(m8ec)

target_sources(
  m8ec
  PRIVATE syscalls.cpp
          drivers.cpp
          services.cpp
          m8ec.cpp
          m8/protocol.cpp
          m8/protocol/DisplayAdaptorILI9341.cpp
          drivers/DisplayILI9341.cpp
          m8/protocol/slip.c
          periph/UartCallbacks.cpp
          periph/Uart4.cpp
          periph/SpiCallbacks.cpp
          periph/Spi1.cpp
          periph/UsbCdc.cpp
          platform/${PLATFORM}/Keys.cpp
)

target_include_directories(m8ec PUBLIC ../include)

target_link_libraries(
  m8ec
  PUBLIC ln::ln
         platform
         SeggerSysView
         lfsapp_W25Q64
         ILI9341-STM32-HAL
)

generate_firmware_artifacts(m8ec)

add_custom_target(
  m8ec_flash
  DEPENDS m8ec
  COMMAND extern/STM32H7-DevEBox/.venv/bin/python3
          ${PROJECT_SOURCE_DIR}/tools/m8ec.py -fr
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Flashing the device"
  VERBATIM USES_TERMINAL
)

if(SEGGER_SYSVIEW_ENABLED)
  target_compile_options(
    m8ec
    PRIVATE
      -finstrument-functions
      -finstrument-functions-exclude-file-list=stdio,periph/UartCallbacks,periph/SpiCallbacks
  )
endif()
