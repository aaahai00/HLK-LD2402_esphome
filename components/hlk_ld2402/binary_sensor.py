# 导入 ESPHome 代码生成模块
import esphome.codegen as cg
# 导入 ESPHome 配置验证模块
import esphome.config_validation as cv
# 从 ESPHome 组件中导入二进制传感器
from esphome.components import binary_sensor
# 从 ESPHome 常量中导入必要的配置项
from esphome.const import (
    CONF_ID,  # 组件 ID
    CONF_DEVICE_CLASS,  # 设备类别
    DEVICE_CLASS_PRESENCE,  # 存在检测设备类别
    DEVICE_CLASS_MOTION,  # 运动检测设备类别
    DEVICE_CLASS_PROBLEM,  # 问题检测设备类别
)

# 从本模块导入 HLKLD2402Component 和 CONF_HLK_LD2402_ID
from . import HLKLD2402Component, CONF_HLK_LD2402_ID

# 定义传感器类型
CONF_POWER_INTERFERENCE = "power_interference"  # 电源干扰传感器

# 定义二进制传感器的配置模式 - 通过扩展基础模式开始
CONFIG_SCHEMA = binary_sensor.binary_sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),  # 生成传感器 ID
    cv.Required(CONF_HLK_LD2402_ID): cv.use_id(HLKLD2402Component),  # 必需的 HLK LD2402 组件 ID
    cv.Optional(CONF_DEVICE_CLASS): cv.one_of(DEVICE_CLASS_PRESENCE, DEVICE_CLASS_MOTION, DEVICE_CLASS_PROBLEM),  # 可选的设备类别
    cv.Optional(CONF_POWER_INTERFERENCE, default=False): cv.boolean,  # 可选的电源干扰检测，默认为 False
})

# 将配置转换为代码的异步函数
async def to_code(config):
    # 获取父组件变量
    parent = await cg.get_variable(config[CONF_HLK_LD2402_ID])
    # 创建新的二进制传感器
    var = await binary_sensor.new_binary_sensor(config)
    
    # 如果配置了电源干扰检测
    if config.get(CONF_POWER_INTERFERENCE, False):
        # 设置电源干扰二进制传感器
        cg.add(parent.set_power_interference_binary_sensor(var))
    # 否则，如果配置了设备类别
    elif CONF_DEVICE_CLASS in config:
        # 根据设备类别设置相应的传感器
        if config[CONF_DEVICE_CLASS] == DEVICE_CLASS_PRESENCE:
            # 设置存在检测二进制传感器
            cg.add(parent.set_presence_binary_sensor(var))
        elif config[CONF_DEVICE_CLASS] == DEVICE_CLASS_MOTION:
            # 设置微动检测二进制传感器
            cg.add(parent.set_micromovement_binary_sensor(var))