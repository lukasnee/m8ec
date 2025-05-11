#include "m8ec/m8/protocol.hpp"

#include "fonas/fonas.hpp"

#include "main.h"
#include "stm32h7xx_hal.h"

#define ARRAY_SZ(x) (sizeof(x) / sizeof((x)[0]))

namespace m8ec::m8::protocol {

struct Config {
    static constexpr std::size_t gpio_transient_delay_ms = 1;
};

const uint16_t cols[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
const uint16_t rows[] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6};

const Keys::Key key_map[ARRAY_SZ(rows)][ARRAY_SZ(cols)] = {
    {Keys::Key::none, Keys::Key::up, Keys::Key::option, Keys::Key::edit},
    {Keys::Key::left, Keys::Key::down, Keys::Key::right, Keys::Key::none},
    {Keys::Key::none, Keys::Key::shift, Keys::Key::play, Keys::Key::none},
};

bool Keys::Svc::ll_init() {

    GPIO_InitTypeDef GPIO_InitStruct = {};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    // key cols
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // key rows
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    return true;
}

Keys::State Keys::Svc::ll_get_state() {
    State state = {};
    // keyboard matrix with diodes
    for (size_t row_i = 0; row_i < ARRAY_SZ(rows); ++row_i) {
        HAL_GPIO_WritePin(GPIOD, rows[row_i], GPIO_PIN_RESET);
        fonas::delay_ms(Config::gpio_transient_delay_ms);
        for (size_t col_i = 0; col_i < ARRAY_SZ(cols); ++col_i) {
            const bool is_pressed = HAL_GPIO_ReadPin(GPIOD, cols[col_i]) == GPIO_PIN_RESET;
            state.set(key_map[row_i][col_i], is_pressed);
        }
        HAL_GPIO_WritePin(GPIOD, rows[row_i], GPIO_PIN_SET);
    }
    return state;
}

} // namespace m8ec::m8::protocol
