# Minecraft Server Launcher

一个功能完整的 Minecraft 服务器管理工具，支持服务器安装、启动、配置及模组管理。

GitHub: https://github.com/skc-cmd/Minecraft-Server-Launcher  
License: GPL v3

## 简介

Minecraft Server Launcher 是一款专为 Windows 平台设计的轻量级 Minecraft 服务器管理程序。它简化了官方服务器端的下载、配置和启动流程，并提供了对 Fabric 模组加载器的支持，让您能轻松搭建和管理模组服务器。

## 功能特性

- 安装服务器：自动从 Mojang 官方源获取并下载指定版本的服务器端 (server.jar)。
- 启动服务器：根据您配置的内存、JVM 参数和优化选项，一键启动服务器。
- 删除服务器：快速移除不再需要的服务器实例及其所有文件。
- 配置 server.properties：提供交互式界面，方便地修改服务器的核心配置文件。
- 自动配置：预设通用配置项，在安装新服务器时自动应用，提高效率。
- 启动选项配置：为每个服务器单独设置最小/最大内存、启用 Aikar 优化参数、nogui 模式等。
- 模组加载器支持：
    - 安装 Fabric：自动下载并运行 Fabric 安装器，为服务器安装模组加载环境。
    - 安装模组：通过文件选择对话框，便捷地将 .jar 模组文件复制到服务器的 mods 文件夹。
- 插件系统：支持通过 plugin 文件夹扩展程序功能（插件需自行开发）。
- 版本列表：查看可用的 Minecraft 正式版本。
- EULA 自动确认：首次启动服务器时，会提示您同意 Minecraft EULA，同意后自动生成 eula.txt 文件。

## 环境要求

- 操作系统：Windows 7 或更高版本。
- Java 环境：需要安装 Java 运行时环境 (JRE) 或 Java 开发工具包 (JDK) 17 或更高版本。
    - 推荐下载：Java 21 (LTS) 或 Java 26。

## 下载与运行

1. 从 Releases 页面下载最新版本的 MCServerManager.exe。
2. 将下载的 .exe 文件放在您希望作为服务器根目录的文件夹中（例如 D:\MCServer）。
3. 双击运行 MCServerManager.exe。

## 首次使用流程

1. 同意免责声明：程序启动后会显示免责声明，输入 y 继续。
2. 检测 Java：程序会自动检测系统中的 Java 版本。如果未找到，请根据提示安装 Java。
3. 开始使用：您将看到主菜单，可以开始安装或管理您的服务器了。

## 使用指南

### 主菜单

程序启动后，您会看到如下主菜单：

```
========================================
  MC服务器管理程序
========================================
  1. 安装MC服务器
  2. 启动MC服务器
  3. 删除MC服务器
  4. 配置版本的配置文件
  5. 安装服务器自动配置
  6. 启动选项配置
  7. 安装模组加载器
  8. 安装模组
  9. 列出已安装的服务器
  10. 查看可用版本
  11. 插件
  12. 关于
  0. 退出
========================================
请选择操作:
```

### 核心功能详解

#### 1. 安装服务器
选择 1，输入想安装的版本号（如 1.20.1）。程序会自动下载对应版本的官方服务器端文件，并创建 server/<版本号>/ 目录。

#### 2. 启动服务器
选择 2，从列表中选择一个已安装的版本。程序会：
- 检查并引导您同意 EULA。
- 根据配置生成 start.bat 启动脚本。
- 在新命令行窗口中启动服务器。

#### 4. 配置 server.properties
选择 4，选择要配置的服务器版本。您可以：
- 输入配置键（如 online-mode）和值（如 false）。
- 程序会提供每个常用配置项的说明。
- 输入 quit 退出配置界面。

#### 6. 启动选项配置
选择 6，为特定服务器设置：
- 最小内存 (-Xms) 和最大内存 (-Xmx)。
- 是否启用 Aikar 优化参数。
- 额外的 JVM 参数。
- 是否启用 nogui 模式。

#### 7 & 8. Fabric 模组管理
- 安装 Fabric：选择 7，程序会自动下载 Fabric 安装器并运行，为您的服务器安装模组加载环境。
- 安装模组：选择 8，会弹出文件选择窗口，您可以选择本地的 .jar 模组文件，程序会将其复制到对应服务器的 mods 目录中。

## 许可证

本项目使用 GNU General Public License v3.0 开源许可证。详情请见 LICENSE 文件。

## 第三方依赖

- nlohmann/json：用于 JSON 解析。
- Fabric 安装器：用于安装 Fabric 模组加载器。

## 开发辅助

本项目的部分代码在开发过程中使用了 DeepSeek 辅助编写。

## 联系方式

- 作者：skc
- GitHub：https://github.com/skc-cmd
- CSDN：https://blog.csdn.net/weixin_42156143
- 项目地址：https://github.com/skc-cmd/Minecraft-Server-Launcher

如有问题或建议，欢迎在 GitHub 仓库提交 Issue 或 Pull Request。

# Minecraft Server Launcher

A fully-featured Minecraft server management tool that supports server installation, startup, configuration, and mod management.

GitHub: https://github.com/skc-cmd/Minecraft-Server-Launcher
License: GPL v3

## Introduction

Minecraft Server Launcher is a lightweight Minecraft server management program designed for the Windows platform. It simplifies the process of downloading, configuring, and starting official server jars, and provides support for the Fabric mod loader, allowing you to easily set up and manage modded servers.

## Features

- Install Server: Automatically fetch and download the server jar for a specified version from the official Mojang source.
- Start Server: Start the server with one click based on your configured memory, JVM arguments, and optimization options.
- Delete Server: Quickly remove an unwanted server instance and all its files.
- Configure server.properties: Modify the server's core configuration file through an interactive interface.
- Auto Configuration: Preset common configuration items that are automatically applied when installing new servers, improving efficiency.
- Startup Options Configuration: Set minimum/maximum memory, enable Aikar optimization flags, nogui mode, and more for each server individually.
- Mod Loader Support:
    - Install Fabric: Automatically download and run the Fabric installer to set up the mod loading environment for your server.
    - Install Mods: Use a file selection dialog to conveniently copy .jar mod files into the server's mods folder.
- Plugin System: Extend program functionality through the plugin folder (plugins need to be developed separately).
- Version List: View available official Minecraft releases.
- EULA Auto-Confirmation: When starting a server for the first time, you will be prompted to accept the Minecraft EULA. Upon agreement, the eula.txt file is automatically generated.

## System Requirements

- Operating System: Windows 7 or higher.
- Java Environment: Java Runtime Environment (JRE) or Java Development Kit (JDK) 17 or higher is required.
    - Recommended Downloads: Java 21 (LTS) or Java 26.

## Download and Run

1. Download the latest version of MCServerManager.exe from the Releases page.
2. Place the downloaded .exe file in the folder you want to use as the server root directory (e.g., D:\MCServer).
3. Double-click MCServerManager.exe to run it.

## First-Time Setup

1. Accept the Disclaimer: The program will display a disclaimer. Enter y to continue.
2. Java Detection: The program will automatically detect the Java version on your system. If not found, please install Java following the prompts.
3. Start Using: You will see the main menu and can begin installing or managing your servers.

## User Guide

### Main Menu

After starting the program, you will see the following main menu:

```
========================================
  MC Server Manager
========================================
  1. Install MC Server
  2. Start MC Server
  3. Delete MC Server
  4. Configure server.properties
  5. Configure Auto-Install Settings
  6. Configure Startup Options
  7. Install Mod Loader
  8. Install Mod
  9. List Installed Servers
  10. View Available Versions
  11. Plugins
  12. About
  0. Exit
========================================
Please select an option:
```

### Core Features

#### 1. Install Server
Select 1 and enter the version number you want to install (e.g., 1.20.1). The program will automatically download the official server jar and create the server/<version>/ directory.

#### 2. Start Server
Select 2 and choose an installed version from the list. The program will:
- Check and guide you to accept the EULA.
- Generate a start.bat script based on your configuration.
- Start the server in a new command prompt window.

#### 4. Configure server.properties
Select 4 and choose the server version you want to configure. You can:
- Enter configuration keys (e.g., online-mode) and values (e.g., false).
- The program provides hints for commonly used configuration items.
- Enter quit to exit the configuration interface.

#### 6. Configure Startup Options
Select 6 to set the following for a specific server:
- Minimum memory (-Xms) and maximum memory (-Xmx).
- Whether to enable Aikar optimization flags.
- Additional JVM arguments.
- Whether to enable nogui mode.

#### 7 & 8. Fabric Mod Management
- Install Fabric: Select 7. The program will automatically download the Fabric installer and run it to set up the mod loading environment for your server.
- Install Mods: Select 8. A file selection dialog will appear, allowing you to choose a local .jar mod file. The program will copy it to the corresponding server's mods directory.

## License

This project is licensed under the GNU General Public License v3.0. See the LICENSE file for details.

## Third-Party Dependencies

- nlohmann/json: Used for JSON parsing.
- Fabric Installer: Used for installing the Fabric mod loader.

## Development Assistance

Portions of this project's code were developed with the assistance of DeepSeek.

## Contact

- Author: skc
- GitHub: https://github.com/skc-cmd
- CSDN: https://blog.csdn.net/weixin_42156143
- Project Repository: https://github.com/skc-cmd/Minecraft-Server-Launcher

If you encounter any issues or have suggestions, please submit an Issue or Pull Request on the GitHub repository.
