#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace waveshare_4b_lcd_init {

class Waveshare4BLcdInit : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;

  float get_setup_priority() const override {
    // Ma wystartować przed display.mipi_rgb.
    return 850.0f;
  }

 protected:
  uint8_t output_state_{0xFF};
  uint8_t config_state_{0xFF};

  bool pca_write_(uint8_t reg, uint8_t value);
  void pca_pin_mode_output_(uint8_t pin);
  void pca_set_pin_(uint8_t pin, bool value);

  void lcd_reset_sequence_();

  void spi_delay_();
  void spi_write9_(bool is_data, uint8_t value);
  void lcd_cmd_(uint8_t cmd);
  void lcd_data_(uint8_t data);
  void lcd_cmd_data_(uint8_t cmd, const uint8_t *data, size_t len);

  void st7701_init_();
};

}  // namespace waveshare_4b_lcd_init
}  // namespace esphome
