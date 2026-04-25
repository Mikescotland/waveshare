import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

waveshare_ns = cg.esphome_ns.namespace("waveshare_4b_backlight")

Waveshare4BBacklightOutput = waveshare_ns.class_(
    "Waveshare4BBacklightOutput",
    cg.Component,
    output.FloatOutput,
)

CONFIG_SCHEMA = (
    output.FLOAT_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Waveshare4BBacklightOutput),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)
