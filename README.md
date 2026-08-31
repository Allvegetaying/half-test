| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |

# 传感器数据对比测试 (Sensor Data Comparison Test)

基于 ESP32 的传感器数据对比测试项目，用于验证 UART1 串口传感器数据与 433MHz RF 无线数据的一致性。

## 功能概述

- **双串口通信**：UART0 用于上位机通信，UART1 用于传感器数据接收
- **433MHz RF 接收**：使用 A7169 模块接收无线数据
- **数据对比**：自动对比 UART1 和 RF 接收的数据，并输出 PASS/FAIL 结果
- **状态控制**：通过 GPIO8 检测引脚控制激活/待机状态
- **按键重置**：支持通过 GPIO3/GPIO4 按键重置对比状态

## 硬件连接

| 功能 | 引脚 | 说明 |
|------|------|------|
| 唤醒输出 | GPIO2 | 控制外部设备唤醒 |
| 行程检测输入 | GPIO8 | 高电平激活，低电平待机 |
| 开关输入1 | GPIO3 | 按键重置（低电平有效） |
| 开关输入2 | GPIO4 | 按键重置（低电平有效） |
| UART0 TX | GPIO43 | 上位机通信 |
| UART0 RX | GPIO44 | 上位机通信 |
| UART1 TX | GPIO17 | 传感器通信 |
| UART1 RX | GPIO18 | 传感器通信 |

## 通信参数

- **UART0**：波特率 115200，8N1
- **UART1**：波特率 115200，8N1

## 数据格式

传感器数据帧格式（至少5字节）：

| 字节 | 含义 |
|------|------|
| Byte 0 | 设备 ID |
| Byte 1 | 压力值 |
| Byte 2 | 温度值 |
| Byte 3 | 加速度值 |
| Byte 4 | 电池电压 |

## 工作流程

1. 系统启动后，初始化 GPIO、UART0、UART1 和 433 RF 模块
2. GPIO8 检测到高电平时进入激活状态，使能 UART1 和 RF 数据接收
3. UART1 和 RF 同时接收数据，数据就绪后自动进行对比
4. 对比结果通过 UART0 输出：
   - `PASS: 数据一致` - 两种方式接收的数据完全相同
   - `FAIL: 数据不一致` - 数据存在差异，并打印详细对比信息
5. 按下 GPIO3 或 GPIO4 按键可重置对比状态

## 构建与烧录

```bash
# 配置项目
idf.py menuconfig

# 编译
idf.py build

# 烧录
idf.py -p /dev/ttyUSB0 flash

# 查看串口输出
idf.py -p /dev/ttyUSB0 monitor
```

## 项目结构

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── test.c                  # 主程序
├── components
│   └── A7169                   # 433 RF 模块驱动
└── README.md
```

## Troubleshooting

* 程序上传失败

    * 检查硬件连接是否正确：运行 `idf.py -p PORT monitor`，重启开发板查看是否有输出日志
    * 下载波特率过高：在 `menuconfig` 菜单中降低波特率后重试

## 技术支持

如有技术问题，请访问 [esp32.com](https://esp32.com/) 论坛。
