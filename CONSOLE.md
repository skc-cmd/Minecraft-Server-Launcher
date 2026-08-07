# Minecraft-Server-Launcher - 命令行文档

## 概述

Minecraft-Server-Launcher 支持命令行模式，通过参数直接执行操作，无需进入交互界面。

---

## 基础用法

```
Minecraft-Server-Launcher.exe <命令> [参数]
```

---

## 命令列表

### 信息类

| 命令 | 说明 |
|------|------|
| `-h` / `--help` / `/?` | 显示帮助信息 |
| `-a` / `--about` | 查看关于信息 |
| `-listserver` | 列出已安装的服务器 |
| `-shownewserver` | 查看可用版本 |

---

### 服务器管理

| 命令 | 参数 | 说明 |
|------|------|------|
| `-install` | `-sv <版本>` | 安装指定版本服务器 |
| `-start` | `-sv <版本>` | 启动指定版本服务器 |
| `-remove` | `-sv <版本>` | 删除指定版本服务器 |

---

### 配置管理

| 命令 | 参数 | 说明 |
|------|------|------|
| `-configserver` | `-sv <版本> -key <键> -value <值>` | 配置 server.properties |
| `-autoconfig` | `-key <键> -value <值>` | 设置自动配置 |
| `-startconfig` | `-sv <版本> -key <键> -value <值>` | 配置启动选项 |

---

### 模组管理

| 命令 | 参数 | 说明 |
|------|------|------|
| `-install-loader` | `-sv <版本>` | 安装 Fabric 模组加载器 |
| `-install-mod` | `-sv <版本>` | 安装模组（打开文件选择对话框） |
| `-list-mod` | `-sv <版本>` | 列出已安装模组 |
| `-movemod` | `-from <路径> -to <路径>` | 移动模组文件 |

---

### 其他

| 命令 | 参数 | 说明 |
|------|------|------|
| `-plugin` | `-name <名称>` | 运行插件 |
| `-feedback` | `-data <内容>` | 发送反馈 |
| `-webmode` | `on` / `off` | 开启/关闭 Web 模式 |
| `-TLS` | 无 | 启用 TLS（需配合 `-webmode on`） |
| `-port` | `<端口>` | 指定端口（默认 4466） |

---

## 示例

```bash
# 安装服务器
Minecraft-Server-Launcher.exe -install -sv 1.20.1

# 启动服务器
Minecraft-Server-Launcher.exe -start -sv 1.20.1

# 删除服务器
Minecraft-Server-Launcher.exe -remove -sv 1.20.1

# 配置 server.properties
Minecraft-Server-Launcher.exe -configserver -sv 1.20.1 -key online-mode -value false

# 启动 Web 模式
Minecraft-Server-Launcher.exe -webmode on

# 启动 Web 模式（HTTPS + 自定义端口）
Minecraft-Server-Launcher.exe -webmode on -TLS -port 8080

# 关闭 Web 模式
Minecraft-Server-Launcher.exe -webmode off

# 列出服务器
Minecraft-Server-Launcher.exe -listserver

# 查看帮助
Minecraft-Server-Launcher.exe -h
```

---

## 别名

`-sv` 可使用以下别名：
- `-serverv`
- `-server-version`
- `-sversion`
