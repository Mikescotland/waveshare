#include "waveshare_4b_lcd_init.h"

namespace esphome {
namespace waveshare_4b_lcd_init {

static const char *const TAG = "waveshare_4b_lcd_init";

// Według Waveshare ESP-IDF BSP:
// CS  = expander pin 0
// SDA = expander pin 1
// SCL = expander pin 2
static constexpr uint8_t PIN_CS = 0;
static constexpr uint8_t PIN_SDA = 1;
static constexpr uint8_t PIN_SCL = 2;

// Z Arduino przykładu:
// expander pin 6 LOW
// expander pin 5 reset LOW -> HIGH
static constexpr uint8_t PIN_LCD_RST = 5;
static constexpr uint8_t PIN_LCD_PWR = 6;

// PCA9554 registers
static constexpr uint8_t REG_INPUT = 0x00;
static constexpr uint8_t REG_OUTPUT = 0x01;
static constexpr uint8_t REG_POLARITY = 0x02;
static constexpr uint8_t REG_CONFIG = 0x03;

bool Waveshare4BLcdInit::pca_write_(uint8_t reg, uint8_t value) {
  return this->write_byte(reg, value);
}

void Waveshare4BLcdInit::pca_pin_mode_output_(uint8_t pin) {
  this->config_state_ &= ~(1 << pin);
  this->pca_write_(REG_CONFIG, this->config_state_);
}

void Waveshare4BLcdInit::pca_set_pin_(uint8_t pin, bool value) {
  if (value) {
    this->output_state_ |= (1 << pin);
  } else {
    this->output_state_ &= ~(1 << pin);
  }

  this->pca_write_(REG_OUTPUT, this->output_state_);
}

void Waveshare4BLcdInit::set_backlight(bool on) {
  ESP_LOGI(TAG, "Backlight %s", on ? "ON" : "OFF");

  // Na tej płytce pin wygląda na aktywne LOW,
  // bo w przykładzie Arduino pin 6 ustawiony LOW włączał ekran/backlight.
  this->pca_set_pin_(PIN_LCD_PWR, !on);
}

void Waveshare4BLcdInit::spi_delay_() {
  delayMicroseconds(2);
}

void Waveshare4BLcdInit::spi_write9_(bool is_data, uint8_t value) {
  // 3-wire / 9-bit SPI:
  // pierwszy bit = D/C, potem 8 bitów danych, MSB first.

  this->pca_set_pin_(PIN_SDA, is_data);
  this->spi_delay_();
  this->pca_set_pin_(PIN_SCL, true);
  this->spi_delay_();
  this->pca_set_pin_(PIN_SCL, false);
  this->spi_delay_();

  for (int bit = 7; bit >= 0; bit--) {
    this->pca_set_pin_(PIN_SDA, (value >> bit) & 0x01);
    this->spi_delay_();
    this->pca_set_pin_(PIN_SCL, true);
    this->spi_delay_();
    this->pca_set_pin_(PIN_SCL, false);
    this->spi_delay_();
  }
}

void Waveshare4BLcdInit::lcd_cmd_(uint8_t cmd) {
  this->pca_set_pin_(PIN_CS, false);
  this->spi_write9_(false, cmd);
  this->pca_set_pin_(PIN_CS, true);
  delayMicroseconds(10);
}

void Waveshare4BLcdInit::lcd_data_(uint8_t data) {
  this->pca_set_pin_(PIN_CS, false);
  this->spi_write9_(true, data);
  this->pca_set_pin_(PIN_CS, true);
  delayMicroseconds(10);
}

void Waveshare4BLcdInit::lcd_cmd_data_(uint8_t cmd, const uint8_t *data, size_t len) {
  this->pca_set_pin_(PIN_CS, false);

  this->spi_write9_(false, cmd);

  for (size_t i = 0; i < len; i++) {
    this->spi_write9_(true, data[i]);
  }

  this->pca_set_pin_(PIN_CS, true);
  delayMicroseconds(20);
}

void Waveshare4BLcdInit::lcd_reset_sequence_() {
  ESP_LOGI(TAG, "LCD reset sequence");

  this->pca_pin_mode_output_(PIN_CS);
  this->pca_pin_mode_output_(PIN_SDA);
  this->pca_pin_mode_output_(PIN_SCL);
  this->pca_pin_mode_output_(PIN_LCD_RST);
  this->pca_pin_mode_output_(PIN_LCD_PWR);

  this->pca_set_pin_(PIN_CS, true);
  this->pca_set_pin_(PIN_SCL, false);
  this->pca_set_pin_(PIN_SDA, false);

  // Jak w Arduino:
  // expander->digitalWrite(6, LOW);
  // delay(200);
  // expander->digitalWrite(5, LOW);
  // delay(200);
  // expander->digitalWrite(5, HIGH);
  // delay(200);

  this->pca_set_pin_(PIN_LCD_PWR, false);
  delay(200);

  this->pca_set_pin_(PIN_LCD_RST, false);
  delay(200);

  this->pca_set_pin_(PIN_LCD_RST, true);
  delay(200);
}

void Waveshare4BLcdInit::st7701_init_() {
  ESP_LOGI(TAG, "Sending ST7701 init over PCA9554 3-wire SPI");

  {
    const uint8_t d[] = {0x77, 0x01, 0x00, 0x00, 0x10};
    this->lcd_cmd_data_(0xFF, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x3B, 0x00};
    this->lcd_cmd_data_(0xC0, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x0D, 0x02};
    this->lcd_cmd_data_(0xC1, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x21, 0x08};
    this->lcd_cmd_data_(0xC2, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x08};
    this->lcd_cmd_data_(0xCD, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x00, 0x11, 0x18, 0x0E,
      0x11, 0x06, 0x07, 0x08,
      0x07, 0x22, 0x04, 0x12,
      0x0F, 0xAA, 0x31, 0x18
    };
    this->lcd_cmd_data_(0xB0, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x00, 0x11, 0x19, 0x0E,
      0x12, 0x07, 0x08, 0x08,
      0x08, 0x22, 0x04, 0x11,
      0x11, 0xA9, 0x32, 0x18
    };
    this->lcd_cmd_data_(0xB1, d, sizeof(d));
  }

  {
    const uint8_t d[] = {0x77, 0x01, 0x00, 0x00, 0x11};
    this->lcd_cmd_data_(0xFF, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x60};
    this->lcd_cmd_data_(0xB0, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x30};
    this->lcd_cmd_data_(0xB1, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x87};
    this->lcd_cmd_data_(0xB2, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x80};
    this->lcd_cmd_data_(0xB3, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x49};
    this->lcd_cmd_data_(0xB5, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x85};
    this->lcd_cmd_data_(0xB7, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x21};
    this->lcd_cmd_data_(0xB8, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x78};
    this->lcd_cmd_data_(0xC1, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x78};
    this->lcd_cmd_data_(0xC2, d, sizeof(d));
  }

  delay(20);

  {
    const uint8_t d[] = {0x00, 0x1B, 0x02};
    this->lcd_cmd_data_(0xE0, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x08, 0xA0, 0x00, 0x00,
      0x07, 0xA0, 0x00, 0x00,
      0x00, 0x44, 0x44
    };
    this->lcd_cmd_data_(0xE1, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x11, 0x11, 0x44, 0x44,
      0xED, 0xA0, 0x00, 0x00,
      0xEC, 0xA0, 0x00, 0x00
    };
    this->lcd_cmd_data_(0xE2, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x00, 0x00, 0x11, 0x11};
    this->lcd_cmd_data_(0xE3, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x44, 0x44};
    this->lcd_cmd_data_(0xE4, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x0A, 0xE9, 0xD8, 0xA0,
      0x0C, 0xEB, 0xD8, 0xA0,
      0x0E, 0xED, 0xD8, 0xA0,
      0x10, 0xEF, 0xD8, 0xA0
    };
    this->lcd_cmd_data_(0xE5, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x00, 0x00, 0x11, 0x11};
    this->lcd_cmd_data_(0xE6, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x44, 0x44};
    this->lcd_cmd_data_(0xE7, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0x09, 0xE8, 0xD8, 0xA0,
      0x0B, 0xEA, 0xD8, 0xA0,
      0x0D, 0xEC, 0xD8, 0xA0,
      0x0F, 0xEE, 0xD8, 0xA0
    };
    this->lcd_cmd_data_(0xE8, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40};
    this->lcd_cmd_data_(0xEB, d, sizeof(d));
  }
  {
    const uint8_t d[] = {0x3C, 0x00};
    this->lcd_cmd_data_(0xEC, d, sizeof(d));
  }
  {
    const uint8_t d[] = {
      0xAB, 0x89, 0x76, 0x54,
      0x02, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0x20,
      0x45, 0x67, 0x98, 0xBA
    };
    this->lcd_cmd_data_(0xED, d, sizeof(d));
  }

  {
    const uint8_t d[] = {0x77, 0x01, 0x00, 0x00, 0x00};
    this->lcd_cmd_data_(0xFF, d, sizeof(d));
  }

  {
    const uint8_t d[] = {0x00};
    this->lcd_cmd_data_(0x36, d, sizeof(d));
  }

  {
    const uint8_t d[] = {0x66};  // RGB666 / 18-bit
    this->lcd_cmd_data_(0x3A, d, sizeof(d));
  }

  this->lcd_cmd_(0x21);  // invert on
  this->lcd_cmd_(0x11);  // sleep out
  delay(120);

  this->lcd_cmd_(0x29);  // display on
  delay(20);

  ESP_LOGI(TAG, "ST7701 init done");
}

void Waveshare4BLcdInit::setup() {
  ESP_LOGI(TAG, "Setup start");

  this->lcd_reset_sequence_();
  this->st7701_init_();

  ESP_LOGI(TAG, "Setup complete");
}

void Waveshare4BLcdInit::dump_config() {
  ESP_LOGCONFIG(TAG, "Waveshare ESP32-S3-Touch-LCD-4B LCD init component");
  LOG_I2C_DEVICE(this);
}

}  // namespace waveshare_4b_lcd_init
}  // namespace esphome
