# __init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import (
    CONF_ID,
    CONF_SAMPLE_RATE,
    CONF_BITS_PER_SAMPLE,
)

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor"]

es7210_ns = cg.esphome_ns.namespace("es7210")
ES7210Component = es7210_ns.class_("ES7210Component", i2c.I2CDevice)

# Custom constants
CONF_MIC_GAIN = "mic_gain"
CONF_MIC_BIAS = "mic_bias"
CONF_MIC_SELECT = "mic_select"

# Validation for microphone selection (1-4)
MIC_SCHEMA = cv.one_of(1, 2, 3, 4)

# Supported sample rates
SAMPLE_RATES = {
    8000: 0,
    11025: 1,
    12000: 2,
    16000: 3,
    22050: 4,
    24000: 5,
    32000: 6,
    44100: 7,
    48000: 8,
    64000: 9,
    88200: 10,
    96000: 11,
}

# Configuration schema
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ES7210Component),
            cv.Required(CONF_SAMPLE_RATE): cv.enum(SAMPLE_RATES, int=True),
            cv.Optional(CONF_BITS_PER_SAMPLE, default=16): cv.one_of(16, 24, 32, int=True),
            cv.Optional(CONF_MIC_GAIN, default=0): cv.int_range(min=0, max=8),
            cv.Optional(CONF_MIC_BIAS, default=True): cv.boolean,
            cv.Optional(CONF_MIC_SELECT, default=[1]): cv.ensure_list(MIC_SCHEMA),
        }
    )
    .extend(i2c.i2c_device_schema(0x40))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    cg.add(var.set_mic_gain(config[CONF_MIC_GAIN]))
    cg.add(var.set_mic_bias(config[CONF_MIC_BIAS]))
    
    for mic in config[CONF_MIC_SELECT]:
        cg.add(var.enable_microphone(mic))
