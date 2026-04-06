# 导入 ESPHome 代码生成模块
import esphome.codegen as cg
# 导入 ESPHome 配置验证模块
import esphome.config_validation as cv
# 从 ESPHome 组件中导入文本传感器
from esphome.components import text_sensor
# 从 ESPHome 常量中导入必要的配置项
from esphome.const import (
    CONF_ID,  # 组件 ID
    ENTITY_CATEGORY_DIAGNOSTIC,  # 诊断实体类别
)

# 从本模块导入 HLKLD2402Component 和 CONF_HLK_LD2402_ID
from . import HLKLD2402Component, CONF_HLK_LD2402_ID

# 定义文本传感器类型
CONF_FIRMWARE_VERSION = "firmware_version"  # 固件版本
CONF_OPERATING_MODE = "operating_mode"  # 操作模式

# 定义带有可选传感器类型的配置模式
CONFIG_SCHEMA = text_sensor.text_sensor_schema(
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC  # 设置实体类别为诊断
).extend({
    cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),  # 生成传感器 ID
    cv.Required(CONF_HLK_LD2402_ID): cv.use_id(HLKLD2402Component),  # 必需的 HLK LD2402 组件 ID
    cv.Optional(CONF_FIRMWARE_VERSION, default=False): cv.boolean,  # 可选的固件版本，默认为 False
    cv.Optional(CONF_OPERATING_MODE, default=False): cv.boolean,  # 可选的操作模式，默认为 False
})

# 将配置转换为代码的异步函数
async def to_code(config):
    # 获取父组件变量
    parent = await cg.get_variable(config[CONF_HLK_LD2402_ID])
    # 创建新的文本传感器
    var = await text_sensor.new_text_sensor(config)
    
    # 如果配置了固件版本
    if config.get(CONF_FIRMWARE_VERSION):
        # 设置固件版本文本传感器
        cg.add(parent.set_firmware_version_text_sensor(var))
    # 如果配置了操作模式
    elif config.get(CONF_OPERATING_MODE):
        # 设置操作模式文本传感器
        cg.add(parent.set_operating_mode_text_sensor(var))