# es7210.h
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "es7210_const.h"

namespace esphome {
namespace es7210 {

class ES7210Component : public i2c::I2CDevice, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void set_sample_rate(uint32_t sample_rate) { this->sample_rate_ = sample_rate; }
  void set_bits_per_sample(uint8_t bits_per_sample) { this->bits_per_sample_ = bits_per_sample; }
  void set_mic_gain(uint8_t gain) { this->mic_gain_ = gain; }
  void set_mic_bias(bool enable) { this->mic_bias_ = enable; }
  void enable_microphone(uint8_t mic_number) { this->enabled_mics_[mic_number - 1] = true; }

 protected:
  bool write_register(uint8_t reg, uint8_t value);
  bool read_register(uint8_t reg, uint8_t *value);
  bool init_audio_codec();
  bool set_mic_gain_all();
  bool set_mic_bias_all();
  
  uint32_t sample_rate_{16000};
  uint8_t bits_per_sample_{16};
  uint8_t mic_gain_{0};
  bool mic_bias_{true};
  bool enabled_mics_[4] = {false, false, false, false};
};

}  // namespace es7210
}  // namespace esphome
