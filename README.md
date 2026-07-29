# Minecraft Server Launcher

一个功能完整的 Minecraft 服务器管理工具，支持服务器安装、启动、配置、模组管理、插件扩展及用户反馈。

GitHub: https://github.com/skc-cmd/Minecraft-Server-Launcher  
License: GPL v3

## 简介

Minecraft Server Launcher 是一款专为 Windows 平台设计的轻量级 Minecraft 服务器管理程序。它简化了官方服务器端的下载、配置和启动流程，提供了对 **Fabric** 模组加载器的支持，并创新性地引入了**插件系统**和**用户反馈机制**，让您能更轻松地搭建、管理模组服务器，并参与项目改进。

## 功能特性

- **安装服务器**：自动从 Mojang 官方源获取并下载指定版本的服务器端 (`server.jar`)。
- **启动服务器**：根据您配置的内存、JVM 参数和优化选项，一键启动服务器，并自动处理 `eula.txt`。
- **删除服务器**：快速移除不再需要的服务器实例及其所有文件。
- **配置 `server.properties`**：提供交互式界面，方便地修改服务器的核心配置文件。
- **自动配置**：预设通用配置项，在安装新服务器时自动应用，提高效率。
- **启动选项配置**：为每个服务器单独设置最小/最大内存、启用 Aikar 优化参数、`nogui` 模式等。
- **模组加载器支持**：
    - **安装 Fabric**：自动下载并运行 Fabric 安装器，为服务器安装模组加载环境。
    - **安装模组**：通过文件选择对话框，便捷地将 `.jar` 模组文件复制到服务器的 `mods` 文件夹。
- **插件系统**：支持通过 `plugin` 文件夹加载外部插件，以扩展程序功能（插件需自行开发，详见 [PLUGIN.md](PLUGIN.md)）。
- **用户反馈系统**：内置反馈功能，您可以将意见和建议直接发送给开发者（需配置 `feedback.txt`）。
- **版本列表**：查看可用的 Minecraft 正式版本。
- **EULA 自动确认**：首次启动服务器时，会提示您同意 Minecraft EULA，同意后自动生成 `eula.txt` 文件。

## 环境要求

- **操作系统**：Windows 7 或更高版本。
- **Java 环境**：需要安装 Java 运行时环境 (JRE) 或 Java 开发工具包 (JDK) **17 或更高版本**。
    - 推荐下载：Java 21 (LTS) 或 Java 26。

## 下载与运行

1.  从 [Releases](https://github.com/skc-cmd/Minecraft-Server-Launcher/releases) 页面下载最新版本的 `MCServerManager.exe`。
2.  将下载的 `.exe` 文件放在您希望作为服务器根目录的文件夹中（例如 `D:\MCServer`）。
3.  双击运行 `MCServerManager.exe`。

## 首次使用流程

1.  **同意免责声明**：程序启动后会显示免责声明，输入 `y` 继续。
2.  **检测 Java**：程序会自动检测系统中的 Java 版本。如果未找到，请根据提示安装 Java。
3.  **开始使用**：您将看到主菜单，可以开始安装或管理您的服务器了。

## 使用指南

### 主菜单

程序启动后，您会看到如下主菜单（v1.6 版本）：

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
  13. 发送反馈
  0. 退出
========================================
请选择操作:
```

### 核心功能详解

#### 1. 安装服务器
选择 `1`，输入想安装的版本号（如 `1.20.1`）。程序会自动下载对应版本的官方服务器端文件，并创建 `server/<版本号>/` 目录。

#### 2. 启动服务器
选择 `2`，从列表中选择一个已安装的版本。程序会：
- 检查并引导您同意 EULA。
- 根据配置生成 `start.bat` 启动脚本。
- 在新命令行窗口中启动服务器。

#### 3. 删除服务器  
选择 `3`，选择要删除的服务器版本。程序会删除对应的服务器版本。

#### 4. 配置服务器  
选择 `4`，选择要配置的服务器版本。您可以：
- 输入配置键（如 `online-mode`）和值（如 `false`）。
- 程序会提供每个常用配置项的说明。
- 输入 `quit` 退出配置界面。

#### 5. 安装服务器自动配置  
选择 `5`，设置自动配置，每当安装服务器，将会自动设置好这些配置。

#### 6. 启动选项配置
选择 `6`，为特定服务器设置：
- 最小内存 (`-Xms`) 和最大内存 (`-Xmx`)。
- 是否启用 Aikar 优化参数。
- 额外的 JVM 参数。
- 是否启用 `nogui` 模式。

#### 7 & 8. Fabric 模组管理
- **安装 Fabric**：选择 `7`，程序会自动下载 Fabric 安装器并运行，为您的服务器安装模组加载环境。
- **安装模组**：选择 `8`，会弹出文件选择窗口，您可以选择本地的 `.jar` 模组文件，程序会将其复制到对应服务器的 `mods` 目录中。

#### 9. 列出安装的服务器  
选择 `9`，程序会列出用户安装的服务器。

#### 10. 查看可用版本  
选择 `10`，程序会输出现在的最新版本。

#### 11. 插件
选择 `11`，程序会扫描 `plugin` 文件夹下的插件并列出。选择插件后，程序会以 `-<服务器版本>` 为参数调用对应的可执行文件。插件开发规范请参考 [PLUGIN.md](PLUGIN.md)。

#### 12. 关于
选择 `12`，查看关于程序作者的信息。

#### 13. 发送反馈
选择 `13`，程序会引导您输入多行反馈内容（连续输入两个空行结束）。确认发送后，反馈数据会通过 UDP 协议发送至您在 `feedback.txt` 中配置的服务器。反馈服务器会监听 **7767** 端口（ASCII: 77='M', 67='C'，代表 **MC**）。

**配置反馈服务器地址**：在程序根目录创建 `feedback.txt`，内容为：
```
server=<服务器IP地址>
```
例如：`server=127.0.0.1`

## 插件系统

程序支持通过 `plugin` 文件夹加载外部插件，以扩展功能。详细的开发规范、目录结构及模板代码，请参阅 [PLUGIN.md](PLUGIN.md)。

## 许可证

本项目使用 **GNU General Public License v3.0** 开源许可证。详情请见 [LICENSE](https://github.com/skc-cmd/Minecraft-Server-Launcher/blob/main/LICENSE) 文件。

## 第三方依赖

- [nlohmann/json](https://github.com/nlohmann/json)：用于 JSON 解析。
- Fabric 安装器：用于安装 Fabric 模组加载器。

## 开发辅助

本项目的部分代码在开发过程中使用了 **DeepSeek** 辅助编写。

## 联系方式

- **作者**：skc
- **GitHub**：[skc-cmd](https://github.com/skc-cmd)
- **CSDN**：[skc的博客](https://blog.csdn.net/weixin_42156143)
- **项目地址**：[https://github.com/skc-cmd/Minecraft-Server-Launcher](https://github.com/skc-cmd/Minecraft-Server-Launcher)

如有问题或建议，欢迎在 GitHub 仓库提交 [Issue](https://github.com/skc-cmd/Minecraft-Server-Launcher/issues) 或 Pull Request。

---

# Minecraft Server Launcher

A fully-featured Minecraft server management tool that supports server installation, startup, configuration, mod management, plugin extensions, and user feedback.

GitHub: https://github.com/skc-cmd/Minecraft-Server-Launcher
License: GPL v3

## Introduction

Minecraft Server Launcher is a lightweight Minecraft server management program designed for the Windows platform. It simplifies the process of downloading, configuring, and starting official server jars, provides support for the **Fabric** mod loader, and innovatively introduces a **plugin system** and a **user feedback mechanism**, allowing you to easily set up and manage modded servers and participate in project improvement.

## Features

- **Install Server**: Automatically fetch and download the server jar for a specified version from the official Mojang source.
- **Start Server**: Start the server with one click based on your configured memory, JVM arguments, and optimization options, with automatic `eula.txt` handling.
- **Delete Server**: Quickly remove an unwanted server instance and all its files.
- **Configure `server.properties`**: Modify the server's core configuration file through an interactive interface.
- **Auto Configuration**: Preset common configuration items that are automatically applied when installing new servers, improving efficiency.
- **Startup Options Configuration**: Set minimum/maximum memory, enable Aikar optimization flags, `nogui` mode, and more for each server individually.
- **Mod Loader Support**:
    - **Install Fabric**: Automatically download and run the Fabric installer to set up the mod loading environment for your server.
    - **Install Mods**: Use a file selection dialog to conveniently copy `.jar` mod files into the server's `mods` folder.
- **Plugin System**: Extend program functionality through the `plugin` folder (plugins need to be developed separately, see [PLUGIN.md](PLUGIN.md)).
- **User Feedback System**: Built-in feedback function to send suggestions directly to the developer (requires `feedback.txt` configuration).
- **Version List**: View available official Minecraft releases.
- **EULA Auto-Confirmation**: When starting a server for the first time, you will be prompted to accept the Minecraft EULA. Upon agreement, the `eula.txt` file is automatically generated.

## System Requirements

- **Operating System**: Windows 7 or higher.
- **Java Environment**: Java Runtime Environment (JRE) or Java Development Kit (JDK) **17 or higher** is required.
    - Recommended Downloads: Java 21 (LTS) or Java 26.

## Download and Run

1.  Download the latest version of `MCServerManager.exe` from the [Releases](https://github.com/skc-cmd/Minecraft-Server-Launcher/releases) page.
2.  Place the downloaded `.exe` file in the folder you want to use as the server root directory (e.g., `D:\MCServer`).
3.  Double-click `MCServerManager.exe` to run it.

## First-Time Setup

1.  **Accept the Disclaimer**: The program will display a disclaimer. Enter `y` to continue.
2.  **Java Detection**: The program will automatically detect the Java version on your system. If not found, please install Java following the prompts.
3.  **Start Using**: You will see the main menu and can begin installing or managing your servers.

## User Guide

### Main Menu

After starting the program, you will see the following main menu (v1.6):

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
  13. Send Feedback
  0. Exit
========================================
Please select an option:
```

### Core Features

#### 1. Install Server
Select `1` and enter the version number you want to install (e.g., `1.20.1`). The program will automatically download the official server jar and create the `server/<version>/` directory.

#### 2. Start Server
Select `2` and choose an installed version from the list. The program will:
- Check and guide you to accept the EULA.
- Generate a `start.bat` script based on your configuration.
- Start the server in a new command prompt window.

#### 4. Configure `server.properties`
Select `4` and choose the server version you want to configure. You can:
- Enter configuration keys (e.g., `online-mode`) and values (e.g., `false`).
- The program provides hints for commonly used configuration items.
- Enter `quit` to exit the configuration interface.

#### 6. Configure Startup Options
Select `6` to set the following for a specific server:
- Minimum memory (`-Xms`) and maximum memory (`-Xmx`).
- Whether to enable Aikar optimization flags.
- Additional JVM arguments.
- Whether to enable `nogui` mode.

#### 7 & 8. Fabric Mod Management
- **Install Fabric**: Select `7`. The program will automatically download the Fabric installer and run it to set up the mod loading environment for your server.
- **Install Mods**: Select `8`. A file selection dialog will appear, allowing you to choose a local `.jar` mod file. The program will copy it to the corresponding server's `mods` directory.

#### 11. Plugins
Select `11`, the program will scan and list plugins in the `plugin` folder. After selection, the plugin executable will be called with the `-<server-version>` argument. For plugin development specifications, please refer to [PLUGIN.md](PLUGIN.md).

#### 13. Send Feedback
Select `13`, the program will guide you to enter multi-line feedback (end with two consecutive blank lines). After confirmation, the feedback data will be sent via UDP to the server configured in `feedback.txt`. The feedback server listens on port **7767** (ASCII: 77='M', 67='C', representing **MC**).

**Configure Feedback Server Address**: Create `feedback.txt` in the program root directory with the following content:
```
server=<Server IP Address>
```
Example: `server=192.168.1.100`

## Plugin System

The program supports loading external plugins through the `plugin` folder to extend functionality. For detailed development specifications, directory structure, and template code, please refer to [PLUGIN.md](PLUGIN.md).

## License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](https://github.com/skc-cmd/Minecraft-Server-Launcher/blob/main/LICENSE) file for details.

## Third-Party Dependencies

- [nlohmann/json](https://github.com/nlohmann/json): Used for JSON parsing.
- Fabric Installer: Used for installing the Fabric mod loader.

## Development Assistance

Portions of this project's code were developed with the assistance of **DeepSeek**.

## Contact

- **Author**: skc
- **GitHub**: [skc-cmd](https://github.com/skc-cmd)
- **CSDN**: [skc's Blog](https://blog.csdn.net/weixin_42156143)
- **Project Repository**: [https://github.com/skc-cmd/Minecraft-Server-Launcher](https://github.com/skc-cmd/Minecraft-Server-Launcher)

If you encounter any issues or have suggestions, please submit an [Issue](https://github.com/skc-cmd/Minecraft-Server-Launcher/issues) or Pull Request on the GitHub repository.
