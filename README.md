# 基于ESP32的多协议转WiFi模块

> 本项目为基于乐鑫公司的 ESP32-pico-d4 芯片制作的无线模块，具有多个通信协议接口：UART、SPI、SDIO。设计初衷是为了方便智能车比赛的摄像头算法调试，通过和上位机配合降低调试难度

由于芯片名称里有一个 pico 单词而且本项目和图像有关，所以将该模块取名为 Pic-o Link

该模块的一大特点是可以兼容逐飞的无线串口接口，具有硬件流控功能，并且可以直接使用逐飞的无线串口驱动实现单向通信，免去了车友们测试该模块需要重新制板的需要

> esp32 硬件支持 5Mbps UART 和 10Mbps SPI slave

**目前仅支持单向发送到上位机**

开源上位机可以参考另一位大佬的 https://gitee.com/zhou-wenqi/ipc-for-car.git

本项目还处于雏形，代码硬件还比较粗糙，欢迎各位大佬来交流和提出改进意见，本人QQ:1626632460

**Pic-o Link 成品[兼容逐飞无线串口接口(去除SDIO)，集成固件]**及**致用上位机（可配置 Pic-o Link 参数）**已上架淘宝：https://m.tb.cn/h.fLp50d0?tk=2Hlu26q4nDN

**PCB 仿真如下**

|     类别     |                    原版（UART+SPI+SDIO）                     |               兼容逐飞无线串口接口（去除SDIO）               |
| :----------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|   3D仿真图   | <img src="image/image-20220313214826327.png" alt="image-20220313214826327" style="zoom:80%;" /> | <img src="image/image-20220313214931448.png" alt="image-20220313214931448" style="zoom:80%;" /> |
| 立创开源链接 | https://oshwhub.com/Wander_er/q42iJHazCk9v8lMXZTbMWPPNwijA22Vf | https://oshwhub.com/Wander_er/891fe1d235694ef7afe684f5a2f05b73 |

**焊接效果还可以**

| ![](image/IMG_20220329_100706.jpg) | ![](image/IMG_20220329_100642.jpg) |
| :--------------------------------: | :--------------------------------: |

**目录结构**

- `doc` 文档
- `firmware` Pic-o Link 固件及单片机使用例程
  - `Pic-o_Link` Pic-o Link 固件
  - `RT1064 `RT1064使用例程
- `hardware` Pic-o Link 两个版本的硬件文件
- `software` 上位机软件
  - `monitor_python` python 上位机脚本
- `image`图片

## Pic-o Link

### 硬件（立创EDA-专业版）

- 基于 esp32-pico-d4
- spi 接口
- uart 接口
- sdio 接口
- 一个复位按键
- 一个下载按键
- 电源指示灯
- rgb 三色灯
- 5V 电源输入，3.3V 电源稳压

**接口及PCB绘制要求**

- 接口：2.54mm 2x4或5p 排母
- 5V 电源至少要保证 500mA 的输出电流

### 固件（基于 espressif 框架）

==**三种通信模式固件**==

- **UART** 最大5Mbps

  使用串口轮询从缓冲区中提取接收数据，有两个参数，分别是接收缓冲区大小和最长等待时间，当接收的字节数达到缓冲区大小时立即视为完成一次接收；发送完毕但没有达到接收缓冲区大小时，则等待最长等待时间再视作完成一次传输，这个等待时间是固定的20ms

  所以可以视作具有两种模式，通过在固件中修改 `uart_read_bytes`() 函数的 `length` 参数切换

  - **透传模式**：指传输数据没有固定的字节数限制，每次通信可以传输各种大小的数据，更加灵活

    使用透传模式建议发送间隔大于 `串口传输时间` + 20ms

    `length` 参数为缓冲区大小 `RX_BUF_SIZE-1` 时为透传模式，即假设没有数据会达到这个长度

  - **固定字节模式**：指传输数据有固定的字节数限制，速度比透传模式更快，但只能传输固定大小的数据

    使用固定字节模式建议发送间隔大于 `串口传输时间`，并且 **Pic-o Link RTS 引脚要与发送端 CTS 引脚相连实现硬件流控（或者直接参考逐飞无线串口接口的原理图）**

    **UART 固件使用的 RTS 引脚为 MOSI 引脚**

    `length` 参数为要发送的固定数据的字节数时为固定字节，例如传输60x90灰度图时为5400

    > 当然，发送低于这个长度的数据时也可视作为透传模式

- **SPI** 最大10Mbps

  注意：只支持4字节倍数长度的数据接收

- **SDIO**（待开发）

==**一个集成固件（Pic-o_Link）**==

- 集成了 UART 和 SPI 两种通信模式
- 具有配置模式，可以通过串口通信协议配置参数写进Flash里，掉电不丢失，免去了反复修改固件的需要

**集成固件如何进入和使用配置模式？**

- 电脑使用 USB 转 TTL 串口助手连接 Pic-o Link，短接 MOSI 和 CS 引脚再复位，指示灯显示黄色代表进入配置模式，此时可以通过串口配置 Pic-o Link 参数，协议见表格

  |              类别               |  帧头   |  长度  |
  | :-----------------------------: | :-----: | :----: |
  |          通信协议选择           | 0x41(A) | 1字节  |
  |      UART 速率（<5000000）      | 0x42(B) | 4字节  |
  | UART 接收缓冲字节大小（<20000） | 0x43(C) | 2字节  |
  |            WiFi 账号            | 0x44(D) | 32字节 |
  |            WiFi 密码            | 0x45(E) | 64字节 |
  |           UDP ip地址            | 0x46(F) | 15字节 |
  |            UDP 端口             | 0x47(G) | 2字节  |
  |          读取（指令）           | 0x48(H) |   无   |
  |      写入所有参数（指令）       | 0x49(I) |   无   |

**其他固件可以自定义的参数**

- Pic-o Link 连接的 WiFi 账号及密码
- 上位机 udp server 的 IP 地址以及端口（**上位机需与 Pic-o Link 处于同一网段**）

**如何打开固件工程？**

- 使用 VS code 的 Plaform IO 插件

**如何进入下载模式？**

- 按住 DOWNLOAD 键不放再按一下 RESET 键，rgb 灯不亮即进入了下载模式，可以点击 Platform IO 的下载按键一键编译下载，下载完毕后一定要记得按 RESET 键

**如何修改模块的主机名？**

- 修改 `sdkconfig.pico32` 文件的 `CONFIG_LWIP_LOCAL_HOSTNAME` 项再重新编译烧录固件

**模块支持的WiFI频率？**

- 仅支持2.4GHz

### 工作流程

1. 上电
2. nvs flash 初始化
3. 设置为 STA 模式
4. WiFI 扫描模式，指示灯显示当前状态 -> 红
5. WiFi 连接成功，指示灯显示当前状态 -> 绿
6. 设置为 udp client 模式，指示灯显示当前状态 -> 蓝
7. 进入和下位机的通信模式
9. 传输数据时，指示灯显示当前状态 -> 白

## 使用情形

以下两种情形指示了 Pic-o Link 配置的 IP 地址参数：

![使用情形](image/使用情形.png)

## 软件

简易的 python 脚本

- 串口发送测试
- 接收图像显示

## 测试1

**固件**：**UART** 1.5Mbps 透传模式

> 由于电脑串口助手没有引出硬件流控引脚，所以在串口在发送大量数据时可能会丢失，固定字节模式固定的字节数可能达不到引发错误，所以电脑测试统一使用透传模式

使用电脑串口通信，电脑运行脚本 `.\software\monitor_python\serial_sender.py` 通过串口向 `Pic-o Link` 发送视频帧数据（`.\software\monitor_python\example1.mp4`）。为 90 x 60 灰度图，大小5400字节

电脑运行 `.\software\monitor_python\monitor_image.py` 脚本接收图像。

视频见文件 `.\image\测试1.mp4`，接收还原的视频平均帧数16.2帧，，实际串口发送延迟35ms

总共发送4555帧，出现14帧错误。错误率：0.3%

![测试1](image/测试1.png)

## 测试2

**固件**：**UART** 1.5Mbps 透传模式

使用电脑串口通信，电脑运行脚本 `.\software\monitor_python\serial_sender_color.py` 通过串口向 `Pic-o Link` 发送彩色视频帧数据（`.\software\monitor_python\example2.mp4`）。为 45 x 30 彩色图，大小不固定，平均3500字节。

电脑运行 `.\software\monitor_python\monitor_image_color.py` 脚本接收彩色图像。

视频见文件 `.\image\测试2.mp4`，接收还原的视频平均帧数20帧，实际串口发送延迟23ms

![测试2](image/测试2.png)

## 测试3

**固件**：**UART** 5Mbps 固定字节模式

使用 RT1064 通信，波特率 5Mbps，RT1064 固件为 `.\firmware\rt1064\UART` 工程。

电脑运行 `.\software\monitor_python\monitor_image.py` 脚本接收图像。

使用摄像头为总钻风摄像头，图像分辨率 90 x 60，摄像头帧数50帧。

RT1064 在主函数中刷新图像后发送采集图像。实际测试上位机平均每20ms收到图像，帧数50帧左右。由于使用硬件流控没有出现图像数据缺失，视频见文件 `.\image\测试3.mp4`，由于是电脑大屏显示，所以 90x60 的图像显得有些模糊。芯片测试得发送一张灰度图用时13ms

![测试3](image/测试3.png)

## 测试4

**固件**：**SPI**

使用 RT1064 通信，SPI 频率 10Mbps，RT1064 固件为 `.\firmware\rt1064\SPI` 工程。

电脑运行 `.\software\monitor_python\monitor_image.py` 脚本接收图像。

使用摄像头为总钻风摄像头，图像分辨率 90 x 60，摄像头帧数50帧。

RT1064 在主函数中刷新图像后发送采集图像。实际测试上位机平均每21ms收到图像，帧数50帧左右。芯片测试得发送一张灰度图用时4829us,视频见文件 `.\image\测试4.mp4`

![测试4](image/测试4.png)

## 缺陷

- 发热比较严重
- UART 固件透传模式有一个固定的 20ms 延迟

## 后续计划

1. 编写 SDIO 固件
2. 实现全双工模式

## 其他资料

乐鑫官方测试的 ESP32 的 udp/tcp 速率：

| Type/Throughput      | Air In Lab | Shield-box     | Test Tool     | IDF Version (commit ID) |
| -------------------- | ---------- | -------------- | ------------- | ----------------------- |
| Raw 802.11 Packet RX | N/A        | **130 MBit/s** | Internal tool | NA                      |
| Raw 802.11 Packet TX | N/A        | **130 MBit/s** | Internal tool | NA                      |
| UDP RX               | 30 MBit/s  | 85 MBit/s      | iperf example | 15575346                |
| UDP TX               | 30 MBit/s  | 75 MBit/s      | iperf example | 15575346                |
| TCP RX               | 20 MBit/s  | 65 MBit/s      | iperf example | 15575346                |
| TCP TX               | 20 MBit/s  | 75 MBit/s      | iperf example | 1557534                 |
