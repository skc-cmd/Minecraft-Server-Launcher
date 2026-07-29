# Plugin 开发指南

本文档说明如何为 Minecraft Server Launcher 开发插件。

## 概述

Minecraft Server Launcher 支持通过 `plugin` 文件夹加载外部插件，以扩展程序功能。程序启动时会扫描 `plugin` 目录下的子文件夹，读取每个插件的 `config.json` 配置文件，并在主菜单的 "插件" 选项中显示可用的插件列表。

## 目录结构

一个标准的插件目录结构如下：

    程序根目录/
    ├── MCServerManager.exe
    ├── server/
    ├── plugin/
    │   └── <插件名称>/
    │       ├── config.json
    │       └── <插件可执行文件>
    └── ...

### 命名规范

* 插件文件夹名称建议使用英文小写字母和连字符，如 `backup-tool`、`world-manager`
* 可执行文件名称建议与插件名称保持一致
  
  ## config.json 配置说明
  
  ### 完整示例
  
      {
        "name": "备份工具",
        "level": 1,
        "plugin": {
            "name": "backup-tool.exe"
        }
      }
  
  ### 字段说明
  
  | 字段  | 类型  | 必填  | 说明  |
  | --- | --- | --- | --- |
  | `name` | string | 是   | 插件在菜单中显示的名称 |
  | `level` | integer | 否   | 显示优先级，数字越小在菜单中排得越靠前，默认 999 |
  | `plugin.name` | string | 是   | 可执行文件的名称（包含 .exe 后缀） |
  
  ### 配置示例
  
      {
        "name": "世界管理",
        "level": 2,
        "plugin": {
            "name": "world-manager.exe"
        }
      }
  
  ## 插件开发规范
  
  ### 命令行参数
  
  当用户在程序主菜单中选择 "插件" 并选择一个插件后，程序会以以下方式启动插件：
  
      <插件可执行文件> -<服务器版本>
  
  例如，用户选择了版本 `1.20.1` 的服务器，程序会执行：
  
      backup-tool.exe -1.20.1
  
  ### 插件应实现的功能
  

1. **解析命令行参数**：获取用户传递的服务器版本号
  
2. **定位服务器目录**：根据版本号定位到 `server/<版本号>/` 目录
  
3. **执行核心功能**：在服务器目录上执行插件自身的功能
  
  ### C++ 插件模板
  
      #include <iostream>
      #include <string>
      #include <filesystem>
      namespace fs = std::filesystem;
      int main(int argc, char* argv[]) {
       // 检查命令行参数
       if (argc < 2) {
           std::cerr << "用法: " << argv[0] << " -<服务器版本>" << std::endl;
           return 1;
       }
       std::string arg = argv[1];
      
       // 解析版本号
       if (arg.empty() || arg[0] != '-') {
           std::cerr << "错误: 参数格式不正确，应为 -<服务器版本>" << std::endl;
           return 1;
       }
      
       std::string version = arg.substr(1);
      
       // 构建服务器目录路径
       std::string serverDir = "server/" + version;
      
       if (!fs::exists(serverDir) || !fs::is_directory(serverDir)) {
           std::cerr << "错误: 服务器目录不存在: " << serverDir << std::endl;
           return 1;
       }
      
       std::cout << "插件已启动" << std::endl;
       std::cout << "服务器版本: " << version << std::endl;
       std::cout << "服务器目录: " << serverDir << std::endl;
      
       // 在此处编写插件的核心逻辑
      
       std::cout << "插件执行完毕" << std::endl;
       return 0;
      }
  
  ### C# 插件模板
  
      using System;
      using System.IO;
      class Plugin
      {
       static void Main(string[] args)
       {
           if (args.Length < 1)
           {
               Console.WriteLine("用法: Plugin.exe -<服务器版本>");
               return;
           }
           string arg = args[0];
           if (string.IsNullOrEmpty(arg) || arg[0] != '-')
           {
               Console.WriteLine("错误: 参数格式不正确，应为 -<服务器版本>");
               return;
           }
           string version = arg.Substring(1);
           string serverDir = Path.Combine("server", version);
           if (!Directory.Exists(serverDir))
           {
               Console.WriteLine("错误: 服务器目录不存在: " + serverDir);
               return;
           }
           Console.WriteLine("插件已启动");
           Console.WriteLine("服务器版本: " + version);
           Console.WriteLine("服务器目录: " + serverDir);
           // 在此处编写插件的核心逻辑
           Console.WriteLine("插件执行完毕");
       }
      }
  
  ### Python 插件模板
  
      #!/usr/bin/env python3
      import sys
      import os
      def main():
       if len(sys.argv) < 2:
           print("用法: plugin.py -<服务器版本>")
           return
       arg = sys.argv[1]
       if not arg or arg[0] != '-':
           print("错误: 参数格式不正确，应为 -<服务器版本>")
           return
       version = arg[1:]
       server_dir = os.path.join("server", version)
       if not os.path.exists(server_dir) or not os.path.isdir(server_dir):
           print(f"错误: 服务器目录不存在: {server_dir}")
           return
       print("插件已启动")
       print(f"服务器版本: {version}")
       print(f"服务器目录: {server_dir}")
       # 在此处编写插件的核心逻辑
       print("插件执行完毕")
      if __name__ == "__main__":
       main()
  
  ## 插件安装
  
4. 在 `plugin` 目录下创建以插件名称命名的文件夹
  
5. 将 `config.json` 和插件可执行文件放入该文件夹
  
6. 重启 Minecraft Server Launcher 或回到主菜单重新扫描
  
  ## 注意事项
  
7. 插件可执行文件需要与操作系统兼容（Windows 平台）
  
8. 插件运行的当前工作目录为程序根目录
  
9. 插件应自行处理错误和异常，避免影响主程序
  
10. 插件不应修改或删除主程序的文件
  
11. 建议插件以独立窗口运行，不要在主程序控制台内输出过多内容
  
  ## 示例插件功能建议
  
  以下是一些插件功能建议，供开发者参考：
  

* 备份工具：自动备份服务器世界文件夹
* 世界管理：创建、删除、切换世界
* 玩家管理：查看在线玩家、踢出玩家、封禁玩家
* 性能监控：显示服务器 CPU、内存使用情况
* 定时任务：定时重启服务器、执行命令
* 日志分析：分析服务器日志，生成统计报告

* * *

# Plugin Development Guide

This document explains how to develop plugins for Minecraft Server Launcher.

## Overview

Minecraft Server Launcher supports loading external plugins through the `plugin` folder to extend program functionality. When the program starts, it scans subdirectories under the `plugin` directory, reads each plugin's `config.json` configuration file, and displays the available plugin list in the "Plugins" option of the main menu.

## Directory Structure

A standard plugin directory structure looks like this:

    Program Root/
    ├── MCServerManager.exe
    ├── server/
    ├── plugin/
    │   └── <plugin-name>/
    │       ├── config.json
    │       └── <plugin-executable>
    └── ...

### Naming Conventions

* Plugin folder names should use lowercase English letters and hyphens, e.g., `backup-tool`, `world-manager`
* Executable file names should match the plugin name
  
  ## config.json Configuration
  
  ### Full Example
  
      {
        "name": "Backup Tool",
        "level": 1,
        "plugin": {
            "name": "backup-tool.exe"
        }
      }
  
  ### Field Descriptions
  
  | Field | Type | Required | Description |
  | --- | --- | --- | --- |
  | `name` | string | Yes | Display name of the plugin in the menu |
  | `level` | integer | No  | Display priority, lower numbers appear first, default 999 |
  | `plugin.name` | string | Yes | Executable file name (including .exe extension) |
  
  ### Configuration Example
  
      {
        "name": "World Manager",
        "level": 2,
        "plugin": {
            "name": "world-manager.exe"
        }
      }
  
  ## Plugin Development Specifications
  
  ### Command Line Arguments
  
  When the user selects "Plugins" from the main menu and chooses a plugin, the program launches the plugin as follows:
  
      <plugin-executable> -<server-version>
  
  For example, if the user selects server version `1.20.1`, the program executes:
  
      backup-tool.exe -1.20.1
  
  ### Plugin Responsibilities
  

1. **Parse command line arguments**: Extract the server version passed by the user
  
2. **Locate the server directory**: Navigate to `server/<version>/` based on the version
  
3. **Execute core functionality**: Perform the plugin's intended operations on the server directory
  
  ### C++ Plugin Template
  
      #include <iostream>
      #include <string>
      #include <filesystem>
      namespace fs = std::filesystem;
      int main(int argc, char* argv[]) {
       // Check command line arguments
       if (argc < 2) {
           std::cerr << "Usage: " << argv[0] << " -<server-version>" << std::endl;
           return 1;
       }
       std::string arg = argv[1];
      
       // Parse version
       if (arg.empty() || arg[0] != '-') {
           std::cerr << "Error: Invalid argument format, expected -<server-version>" << std::endl;
           return 1;
       }
      
       std::string version = arg.substr(1);
      
       // Build server directory path
       std::string serverDir = "server/" + version;
      
       if (!fs::exists(serverDir) || !fs::is_directory(serverDir)) {
           std::cerr << "Error: Server directory does not exist: " << serverDir << std::endl;
           return 1;
       }
      
       std::cout << "Plugin started" << std::endl;
       std::cout << "Server version: " << version << std::endl;
       std::cout << "Server directory: " << serverDir << std::endl;
      
       // Write your plugin core logic here
      
       std::cout << "Plugin execution completed" << std::endl;
       return 0;
      }
  
  ### C# Plugin Template
  
      using System;
      using System.IO;
      class Plugin
      {
       static void Main(string[] args)
       {
           if (args.Length < 1)
           {
               Console.WriteLine("Usage: Plugin.exe -<server-version>");
               return;
           }
           string arg = args[0];
           if (string.IsNullOrEmpty(arg) || arg[0] != '-')
           {
               Console.WriteLine("Error: Invalid argument format, expected -<server-version>");
               return;
           }
           string version = arg.Substring(1);
           string serverDir = Path.Combine("server", version);
           if (!Directory.Exists(serverDir))
           {
               Console.WriteLine("Error: Server directory does not exist: " + serverDir);
               return;
           }
           Console.WriteLine("Plugin started");
           Console.WriteLine("Server version: " + version);
           Console.WriteLine("Server directory: " + serverDir);
           // Write your plugin core logic here
           Console.WriteLine("Plugin execution completed");
       }
      }
  
  ### Python Plugin Template
  
      #!/usr/bin/env python3
      import sys
      import os
      def main():
       if len(sys.argv) < 2:
           print("Usage: plugin.py -<server-version>")
           return
       arg = sys.argv[1]
       if not arg or arg[0] != '-':
           print("Error: Invalid argument format, expected -<server-version>")
           return
       version = arg[1:]
       server_dir = os.path.join("server", version)
       if not os.path.exists(server_dir) or not os.path.isdir(server_dir):
           print(f"Error: Server directory does not exist: {server_dir}")
           return
       print("Plugin started")
       print(f"Server version: {version}")
       print(f"Server directory: {server_dir}")
       # Write your plugin core logic here
       print("Plugin execution completed")
      if __name__ == "__main__":
       main()
  
  ## Plugin Installation
  
4. Create a folder named after your plugin inside the `plugin` directory
  
5. Place `config.json` and the plugin executable file into this folder
  
6. Restart Minecraft Server Launcher or return to the main menu to rescan
  
  ## Important Notes
  
7. Plugin executables must be compatible with the operating system (Windows platform)
  
8. The plugin's current working directory is the program root directory
  
9. Plugins should handle errors and exceptions properly to avoid affecting the main program
  
10. Plugins should not modify or delete main program files
  
11. It is recommended that plugins run in separate windows rather than outputting excessive content in the main console
  
  ## Suggested Plugin Features
  
  Here are some plugin feature suggestions for developers:
  

* Backup Tool: Automatically backup server world folders
* World Manager: Create, delete, and switch worlds
* Player Management: View online players, kick players, ban players
* Performance Monitor: Display server CPU and memory usage
* Scheduled Tasks: Schedule server restarts and command execution
* Log Analyzer: Analyze server logs and generate statistics reports
