# 导入 ESPHome 代码生成模块
import esphome.codegen as cg
# 导入 ESPHome 配置验证模块
import esphome.config_validation as cv
# 从 ESPHome 组件中导入传感器
from esphome.components import sensor
# 从 ESPHome 常量中导入必要的配置项
from esphome.const import (
    CONF_ID,  # 组件 ID
    DEVICE_CLASS_DISTANCE,  # 距离设备类别
    STATE_CLASS_MEASUREMENT,  # 测量状态类别
    UNIT_CENTIMETER,  # 厘米单位
    UNIT_PERCENT,  # 百分比单位
    ENTITY_CATEGORY_DIAGNOSTIC,  # 诊断实体类别
)

# 从本模块导入 HLKLD2402Component 和 CONF_HLK_LD2402_ID
from . import HLKLD2402Component, CONF_HLK_LD2402_ID

# 定义配置项
CONF_THROTTLE = "throttle"  # 节流时间
CONF_DISTANCE_CHANGE_THRESHOLD = "distance_change_threshold"  # 距离变化阈值
CONF_CALIBRATION_PROGRESS = "calibration_progress"  # 校准进度
CONF_ENERGY_GATE = "energy_gate"  # 运动能量门传感器（前16个gate）
CONF_STILL_ENERGY_GATE = "still_energy_gate"  # 静止能量门传感器（后16个gate）
CONF_GATE_INDEX = "gate_index"     # 门编号（0-15）
CONF_MOTION_THRESHOLD = "motion_threshold"  # 运动阈值传感器
CONF_MICROMOTION_THRESHOLD = "micromotion_threshold"  # 微动阈值传感器

# 更新配置模式以包含静止能量门传感器
CONFIG_SCHEMA = sensor.sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(sensor.Sensor),  # 生成传感器 ID
    cv.Required(CONF_HLK_LD2402_ID): cv.use_id(HLKLD2402Component),  # 必需的 HLK LD2402 组件 ID
    cv.Optional(CONF_THROTTLE): cv.positive_time_period_milliseconds,  # 可选的节流时间
    cv.Optional(CONF_DISTANCE_CHANGE_THRESHOLD): cv.float_range(min=0.1, max=1000.0),  # 距离变化阈值，单位厘米
    cv.Optional(CONF_CALIBRATION_PROGRESS, default=False): cv.boolean,  # 可选的校准进度，默认为 False
    cv.Optional(CONF_ENERGY_GATE): cv.Schema({  # 可选的能量门配置
        cv.Required(CONF_GATE_INDEX): cv.int_range(0, 15),  # 必需的门索引（0-15）
    }),
    # 添加静止能量门配置
    cv.Optional(CONF_STILL_ENERGY_GATE): cv.Schema({  # 可选的静止能量门配置
        cv.Required(CONF_GATE_INDEX): cv.int_range(0, 15),  # 必需的门索引（0-15）
    }),
    cv.Optional(CONF_MOTION_THRESHOLD): cv.Schema({  # 可选的运动阈值配置
        cv.Required(CONF_GATE_INDEX): cv.int_range(0, 15),  # 必需的门索引（0-15）
    }),
    cv.Optional(CONF_MICROMOTION_THRESHOLD): cv.Schema({  # 可选的微动阈值配置
        cv.Required(CONF_GATE_INDEX): cv.int_range(0, 15),  # 必需的门索引（0-15）
    }),
})

# 将配置转换为代码的异步函数
async def to_code(config):
    # 获取父组件变量
    parent = await cg.get_variable(config[CONF_HLK_LD2402_ID])
    # 创建新的传感器
    var = await sensor.new_sensor(config)
    
    # 如果配置了能量门
    if CONF_ENERGY_GATE in config:
        gate_index = config[CONF_ENERGY_GATE][CONF_GATE_INDEX]
        # 设置能量门传感器
        cg.add(parent.set_energy_gate_sensor(gate_index, var))
    # 添加静止能量门传感器处理
    elif CONF_STILL_ENERGY_GATE in config:
        gate_index = config[CONF_STILL_ENERGY_GATE][CONF_GATE_INDEX]
        # 设置静止能量门传感器
        cg.add(parent.set_still_energy_gate_sensor(gate_index, var))
    # 如果配置了运动阈值
    elif CONF_MOTION_THRESHOLD in config:
        gate_index = config[CONF_MOTION_THRESHOLD][CONF_GATE_INDEX]
        # 设置运动阈值传感器
        cg.add(parent.set_motion_threshold_sensor(gate_index, var))
    # 如果配置了微动阈值
    elif CONF_MICROMOTION_THRESHOLD in config:
        gate_index = config[CONF_MICROMOTION_THRESHOLD][CONF_GATE_INDEX]
        # 设置微动阈值传感器
        cg.add(parent.set_micromotion_threshold_sensor(gate_index, var))
    # 如果配置了校准进度
    elif config.get(CONF_CALIBRATION_PROGRESS):
        # 这是一个校准进度传感器
        cg.add(parent.set_calibration_progress_sensor(var))
    else:
        # 这是一个常规距离传感器
        cg.add(parent.set_distance_sensor(var))
        # 如果配置了节流时间
        if CONF_THROTTLE in config:
            cg.add(parent.set_distance_throttle(config[CONF_THROTTLE]))
        # 如果配置了距离变化阈值
        if CONF_DISTANCE_CHANGE_THRESHOLD in config:
            cg.add(parent.set_distance_change_threshold(config[CONF_DISTANCE_CHANGE_THRESHOLD]))