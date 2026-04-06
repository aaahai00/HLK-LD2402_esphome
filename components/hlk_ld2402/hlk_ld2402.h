#pragma once

// 包含必要的头文件
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"  // 无条件包含

namespace esphome {
namespace hlk_ld2402 {

// 帧头和帧尾定义
static const uint8_t FRAME_HEADER[] = {0xFD, 0xFC, 0xFB, 0xFA};  // 帧头
static const uint8_t FRAME_FOOTER[] = {0x04, 0x03, 0x02, 0x01};  // 帧尾

// 添加新的帧格式常量
static const uint8_t DATA_FRAME_HEADER[] = {0xF4, 0xF3, 0xF2, 0xF1}; // 数据帧头
static const uint8_t DATA_FRAME_FOOTER[] = {0xF8, 0xF7, 0xF6, 0xF5}; // 数据帧尾
static const uint8_t DATA_FRAME_TYPE_DISTANCE = 0x83; // 距离数据帧类型
static const uint8_t DATA_FRAME_TYPE_ENGINEERING = 0x84; // 工程数据帧类型

// 命令定义
static const uint16_t CMD_GET_VERSION = 0x0000;  // 读取固件版本命令
static const uint16_t CMD_ENABLE_CONFIG = 0x00FF;  // 启用配置模式
static const uint16_t CMD_DISABLE_CONFIG = 0x00FE;  // 结束配置模式
static const uint16_t CMD_GET_SN_HEX = 0x0016;  // 读取序列号（十六进制格式）
static const uint16_t CMD_GET_SN_CHAR = 0x0011;  // 读取序列号（字符格式）
static const uint16_t CMD_GET_PARAMS = 0x0008;  // 读取参数
static const uint16_t CMD_SET_PARAMS = 0x0007;  // 设置参数
static const uint16_t CMD_SET_MODE = 0x0012;  // 设置数据输出模式
static const uint16_t CMD_START_CALIBRATION = 0x0009;  // 开始自动阈值生成
static const uint16_t CMD_GET_CALIBRATION_STATUS = 0x000A;  // 查询校准进度
static const uint16_t CMD_CALIBRATION_INTERFERENCE = 0x0014;  // 报告校准干扰
static const uint16_t CMD_SAVE_PARAMS = 0x00FD;  // 保存参数到闪存
static const uint16_t CMD_AUTO_GAIN = 0x00EE;  // 自动增益调整
static const uint16_t CMD_AUTO_GAIN_COMPLETE = 0x00F0;  // 自动增益完成通知

// 参数定义
static const uint16_t PARAM_MAX_DISTANCE = 0x0001;  // 最大检测距离
static const uint16_t PARAM_TIMEOUT = 0x0004;  // 目标消失延迟
static const uint16_t PARAM_POWER_INTERFERENCE = 0x0005;  // 电源干扰状态（只读）
static const uint16_t PARAM_TRIGGER_THRESHOLD = 0x0010;  // 运动触发阈值基址（0x0010-0x001F）
static const uint16_t PARAM_MICRO_THRESHOLD = 0x0030;  // 微动阈值基址（0x0030-0x003F）

// 工作模式
static const uint32_t MODE_PRODUCTION = 0x00000064;  // 正常生产模式
static const uint32_t MODE_NORMAL = 0x00000064;  // 生产模式的别名
static const uint32_t MODE_CONFIG = 0x00000001;  // 配置模式
static const uint32_t MODE_ENGINEERING = 0x00000004;  // 工程/调试模式

// 更新 - 根据手册修正波特率
static const uint32_t UART_BAUD_RATE = 115200;  // UART波特率
static const uint8_t UART_STOP_BITS = 1;  // UART停止位
static const uint8_t UART_DATA_BITS = 8;  // UART数据位
static const esphome::uart::UARTParityOptions UART_PARITY = esphome::uart::UART_CONFIG_PARITY_NONE;  // UART奇偶校验

// 手册中的常量
static constexpr float MAX_THEORETICAL_RANGE = 10.0f;  // 运动的最大理论范围（10米）
static constexpr float MOVEMENT_RANGE = 10.0f;         // 运动的最大范围（10米）
static constexpr float MICROMOVEMENT_RANGE = 6.0f;     // 微动的最大范围（6米）
static constexpr float STATIC_RANGE = 5.0f;            // 静态检测的最大范围（5米）
static constexpr float DISTANCE_PRECISION = 0.15f;     // 距离精度（±0.15米）
static constexpr float DISTANCE_GATE_SIZE = 0.7f;      // 每个门的大小（0.7米）
static const uint8_t MAX_GATES = 32;                   // 硬件最大门数
static const uint8_t DEFAULT_GATES = 16;               // 更新为16以包含门15

// 添加校准系数
static const uint8_t DEFAULT_COEFF = 0x1E;  // 默认系数（3.0）
static const float MIN_COEFF = 1.0f;  // 最小系数
static const float MAX_COEFF = 20.0f;  // 最大系数

// HLK LD2402 组件类，继承自 Component 和 uart::UARTDevice
class HLKLD2402Component : public Component, public uart::UARTDevice {
public:
  // 获取设置优先级，返回硬件级优先级
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  // 设置距离传感器
  void set_distance_sensor(sensor::Sensor *distance_sensor) { distance_sensor_ = distance_sensor; }
  // 设置距离传感器节流时间
  void set_distance_throttle(uint32_t throttle_ms) { distance_throttle_ms_ = throttle_ms; }
  // 设置距离变化阈值
  void set_distance_change_threshold(float threshold_cm) { distance_change_threshold_ = threshold_cm; }
  // 设置存在检测二进制传感器
  void set_presence_binary_sensor(binary_sensor::BinarySensor *presence) { presence_binary_sensor_ = presence; }
  // 设置微动检测二进制传感器
  void set_micromovement_binary_sensor(binary_sensor::BinarySensor *micro) { micromovement_binary_sensor_ = micro; }
  // 设置电源干扰二进制传感器
  void set_power_interference_binary_sensor(binary_sensor::BinarySensor *power_interference) { power_interference_binary_sensor_ = power_interference; }
  // 设置最大检测距离
  void set_max_distance(float max_distance) { max_distance_ = max_distance; }
  // 设置超时时间
  void set_timeout(uint32_t timeout) { timeout_ = timeout; }
  // 设置工程数据节流时间
  void set_engineering_throttle(uint32_t throttle_ms) { engineering_throttle_ms_ = throttle_ms; }
  
  // 设置固件版本文本传感器
  void set_firmware_version_text_sensor(text_sensor::TextSensor *version_sensor) { 
    this->firmware_version_text_sensor_ = version_sensor; 
  }
  
  // 设置操作模式文本传感器
  void set_operating_mode_text_sensor(text_sensor::TextSensor *mode_sensor) {
    this->operating_mode_text_sensor_ = mode_sensor;
  }
  
  // 设置校准进度传感器
  void set_calibration_progress_sensor(sensor::Sensor *calibration_progress) { calibration_progress_sensor_ = calibration_progress; }
  
  // 设置能量门传感器
  void set_energy_gate_sensor(uint8_t gate_index, sensor::Sensor *energy_sensor) {
    if (gate_index < MAX_GATES) {  // 使用常量保持一致性
      if (energy_gate_sensors_.size() <= gate_index) {
        energy_gate_sensors_.resize(gate_index + 1, nullptr);
      }
      energy_gate_sensors_[gate_index] = energy_sensor;
      engineering_data_enabled_ = true; // 启用工程数据处理
    }
  }
  
  // 添加静止能量门传感器设置方法
  void set_still_energy_gate_sensor(uint8_t gate_index, sensor::Sensor *still_energy_sensor) {
    if (gate_index < MAX_GATES) {
      if (still_energy_gate_sensors_.size() <= gate_index) {
        still_energy_gate_sensors_.resize(gate_index + 1, nullptr);
      }
      still_energy_gate_sensors_[gate_index] = still_energy_sensor;
      engineering_data_enabled_ = true; // 启用工程数据处理
    }
  }
  
  // 添加阈值传感器设置器
  void set_motion_threshold_sensor(uint8_t gate_index, sensor::Sensor *threshold_sensor) {
    if (gate_index < MAX_GATES) {
      if (motion_threshold_sensors_.size() <= gate_index) {
        motion_threshold_sensors_.resize(gate_index + 1, nullptr);
      }
      motion_threshold_sensors_[gate_index] = threshold_sensor;
    }
  }
  
  // 设置微动阈值传感器
  void set_micromotion_threshold_sensor(uint8_t gate_index, sensor::Sensor *threshold_sensor) {
    if (gate_index < MAX_GATES) {
      if (micromotion_threshold_sensors_.size() <= gate_index) {
        micromotion_threshold_sensors_.resize(gate_index + 1, nullptr);
      }
      micromotion_threshold_sensors_[gate_index] = threshold_sensor;
    }
  }
  
  // 设置方法（override Component 类的方法）
  void setup() override;
  // 循环方法（override Component 类的方法）
  void loop() override;
  //  dump 配置方法（override Component 类的方法）
  void dump_config() override;
  
  // 校准方法
  void calibrate();
  // 保存配置方法
  void save_config();
  // 启用自动增益方法
  void enable_auto_gain();
  // 检查电源干扰方法
  void check_power_interference();
  // 工厂重置方法（新增）
  void factory_reset();
  // 带参数的工厂重置方法（新增）
  void factory_reset_with_params(float max_distance, int timeout);
  
  // 添加新的直接模式设置方法
  void set_engineering_mode_direct();
  void set_normal_mode_direct();
  
  // 保留现有方法以保持向后兼容
  void set_engineering_mode();
  void set_normal_mode();
  
  // 获取序列号方法
  void get_serial_number();

  // 添加新的阈值设置方法
  bool set_motion_threshold(uint8_t gate, float db_value);
  bool set_micromotion_threshold(uint8_t gate, float db_value);
  bool calibrate_with_coefficients(float trigger_coeff, float hold_coeff, float micromotion_coeff);

  // 为特定门设置运动阈值的服务
  void set_gate_motion_threshold(int gate, float db_value) {
    set_motion_threshold(gate, db_value);
  }
  
  // 为特定门设置微动阈值的服务
  void set_gate_micromotion_threshold(int gate, float db_value) {
    set_micromotion_threshold(gate, db_value);
  }

  // 添加批量参数操作的新方法声明
  bool get_all_motion_thresholds();
  bool get_all_micromotion_thresholds();
  
  // 读取阈值的服务
  void read_motion_thresholds() {
    get_all_motion_thresholds();
  }
  
  // 读取微动阈值的服务
  void read_micromotion_thresholds() {
    get_all_micromotion_thresholds();
  }

protected:
  // 进入配置模式
  bool enter_config_mode_();
  // 快速进入配置模式（新方法）
  bool enter_config_mode_quick_();
  // 退出配置模式
  bool exit_config_mode_();
  // 发送命令
  bool send_command_(uint16_t command, const uint8_t *data = nullptr, size_t len = 0);
  // 读取响应（添加了超时参数）
  bool read_response_(std::vector<uint8_t> &response, uint32_t timeout_ms = 1000);
  // 设置参数
  bool set_parameter_(uint16_t param_id, uint32_t value);
  // 获取参数
  bool get_parameter_(uint16_t param_id, uint32_t &value);
  // 设置工作模式
  bool set_work_mode_(uint32_t mode);
  // 带超时的工作模式设置（新方法）
  bool set_work_mode_with_timeout_(uint32_t mode, uint32_t timeout_ms);
  // 处理行数据
  void process_line_(const std::string &line);
  // 打印十六进制数据
  void dump_hex_(const uint8_t *data, size_t len, const char* prefix);
  // 写入帧（新方法）
  bool write_frame_(const std::vector<uint8_t> &frame);
  // 获取固件版本（添加缺失的函数声明）
  void get_firmware_version_();
  // 开始被动版本检测（新方法）
  void begin_passive_version_detection_();
  // 发布当前操作模式（新方法）
  void publish_operating_mode_();
  
  // 保存配置
  bool save_configuration_();
  // 启用自动增益
  bool enable_auto_gain_();
  // 获取十六进制格式的序列号
  bool get_serial_number_hex_();
  // 获取字符格式的序列号
  bool get_serial_number_char_();

  // 将 dB 值转换为原始阈值
  uint32_t db_to_threshold_(float db_value);
  // 将原始阈值转换为 dB 值
  float threshold_to_db_(uint32_t threshold);

  // 解析数据帧
  bool parse_data_frame_(const std::vector<uint8_t> &frame_data);
  // 从距离帧处理工程数据（新方法）
  bool process_engineering_from_distance_frame_(const std::vector<uint8_t> &frame_data);
  // 更新二进制传感器（新辅助方法）
  void update_binary_sensors_(float distance_cm);

  // 批量参数读取方法
  bool get_parameters_batch_(const std::vector<uint16_t> &param_ids, std::vector<uint32_t> &values);

// 在private部分添加帧重组相关变量
private:
  // 根据手册，响应超时应为1秒
  static const uint32_t RESPONSE_TIMEOUT_MS = 1000;
  
  // 传感器指针
  sensor::Sensor *distance_sensor_{nullptr};
  sensor::Sensor *calibration_progress_sensor_{nullptr};
  binary_sensor::BinarySensor *presence_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *micromovement_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *power_interference_binary_sensor_{nullptr};
  
  // 文本传感器指针
  text_sensor::TextSensor *firmware_version_text_sensor_{nullptr};
  text_sensor::TextSensor *operating_mode_text_sensor_{nullptr};
  
  // 配置参数
  float max_distance_{5.0};  // 最大检测距离，默认5.0米
  uint32_t timeout_{5};  // 超时时间，默认5秒
  bool config_mode_{false};  // 配置模式标志
  std::string firmware_version_;  // 固件版本
  std::string line_buffer_;  // 行缓冲区
  bool power_interference_detected_{false};  // 电源干扰检测标志
  uint32_t last_calibration_status_{0};  // 上次校准状态
  bool calibration_in_progress_{false};  // 校准进行中标志
  uint32_t last_calibration_check_{0};   // 上次校准检查时间
  uint32_t calibration_progress_{0};     // 当前校准进度（0-100）
  std::string serial_number_; // 存储序列号的字段
  std::string operating_mode_{"Normal"};  // 跟踪当前操作模式
  uint32_t last_distance_update_{0};   // 上次距离传感器更新时间
  uint32_t distance_throttle_ms_{2000}; // 默认节流时间为2秒
  float last_published_distance_{-1.0};  // 上次发布的距离值
  float distance_change_threshold_{0.0};  // 距离变化阈值，默认0厘米
  uint32_t last_engineering_update_{0}; // 上次工程数据更新时间
  uint32_t engineering_throttle_ms_{2000}; // 工程数据节流时间（2秒）
  std::vector<sensor::Sensor *> energy_gate_sensors_; // 存储门传感器
  bool engineering_data_enabled_{false}; // 启用工程数据处理的标志
  
  // 添加静止能量门传感器存储
  std::vector<sensor::Sensor *> still_energy_gate_sensors_; // 存储静止能量门传感器
  
  // 添加阈值传感器存储
  std::vector<sensor::Sensor *> motion_threshold_sensors_;
  std::vector<sensor::Sensor *> micromotion_threshold_sensors_;
  
  // 添加阈值值的缓存
  std::vector<float> motion_threshold_values_;
  std::vector<float> micromotion_threshold_values_;
  
  // 工程模式帧超时常量
  static const uint32_t FRAME_TIMEOUT_MS = 200; // 调整为200ms
};

}  // namespace hlk_ld2402
}  // namespace esphome