step1: 在系统中安装rust功能

```shell
sudo apt update 
sudo apt 
```

step2: 在系统中安装rust build功能

```shell
# 安装rust支撑交叉编译
rustup target add thumbv7m-none-eabi

# 安装build工具
cargo install cargo-binstall

# 安装untils环境
cargo install cargo-binutils

# 安装probe-rs
cargo binstall probe-rs-tools
```

step3: 添加udev规则

```shell
sudo vim /etc/udev/rules.d/probe-rs.rules

# /etc/udev/rules.d/probe-rs.rules
################################################################################
# ST-LINK
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="3748", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="374b", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="374a", MODE="0666"

# CMSIS-DAP
SUBSYSTEM=="usb", ATTR{idVendor}=="c251", ATTR{idProduct}=="f001", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}==="0d28", ATTR{idProduct}=="0204", MODE="0666"

# J-Link
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0101", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0102", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0103", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0104", MODE="0666"
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", ATTR{idProduct}=="0105", MODE="0666"

# FTDI
SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6010", MODE="0666"
################################################################################

sudo udevadm control --reload-rules
sudo udevadm trigger
``