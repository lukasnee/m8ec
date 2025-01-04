add_library(freertos_kernel STATIC)
target_include_directories(freertos_kernel PUBLIC Source/include Source/CMSIS_RTOS_V2)

target_sources(
  freertos_kernel
  PRIVATE Source/croutine.c
          Source/event_groups.c
          Source/list.c
          Source/queue.c
          Source/stream_buffer.c
          Source/tasks.c
          # Source/timers.c
          Source/CMSIS_RTOS_V2/cmsis_os2.c
)

if(NOT TARGET freertos_config)
  message(FATAL_ERROR "freertos_config not found")
endif()
target_link_libraries(freertos_kernel PUBLIC freertos_config)

if(SEGGER_SYSVIEW_ENABLED)
  target_compile_options(
    freertos_kernel
    PRIVATE
      -finstrument-functions
      -finstrument-functions-exclude-file-list=Source/croutine,Source/event_groups,Source/list,Source/queue,Source/stream_buffer,Source/tasks,Source/CMSIS_RTOS_V2/cmsis_os2
  )
endif()
