# es7210.cpp
#include "es7210.h"
#include "esphome/core/log.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

void ES7210Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES7210...");
  
  if (!this->init_audio_codec()) {
    ESP_LOGE(TAG, "Failed to initialize ES7210");
    this->mark_failed();
    return;
  }

  if (!this->set_mic_gain_all()) {
    ESP_LOGE(TAG, "Failed to set microphone gain");
    this->mark_failed();
    return;
  }

  if (!this->set_mic_bias_all()) {
    ESP_LOGE(TAG, "Failed to set microphone bias");
    this->mark_failed();
    return;
  }
}

void ES7210Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ES7210:");
  ESP_LOGCONFIG(TAG, "  Sample Rate: %u", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  Bits Per Sample: %u", this->bits_per_sample_);
  ESP_LOGCONFIG(TAG, "  Mic Gain: %u", this->mic_gain_);
  ESP_LOGCONFIG(TAG, "  Mic Bias: %s", this->mic_bias_ ? "ON" : "OFF");
  ESP_LOGCONFIG(TAG, "  Enabled Mics: %s%s%s%s",
                this->enabled_mics_[0] ? "1 " : "",
                this->enabled_mics_[1] ? "2 " : "",
                this->enabled_mics_[2] ? "3 " : "",
                this->enabled_mics_[3] ? "4" : "");
  LOG_I2C_DEVICE(this);
}

bool ES7210Component::write_register(uint8_t reg, uint8_t value) {
  return this->write_byte(reg, value);
}

bool ES7210Component::read_register(uint8_t reg, uint8_t *value) {
  return this->read_byte(reg, value);
}

bool ES7210Component::init_audio_codec() {
  // Reset ES7210
  if (!this->write_register(ES7210_RESET_REG00, 0xFF))
    return false;
  delay(10);
  if (!this->write_register(ES7210_RESET_REG00, 0x41))
    return false;
  
  // Find matching coefficient for sample rate
  const ES7210Coefficient *coeff = nullptr;
  for (const auto &c : ES7210_COEFFICIENTS) {
    if (c.lrclk == this->sample_rate_) {
      coeff = &c;
      break;
    }
  }
  
  if (coeff == nullptr) {
    ESP_LOGE(TAG, "Unsupported sample rate: %u", this->sample_rate_);
    return false;
  }

  // Configure clock settings
  if (!this->write_register(ES7210_MAINCLK_REG02, coeff->adc_div))
    return false;
  if (!this->write_register(ES7210_MASTER_CLK_REG03, coeff->mclk_src))
    return false;
  if (!this->write_register(ES7210_LRCK_DIVH_REG04, coeff->lrck_h))
    return false;
  if (!this->write_register(ES7210_LRCK_DIVL_REG05, coeff->lrck_l))
    return false;
  
  // Configure format (I2S)
  if (!this->write_register(ES7210_SDP_INTERFACE1_REG11, 0x00))
    return false;
  
  return true;
}

bool ES7210Component::set_mic_gain_all() {
  const uint8_t gain_regs[] = {
    ES7210_MIC1_GAIN_REG43,
    ES7210_MIC2_GAIN_REG44,
    ES7210_MIC3_GAIN_REG45,
    ES7210_MIC4_GAIN_REG46
  };

  for (int i = 0; i < 4; i++) {
    if (this->enabled_mics_[i]) {
      if (!this->write_register(gain_regs[i], this->mic_gain_))
        return false;
    }
  }
  
  return true;
}

bool ES7210Component::set_mic_bias_all() {
  if (!this->write_register(ES7210_MIC12_BIAS_REG41, this->mic_bias_ ? 0x77 : 0x00))
    return false;
  if (!this->write_register(ES7210_MIC34_BIAS_REG42, this->mic_bias_ ? 0x77 : 0x00))
    return false;
  
  return true;
}

}  // namespace es7210
}  // namespace esphome
