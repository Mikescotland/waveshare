#include "waveshare_4b_backlight_output.h"

namespace esphome {
namespace waveshare_4b_backlight {

static const char *const TAG = "waveshare_4b_backlight";

void Waveshare4BBacklightOutput::setup() {
  ESP_LOGI(TAG, "Setting up Waveshare 4B backlight using ESP-IDF LEDC");

  ledc_timer_config_t timer_config = {};
  timer_config.speed_mode = LEDC_MODE;
  timer_config.timer_num = LEDC_TIMER;
  timer_config.duty_resolution = LEDC_RESOLUTION;
  timer_config.freq_hz = LEDC_FREQ_HZ;
  timer_config.clk_cfg = LEDC_AUTO_CLK;

  esp_err_t err = ledc_timer_config(&timer_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ledc_timer_config failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  this->configure_channel_(0);

  // Start full brightness.
  this->write_state(this->last_state_);

  ESP_LOGI(
    TAG,
    "Backlight setup complete: GPIO4, 5000Hz, 10-bit, low-speed, timer 1, channel 1"
  );
}

void Waveshare4BBacklightOutput::configure_channel_(uint32_t initial_duty) {
  ledc_channel_config_t channel_config = {};
  channel_config.gpio_num = BACKLIGHT_GPIO;
  channel_config.speed_mode = LEDC_MODE;
  channel_config.channel = LEDC_CHANNEL;
  channel_config.intr_type = LEDC_INTR_DISABLE;
  channel_config.timer_sel = LEDC_TIMER;
  channel_config.duty = initial_duty;
  channel_config.hpoint = 0;

#if ESP_IDF_VERSION_MAJOR >= 5
  channel_config.flags.output_invert = 0;
#endif

  esp_err_t err = ledc_channel_config(&channel_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ledc_channel_config failed: %s", esp_err_to_name(err));
    return;
  }

  this->channel_configured_ = true;
}

void Waveshare4BBacklightOutput::write_state(float state) {
  if (state < 0.0f) {
    state = 0.0f;
  }
  if (state > 1.0f) {
    state = 1.0f;
  }

  this->last_state_ = state;

  // GPIO4 is active-low:
  // LOW  = backlight ON
  // HIGH = backlight OFF
  //
  // For full OFF, do not rely on duty=1023.
  // Stop LEDC and force idle HIGH, otherwise the driver may leave
  // a tiny active-low pulse / leakage-like glow.
  if (state <= 0.001f) {
    ESP_LOGD(TAG, "Backlight OFF -> ledc_stop idle HIGH");

    ledc_stop(LEDC_MODE, LEDC_CHANNEL, 1);

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << BACKLIGHT_GPIO;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(BACKLIGHT_GPIO, 1);

    this->channel_configured_ = false;
    return;
  }

  // If LEDC was stopped for full OFF, reattach the channel.
  if (!this->channel_configured_) {
    this->configure_channel_(0);
  }

  // Waveshare/Brookesia BSP mapping:
  // brightness 100% -> duty 0    -> GPIO effectively LOW  -> full ON
  // brightness 0%   -> hard OFF above
  //
  // For dimming:
  // 80% brightness -> duty about 205
  // 50% brightness -> duty about 512
  // 20% brightness -> duty about 818
  uint32_t duty = static_cast<uint32_t>((1.0f - state) * LEDC_MAX_DUTY + 0.5f);

  if (duty > LEDC_MAX_DUTY) {
    duty = LEDC_MAX_DUTY;
  }

  esp_err_t err = ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ledc_set_duty failed: %s", esp_err_to_name(err));
    return;
  }

  err = ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ledc_update_duty failed: %s", esp_err_to_name(err));
    return;
  }

  ESP_LOGD(TAG, "Backlight state %.3f -> duty %u", state, duty);
}

void Waveshare4BBacklightOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "Waveshare ESP32-S3-Touch-LCD-4B Backlight");
  ESP_LOGCONFIG(TAG, "  GPIO: GPIO4");
  ESP_LOGCONFIG(TAG, "  LEDC mode: low speed");
  ESP_LOGCONFIG(TAG, "  LEDC timer: 1");
  ESP_LOGCONFIG(TAG, "  LEDC channel: 1");
  ESP_LOGCONFIG(TAG, "  Frequency: 5000 Hz");
  ESP_LOGCONFIG(TAG, "  Resolution: 10-bit");
  ESP_LOGCONFIG(TAG, "  Active: LOW");
  ESP_LOGCONFIG(TAG, "  Full OFF: ledc_stop idle HIGH + GPIO HIGH");
}

}  // namespace waveshare_4b_backlight
}  // namespace esphome
