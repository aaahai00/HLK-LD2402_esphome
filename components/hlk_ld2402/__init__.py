# 导入 ESPHome 代码生成模块
import esphome.codegen as cg
# 导入 ESPHome 配置验证模块
import esphome.config_validation as cv
# 从 ESPHome 组件中导入 UART 和文本传感器
from esphome.components import uart, text_sensor
# 从 ESPHome 常量中导入必要的配置项
from esphome.const import CONF_ID, CONF_TIMEOUT, ENTITY_CATEGORY_DIAGNOSTIC

# 确保 text_sensor 被列为直接依赖项
DEPENDENCIES = ["uart", "text_sensor"]
# 自动加载 sensor 和 binary_sensor 组件（从 AUTO_LOAD 中移除了 text_sensor）
AUTO_LOAD = ["sensor", "binary_sensor"]

# 定义自定义常量
CONF_MAX_DISTANCE = "max_distance"  # 最大距离配置项
CONF_HLK_LD2402_ID = "hlk_ld2402_id"  # HLK LD2402 组件 ID 配置项
CONF_ENGINEERING_THROTTLE = "engineering_throttle"  # 工程数据节流时间配置项

# 创建 HLK LD2402 命名空间
hlk_ld2402_ns = cg.esphome_ns.namespace("hlk_ld2402")
# 定义 HLKLD2402Component 类，继承自 cg.Component 和 uart.UARTDevice
HLKLD2402Component = hlk_ld2402_ns.class_(
    "HLKLD2402Component", cg.Component, uart.UARTDevice
)

# 使组件在其他平台上正确可见和可用
MULTI_CONF = True

# 主组件配置模式
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HLKLD2402Component),  # 生成组件 ID
    cv.Optional(CONF_MAX_DISTANCE, default=5.0): cv.float_range(min=0.7, max=10.0),  # 最大距离，默认 5.0 米，范围 0.7-10.0 米
    cv.Optional(CONF_TIMEOUT, default=5): cv.int_range(min=0, max=65535),  # 超时时间，默认 5 秒，范围 0-65535 秒
    cv.Optional(CONF_ENGINEERING_THROTTLE): cv.positive_time_period_milliseconds,  # 工程数据节流时间，可选
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

# 将配置转换为代码的异步函数
async def to_code(config):
    # 创建新的组件变量
    var = cg.new_Pvariable(config[CONF_ID])
    # 注册组件
    await cg.register_component(var, config)
    # 注册 UART 设备
    await uart.register_uart_device(var, config)
    
    # 如果配置中包含最大距离，则设置
    if CONF_MAX_DISTANCE in config:
        cg.add(var.set_max_distance(config[CONF_MAX_DISTANCE]))
    # 如果配置中包含超时时间，则设置
    if CONF_TIMEOUT in config:
        cg.add(var.set_timeout(config[CONF_TIMEOUT]))
    # 如果配置中包含工程数据节流时间，则设置
    if CONF_ENGINEERING_THROTTLE in config:
        cg.add(var.set_engineering_throttle(config[CONF_ENGINEERING_THROTTLE]))

# 服务在 services.yaml 文件中定义，由 ESPHome 自动加载