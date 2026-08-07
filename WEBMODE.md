# MC 服务器启动器 - Web 模式接口文档

## 概述

MC 服务器启动器支持 Web 模式，通过 HTTP/HTTPS 接口远程管理 Minecraft 服务器。

* * *

## 启动方式

    HTTP 模式（默认端口 4466）
    -webmode on
    
    指定端口
    -webmode on -port 8080
    
    HTTPS 模式（需要证书）
    -webmode on -TLS
    
    关闭 Web 模式
    -webmode off

* * *

## 证书配置

HTTPS 模式需要证书文件，放置在程序根目录下的 `ssl/` 文件夹中：

    ssl/
     ├── server.crt           # SSL 证书
     └── server.key           # SSL 私钥

* * *

## 通用返回格式

### 成功返回

HTTP 状态码：`200`

    {
        "code": 0,
        "message": "操作成功",
        "data": "额外数据（可选）"
    }

### 错误返回

HTTP 状态码：`400` / `404` / `405` / `500`

    {
        "message": "错误描述"
    }

| HTTP 状态码 | 含义  |
| --- | --- |
| 200 | 成功  |
| 400 | 请求参数错误 |
| 404 | 接口不存在 |
| 405 | 请求方法不允许 |
| 500 | 服务器内部错误 |

* * *

## 接口列表

### 1. 查看服务状态

    GET /api/status

**成功返回：**

    {
        "code": 0,
        "message": "running",
        "port": 4466,
        "tls": false,
        "servers": 2
    }

| 字段  | 说明  |
| --- | --- |
| `port` | 当前监听端口 |
| `tls` | 是否启用 HTTPS |
| `servers` | 已安装服务器数量 |

* * *

### 2. 列出已安装服务器

    GET /api/list

**成功返回：**

    {
        "code": 0,
        "message": "success",
        "data": "1.20.1\n1.21\n"
    }

* * *

### 3. 安装服务器（流式）

    GET /api/install?sv=<版本号>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |

**成功响应：** `Transfer-Encoding: chunked`，每行一个 JSON 对象

    {"type":"info","message":"开始安装服务器 1.20.1"}
    {"type":"info","message":"正在获取下载链接..."}
    {"type":"info","message":"开始下载..."}
    {"type":"progress","downloaded":0,"total":102400,"percent":0}
    {"type":"progress","downloaded":51200,"total":102400,"percent":50}
    {"type":"progress","downloaded":102400,"total":102400,"percent":100}
    {"type":"success","message":"下载完成"}
    {"type":"success","message":"服务器 1.20.1 安装完成"}
    {"type":"done"}

| 字段  | 类型  | 说明  |
| --- | --- | --- |
| `type` | string | `info` / `progress` / `success` / `done` |
| `message` | string | 信息内容 |
| `downloaded` | int | 已下载大小（KB） |
| `total` | int | 总大小（KB），未知时为 `-1` |
| `percent` | int | 下载进度百分比 (0-100) |

* * *

### 4. 启动服务器（流式）

    GET /api/start?sv=<版本号>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |

**成功响应：** `Transfer-Encoding: chunked`，每行一个 JSON 对象

    {"type":"info","message":"正在启动服务器 1.20.1"}
    {"type":"info","message":"EULA 已自动同意"}
    {"type":"info","message":"检测到 Fabric 模组加载器"}
    {"type":"info","message":"启动命令: java -Xmx2048M -jar fabric-server-launch.jar"}
    {"type":"info","message":"服务器正在启动..."}
    {"type":"log","line":"[Server] Starting Minecraft server on *:25565"}
    {"type":"log","line":"[Server] Done (2.345s)! For help, type \"help\""}
    {"type":"info","message":"服务器已停止"}
    {"type":"done"}

| 字段  | 类型  | 说明  |
| --- | --- | --- |
| `type` | string | `info` / `log` / `done` |
| `message` | string | 信息内容 |
| `line` | string | 服务器原始日志行 |

* * *

### 5. 删除服务器

    GET /api/remove?sv=<版本号>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |

**成功返回：**

    {
        "code": 0,
        "message": "服务器 1.20.1 已删除"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少 sv 参数"
    }

* * *

### 6. 配置 server.properties

    GET /api/config?sv=<版本号>&key=<配置键>&value=<配置值>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |
| `key` | 是   | 配置键名 | `online-mode` |
| `value` | 是   | 配置值 | `false` |

**常用配置键：**

| key | 说明  | 可选值 |
| --- | --- | --- |
| `online-mode` | 正版验证 | `true` / `false` |
| `difficulty` | 难度  | `peaceful` / `easy` / `normal` / `hard` |
| `gamemode` | 游戏模式 | `survival` / `creative` / `adventure` / `spectator` |
| `server-port` | 服务器端口 | 数字，默认 `25565` |
| `max-players` | 最大玩家数 | 数字，默认 `20` |
| `motd` | 服务器欢迎语 | 字符串 |
| `view-distance` | 视距  | 数字，默认 `10` |
| `allow-nether` | 允许下界 | `true` / `false` |
| `allow-flight` | 允许飞行 | `true` / `false` |
| `enable-command-block` | 启用命令方块 | `true` / `false` |
| `spawn-protection` | 出生点保护范围 | 数字，默认 `16` |
| `level-name` | 世界文件夹名称 | 字符串，默认 `world` |

**成功返回：**

    {
        "code": 0,
        "message": "配置已更新: online-mode=false"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: sv, key, value"
    }

* * *

### 7. 安装自动配置

    GET /api/autoconfig?key=<配置键>&value=<配置值>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `key` | 是   | 配置键名 | `online-mode` |
| `value` | 是   | 配置值 | `false` |

**成功返回：**

    {
        "code": 0,
        "message": "自动配置已更新"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: key, value"
    }

* * *

### 8. 配置启动选项

    GET /api/startconfig?sv=<版本号>&key=<配置键>&value=<配置值>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |
| `key` | 是   | 配置键名 | `maxMemory` |
| `value` | 是   | 配置值 | `2048M` |

**支持的 key：**

| key | 说明  | 示例值 |
| --- | --- | --- |
| `minMemory` | 最小内存 | `1024M` / `2G` |
| `maxMemory` | 最大内存 | `2048M` / `4G` |
| `useOptimization` | 性能优化 | `true` / `false` |
| `additionalJvmArgs` | 额外 JVM 参数 | `-XX:+UseG1GC` |
| `useNogui` | nogui 模式 | `true` / `false` |

**成功返回：**

    {
        "code": 0,
        "message": "启动配置已更新"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: sv, key, value"
    }

* * *

### 9. 安装 Fabric 模组加载器

    GET /api/install-loader?sv=<版本号>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |

**成功返回：**

    {
        "code": 0,
        "message": "Fabric 安装成功"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少 sv 参数"
    }

* * *

### 10. 安装模组（上传文件）

    POST /api/install-mod?sv=<版本号>
    Content-Type: multipart/form-data

**参数：**

| 参数  | 位置  | 必填  | 说明  |
| --- | --- | --- | --- |
| `sv` | URL Query | 是   | Minecraft 版本号 |
| `file` | Body (multipart) | 是   | jar 模组文件 |

**成功返回：**

    {
        "code": 0,
        "message": "模组安装成功: fabric-api.jar"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "该版本未安装 Fabric 模组加载器"
    }

**curl 示例：**

    curl -X POST "http://127.0.0.1:4466/api/install-mod?sv=1.20.1" \
      -F "file=@fabric-api.jar"

* * *

### 11. 从 Modrinth 下载模组

    GET /api/install-mod-modrinth?sv=<版本号>&project=<项目名>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |
| `project` | 是   | Modrinth 项目名（slug） | `fabric-api` |

**成功返回：**

    {
        "code": 0,
        "message": "模组下载完成"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: sv, project"
    }

* * *

### 12. 查看模组列表

    GET /api/list-mod?sv=<版本号>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |

**成功返回：**

    {
        "code": 0,
        "message": "success",
        "data": "fabric-api.jar\noptifine.jar\n"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少 sv 参数"
    }

* * *

### 13. 删除模组

    GET /api/delete-mod?sv=<版本号>&mod=<模组文件名>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |
| `mod` | 是   | 模组文件名 | `fabric-api.jar` |

**成功返回：**

    {
        "code": 0,
        "message": "已删除: fabric-api.jar"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: sv, mod"
    }

* * *

### 14. 查看最新版本

    GET /api/new-versions

**成功返回：**

    {
        "code": 0,
        "message": "success",
        "data": "1.20.1 <推荐>\n1.20\n1.19.4 <推荐>\n"
    }

* * *

### 15. 运行插件

    GET /api/plugin?sv=<版本号>&name=<插件名>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `sv` | 是   | Minecraft 版本号 | `1.20.1` |
| `name` | 是   | 插件名称 | `backup` |

**成功返回：**

    {
        "code": 0,
        "message": "插件 backup 已启动"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少参数: sv, name"
    }

* * *

### 16. 关于

    GET /api/about

**成功返回：**

    {
        "code": 0,
        "message": "MC服务器管理程序\n作者: skc\nGitHub: https://github.com/skc-cmd/Minecraft-Server-Launcher\nGPL V3"
    }

* * *

### 17. 发送反馈

    GET /api/feedback?data=<反馈内容>

**参数：**

| 参数  | 必填  | 说明  | 示例  |
| --- | --- | --- | --- |
| `data` | 是   | 反馈内容 | `这里写反馈内容` |

**成功返回：**

    {
        "code": 0,
        "message": "反馈已发送"
    }

**错误返回：**

HTTP 状态码：`400`

    {
        "message": "缺少 data 参数"
    }

* * *

## 错误示例

### 400 - 缺少参数

HTTP 状态码：`400`

    GET /api/start?sv=

    {
        "message": "缺少 sv 参数"
    }

### 404 - 接口不存在

HTTP 状态码：`404`

    GET /api/xxx

    {
        "message": "接口不存在: /api/xxx"
    }

### 405 - 方法不允许

HTTP 状态码：`405`

    POST /api/status

    {
        "message": "Method Not Allowed"
    }

* * *

## 接口汇总

| 接口  | 方法  | 流式  | 参数  |
| --- | --- | --- | --- |
| `/api/status` | GET | ❌   | 无   |
| `/api/list` | GET | ❌   | 无   |
| `/api/install` | GET | ✅   | `sv` |
| `/api/start` | GET | ✅   | `sv` |
| `/api/remove` | GET | ❌   | `sv` |
| `/api/config` | GET | ❌   | `sv`, `key`, `value` |
| `/api/autoconfig` | GET | ❌   | `key`, `value` |
| `/api/startconfig` | GET | ❌   | `sv`, `key`, `value` |
| `/api/install-loader` | GET | ❌   | `sv` |
| `/api/install-mod` | POST | ❌   | `sv` + `file` |
| `/api/install-mod-modrinth` | GET | ❌   | `sv`, `project` |
| `/api/list-mod` | GET | ❌   | `sv` |
| `/api/delete-mod` | GET | ❌   | `sv`, `mod` |
| `/api/new-versions` | GET | ❌   | 无   |
| `/api/plugin` | GET | ❌   | `sv`, `name` |
| `/api/about` | GET | ❌   | 无   |
| `/api/feedback` | GET | ❌   | `data` |

* * *

## 完整 API 地址示例

    /api/status
    /api/list
    /api/install?sv=1.20.1
    /api/start?sv=1.20.1
    /api/remove?sv=1.20.1
    /api/config?sv=1.20.1&key=online-mode&value=false
    /api/autoconfig?key=online-mode&value=false
    /api/startconfig?sv=1.20.1&key=maxMemory&value=2048M
    /api/install-loader?sv=1.20.1
    /api/list-mod?sv=1.20.1
    /api/delete-mod?sv=1.20.1&mod=fabric-api.jar
    /api/new-versions
    /api/plugin?sv=1.20.1&name=backup
    /api/about
    /api/feedback?data=hello

* * *

## 注意事项

1. `sv` 参数是 Minecraft 版本号，如 `1.20.1`、`1.21`、`26.2`
  
2. 模组安装需要先安装 Fabric 模组加载器
  
3. `project` 参数使用 Modrinth 项目名（slug），如 `fabric-api`
