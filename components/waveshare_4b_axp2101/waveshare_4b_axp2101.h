#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/automation.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace waveshare_4b_axp2101 {

class Waveshare4BAxp2101 : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;

  bool read_reg(uint8_t reg, uint8_t *value);
  bool write_reg(uint8_t reg, uint8_t value);
  bool set_reg_bit(uint8_t reg, uint8_t mask, bool state);

  float get_setup_priority() const override {
    // Po I2C, ale bez konieczności bycia przed LCD.
    return setup_priority::DATA;
  }

 protected:
  void dump_selected_regs_();
  void dump_range_(uint8_t start, uint8_t end);
};


template<typename... Ts> class SetRegBitAction : public Action<Ts...> {
 public:
  explicit SetRegBitAction(Waveshare4BAxp2101 *parent) : parent_(parent) {}

  void set_reg(uint8_t reg) {
    this->reg_ = reg;
  }

  void set_mask(uint8_t mask) {
    this->mask_ = mask;
  }

  void set_state(bool state) {
    this->state_ = state;
  }

  void play(Ts... x) override {
    this->parent_->set_reg_bit(this->reg_, this->mask_, this->state_);
  }

 protected:
  Waveshare4BAxp2101 *parent_;
  uint8_t reg_{0};
  uint8_t mask_{0};
  bool state_{false};
};


template<typename... Ts> class WriteRegAction : public Action<Ts...> {
 public:
  explicit WriteRegAction(Waveshare4BAxp2101 *parent) : parent_(parent) {}

  void set_reg(uint8_t reg) {
    this->reg_ = reg;
  }

  void set_value(uint8_t value) {
    this->value_ = value;
  }

  void play(Ts... x) override {
    this->parent_->write_reg(this->reg_, this->value_);
  }

 protected:
  Waveshare4BAxp2101 *parent_;
  uint8_t reg_{0};
  uint8_t value_{0};
};

}  // namespace waveshare_4b_axp2101
}  // namespace esphome
