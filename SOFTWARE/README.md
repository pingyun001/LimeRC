# 文件夹说明
- Lime_SimAgreement：仿真协议，用于测试仿真器与仿真模型的通信协议
- LimeCam_Release：图传接收机源代码
- LimeScrBoard_Release：屏幕按键子板源代码
- LimeMainBoard_Release：遥控器主板源代码
- Z-releaseHex：已编译好的Hex固件
# 编译说明
## Lime_SimAgreement仿真（非必须）
推荐使用VSCode，配置CMake环境+GCC编译
## STM32工程（必须）
- LimeCam_Release
- LimeScrBoard_Release
- LimeMainBoard_Release

推荐使用keil 5MDK编译 需安装如下芯片的keil芯片包
- STM32F0
- STM32F4
- STM32H7

相关芯片包可从keil官网下载 若找不到芯片包，可联系作者获取

推荐keil版本V5.38及以上

其中遥控器主板源代码中额外需要安装Keil RTE环境 LVGL 9.3 Release库