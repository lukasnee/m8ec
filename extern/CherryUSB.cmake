set(repo ${CMAKE_CURRENT_LIST_DIR}/CherryUSB)
add_library(CherryUSB)

target_compile_options(
  CherryUSB PRIVATE -Wno-unused-parameter -Wno-unused-variable
                    -Wno-unused-function -Wno-format
)
target_sources(
  CherryUSB
  PRIVATE ${repo}/core/usbh_core.c ${repo}/osal/usb_osal_freertos.c
          ${repo}/port/dwc2/usb_hc_dwc2.c ${repo}/port/dwc2/usb_glue_st.c
)

target_sources(CherryUSB PRIVATE ${repo}/class/hub/usbh_hub.c)
target_link_libraries(CherryUSB INTERFACE "-u hub_class_info")

target_sources(CherryUSB PRIVATE ${repo}/class/cdc/usbh_cdc_acm.c)
target_link_libraries(CherryUSB INTERFACE "-u cdc_acm_class_info")
target_link_libraries(CherryUSB INTERFACE "-u cdc_data_class_info")

target_sources(CherryUSB PRIVATE ${repo}/class/audio/usbh_audio.c)
target_link_libraries(CherryUSB INTERFACE "-u audio_ctrl_intf_class_info")
target_link_libraries(CherryUSB INTERFACE "-u audio_streaming_intf_class_info")

target_include_directories(
  CherryUSB
  PUBLIC ${repo}/core
         ${repo}/class/cdc
         ${repo}/common
         ${repo}/class/hub
         source/CherryUSB
)

if(NOT TARGET CherryUSB_config)
  message(STATUS "CherryUSB_config not found")
endif()

target_link_libraries(CherryUSB PUBLIC CherryUSB_config freertos_kernel fonas)
