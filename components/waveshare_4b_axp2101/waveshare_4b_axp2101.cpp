#include "waveshare_4b_axp2101.h"

namespace esphome {
namespace waveshare_4b_axp2101 {

static const char *const TAG = "waveshare_4b_axp2101";

void Waveshare4BAxp2101::setup() {
  ESP_LOGI(TAG, "AXP2101 diagnostic component setup");

  uint8_t value = 0;
  if (!this->read_reg(0x00, &value)) {
    ESP_LOGW(TAG, "Could not read AXP2101 reg 0x00. Check I2C address 0x34.");
    return;
  }

  ESP_LOGI(TAG, "AXP2101 is responding. REG[0x00] = 0x%02X", value);

  this->dump_selected_regs_();
}

void Waveshare4BAxp2101::dump_config() {
  ESP_LOGCONFIG(TAG, "Waveshare ESP32-S3-Touch-LCD-4B AXP2101 diagnostic");
  LOG_I2C_DEVICE(this);
}

bool Waveshare4BAxp2101::read_reg(uint8_t reg, uint8_t *value) {
  if (value == nullptr) {
    return false;
  }

  bool ok = this->read_byte(reg, value);

  if (!ok) {
    ESP_LOGW(TAG, "Read failed REG[0x%02X]", reg);
    return false;
  }

  return true;
}

bool Waveshare4BAxp2101::write_reg(uint8_t reg, uint8_t value) {
  ESP_LOGI(TAG, "WRITE REG[0x%02X] = 0x%02X", reg, value);

  bool ok = this->write_byte(reg, value);

  if (!ok) {
    ESP_LOGW(TAG, "Write failed REG[0x%02X] = 0x%02X", reg, value);
    return false;
  }

  uint8_t verify = 0;
  if (this->read_reg(reg, &verify)) {
    ESP_LOGI(TAG, "VERIFY REG[0x%02X] = 0x%02X", reg, verify);
  }

  return true;
}

bool Waveshare4BAxp2101::set_reg_bit(uint8_t reg, uint8_t mask, bool state) {
  uint8_t value = 0;

  if (!this->read_reg(reg, &value)) {
    return false;
  }

  uint8_t old_value = value;

  if (state) {
    value |= mask;
  } else {
    value &= ~mask;
  }

  ESP_LOGI(
    TAG,
    "SET BIT REG[0x%02X] mask 0x%02X state %s: 0x%02X -> 0x%02X",
    reg,
    mask,
    state ? "ON" : "OFF",
    old_value,
    value
  );

  return this->write_reg(reg, value);
}

void Waveshare4BAxp2101::dump_selected_regs_() {
  ESP_LOGI(TAG, "Dump selected AXP2101 registers");

  // Status / common PMU area.
  this->dump_range_(0x00, 0x03);

  // Power / output enable area used by many AXP2101 drivers.
  // Nie zakładamy jeszcze znaczenia bitów — tylko logujemy.
  this->dump_range_(0x80, 0x95);

  // Voltage/control area often used around LDO/DCDC config.
  this->dump_range_(0x96, 0xA8);
}

void Waveshare4BAxp2101::dump_range_(uint8_t start, uint8_t end) {
  for (uint16_t r = start; r <= end; r++) {
    uint8_t value = 0;

    if (this->read_reg(static_cast<uint8_t>(r), &value)) {
      ESP_LOGI(TAG, "REG[0x%02X] = 0x%02X", static_cast<uint8_t>(r), value);
    }

    delay(2);
  }
}

}  // namespace waveshare_4b_axp2101
}  // namespace esphome
