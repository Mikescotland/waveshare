import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as automation
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]

waveshare_ns = cg.esphome_ns.namespace("waveshare_4b_axp2101")

Waveshare4BAxp2101 = waveshare_ns.class_(
    "Waveshare4BAxp2101",
    cg.Component,
    i2c.I2CDevice,
)

SetRegBitAction = waveshare_ns.class_(
    "SetRegBitAction",
    automation.Action,
)

WriteRegAction = waveshare_ns.class_(
    "WriteRegAction",
    automation.Action,
)

CONF_REGISTER = "reg"
CONF_MASK = "mask"
CONF_STATE = "state"
CONF_VALUE = "value"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Waveshare4BAxp2101),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x34))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


@automation.register_action(
    "waveshare_4b_axp2101.set_reg_bit",
    SetRegBitAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(Waveshare4BAxp2101),
            cv.Required(CONF_REGISTER): cv.hex_uint8_t,
            cv.Required(CONF_MASK): cv.hex_uint8_t,
            cv.Required(CONF_STATE): cv.boolean,
        }
    ),
)
async def set_reg_bit_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_reg(config[CONF_REGISTER]))
    cg.add(var.set_mask(config[CONF_MASK]))
    cg.add(var.set_state(config[CONF_STATE]))
    return var


@automation.register_action(
    "waveshare_4b_axp2101.write_reg",
    WriteRegAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(Waveshare4BAxp2101),
            cv.Required(CONF_REGISTER): cv.hex_uint8_t,
            cv.Required(CONF_VALUE): cv.hex_uint8_t,
        }
    ),
)
async def write_reg_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_reg(config[CONF_REGISTER]))
    cg.add(var.set_value(config[CONF_VALUE]))
    return var
