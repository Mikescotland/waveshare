#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/output/float_output.h"

#include "driver/ledc.h"
#include "esp_err.h"

namespace esphome {
namespace waveshare_4b_backlight {

class Waveshare4BBacklightOutput : public Component, public output::FloatOutput {
 public:
  void setup() override;
  void dump_config() override;

 protected:
  void write_state(float state) override;

  static constexpr gpio_num_t BACKLIGHT_GPIO = GPIO_NUM_4;

  static constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
  static constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_1;
  static constexpr ledc_channel_t LEDC_CHANNEL = LEDC_CHANNEL_1;

  static constexpr uint32_t LEDC_FREQ_HZ = 5000;
  static constexpr ledc_timer_bit_t LEDC_RESOLUTION = LEDC_TIMER_10_BIT;
  static constexpr uint32_t LEDC_MAX_DUTY = 1023;

  float last_state_{1.0f};
};

}  // namespace waveshare_4b_backlight
}  // namespace esphome
