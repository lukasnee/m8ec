#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief The entry point of the application.
   */
  void platform_app();

  /**
   * @brief Prints FreeRTOS statistics.
   * @param buff Buffer containing the statistics.
   */
  void platform_print_freertos_stats(const char *buff);

#ifdef __cplusplus
}
#endif
