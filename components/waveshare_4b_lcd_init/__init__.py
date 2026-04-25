import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as automation
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]

waveshare_ns = cg.esphome_ns.namespace("waveshare_4b_lcd_init")

Waveshare4BLcdInit = waveshare_ns.class_(
    "Waveshare4BLcdInit",
    cg.Component,
    i2c.I2CDevice,
)

SetBacklightAction = waveshare_ns.class_(
    "SetBacklightAction",
    automation.Action,
)

SetExpanderPinAction = waveshare_ns.class_(
    "SetExpanderPinAction",
    automation.Action,
)

CONF_BACKLIGHT = "backlight"
CONF_PIN = "pin"
CONF_STATE = "state"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Waveshare4BLcdInit),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x20))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


@automation.register_action(
    "waveshare_4b_lcd_init.set_backlight",
    SetBacklightAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(Waveshare4BLcdInit),
            cv.Required(CONF_BACKLIGHT): cv.boolean,
        }
    ),
)
async def set_backlight_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_backlight(config[CONF_BACKLIGHT]))
    return var


@automation.register_action(
    "waveshare_4b_lcd_init.set_expander_pin",
    SetExpanderPinAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(Waveshare4BLcdInit),
            cv.Required(CONF_PIN): cv.int_range(min=0, max=7),
            cv.Required(CONF_STATE): cv.boolean,
        }
    ),
)
async def set_expander_pin_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_state(config[CONF_STATE]))
    return var
