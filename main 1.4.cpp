// MCServerManager.cpp
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <regex>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <thread>
#include <chrono>
#include <algorithm>

// 添加nlohmann/json库
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")

namespace fs = std::filesystem;

// 全局配置
const std::string BASE_DIR = "server";
const std::string CONFIG_FILE = "server_config.txt";
const std::string AUTO_CONFIG_FILE = "auto_config.txt";
const std::string VERSION_MANIFEST_URL = "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json";
const std::string EULA_AGREED_FILE = "eula_agreed.txt";

// 颜色定义
const int COLOR_DEFAULT = 7;
const int COLOR_GREEN = 10;
const int COLOR_RED = 12;
const int COLOR_YELLOW = 14;
const int COLOR_CYAN = 11;

// 版本配置结构
struct ServerVersion {
    std::string version;
    std::string jarPath;
    std::map<std::string, std::string> properties;
    std::string minMemory = "";
    std::string maxMemory = "";
    std::string javaPath = "java";
    bool useOptimization = false;
    std::string additionalJvmArgs = "";
    bool useNogui = false;
};

// 模组加载器信息结构
struct ModLoaderInfo {
    std::string name;           // 显示名称
    std::string id;             // 唯一标识
    std::string installerUrl;   // 安装器下载URL
    std::string installerJar;   // 安装器jar文件名
    std::string mainClass;      // 主类名（用于启动）
    std::string generatedJar;   // 生成的启动jar文件名
    bool supported;             // 是否支持
};

// 插件信息结构
struct PluginInfo {
    std::string name;           // 插件名称
    std::string folderName;     // 插件文件夹名称
    std::string exeName;        // 可执行文件名
    int level;                  // 显示优先级（越小越靠前）
    std::string exePath;        // 可执行文件完整路径
};

// 全局变量
std::map<std::string, ServerVersion> servers;
std::map<std::string, std::string> autoConfigs;
std::string g_javaPath = "java";
int g_javaVersion = 0;
std::vector<PluginInfo> g_plugins;

// ========== 函数声明 ==========
void setConsoleColor(int color);
void resetConsoleColor();
void clearScreen();
void showSeparator();
void showTitle(const std::string& title);
void showSuccess(const std::string& message);
void showError(const std::string& message);
void showInfo(const std::string& message);
void showDebug(const std::string& message);
void showWaiting(const std::string& message);
void showDisclaimer();
bool checkEulaAgreed();
void saveEulaAgreed();

bool directoryExists(const std::string& path);
bool createDirectory(const std::string& path);
bool removeDirectory(const std::string& path);
bool fileExists(const std::string& path);
std::string detectJavaPath();
std::string openFileDialog(const std::string& title, const std::string& filter);

std::string downloadToString(const std::string& url);
bool downloadFile(const std::string& url, const std::string& outputPath);
std::string getVersionManifest();
std::string getServerDownloadUrl(const std::string& version);
bool validateServerProperty(const std::string& key, const std::string& value);
bool isReleaseVersion(const std::string& version);

void listAvailableVersions();
void loadAutoConfigs();
void saveAutoConfigs();
void loadServerConfigs();
void saveServerConfigs();
void installServer();
void startServer();
void deleteServer();
void configureServer();
void configureAutoInstall();
void configureStartupOptions();

// 模组加载器相关函数
std::vector<ModLoaderInfo> getSupportedLoaders();
std::string getLoaderVersion(const std::string& loaderId);
std::string buildInstallCommand(const ModLoaderInfo& loader, const std::string& mcVersion, const std::string& loaderVersion, const std::string& installerPath);
void installModLoader();
void installMod();

// 插件相关函数
void scanPlugins();
void showPluginMenu();
void runPlugin();

void listInstalledServers();
void showMenu();
std::string formatFileSize(DWORD size);

// ========== 函数定义 ==========

// 控制台颜色函数
void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resetConsoleColor() {
    setConsoleColor(COLOR_DEFAULT);
}

// 清屏函数
void clearScreen() {
    system("cls");
}

// 显示分隔线
void showSeparator() {
    std::cout << "========================================" << std::endl;
}

// 显示标题
void showTitle(const std::string& title) {
    setConsoleColor(COLOR_CYAN);
    showSeparator();
    std::cout << "  " << title << std::endl;
    showSeparator();
    resetConsoleColor();
}

// 显示成功消息
void showSuccess(const std::string& message) {
    setConsoleColor(COLOR_GREEN);
    std::cout << "[OK] " << message << std::endl;
    resetConsoleColor();
}

// 显示错误消息
void showError(const std::string& message) {
    setConsoleColor(COLOR_RED);
    std::cout << "[ERROR] " << message << std::endl;
    resetConsoleColor();
}

// 显示信息消息
void showInfo(const std::string& message) {
    setConsoleColor(COLOR_YELLOW);
    std::cout << "[INFO] " << message << std::endl;
    resetConsoleColor();
}

// 显示调试消息
void showDebug(const std::string& message) {
    setConsoleColor(COLOR_CYAN);
    std::cout << "[DEBUG] " << message << std::endl;
    resetConsoleColor();
}

void showWaiting(const std::string& message) {
    std::cout << message;
    for (int i = 0; i < 3; i++) {
        std::cout << ".";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << std::endl;
}

// ===== EULA相关函数 =====
bool checkEulaAgreed() {
    std::string eulaFile = EULA_AGREED_FILE;
    if (fileExists(eulaFile)) {
        std::ifstream file(eulaFile);
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("eula_agreed=true") != std::string::npos) {
                return true;
            }
        }
        file.close();
    }
    return false;
}

void saveEulaAgreed() {
    std::ofstream file(EULA_AGREED_FILE);
    if (file.is_open()) {
        file << "#Minecraft EULA Agreement" << std::endl;
        file << "#https://www.minecraft.net/eula" << std::endl;
        file << "eula_agreed=true" << std::endl;
        file.close();
    }
}

// ===== 显示免责声明 =====
void showDisclaimer() {
    clearScreen();
    showTitle("免责声明与用户协议");

    setConsoleColor(COLOR_YELLOW);
    std::cout << "【重要提示】" << std::endl;
    resetConsoleColor();
    std::cout << std::endl;
    std::cout << "1. 本程序为第三方启动器，并非Minecraft官方产品。" << std::endl;
    std::cout << "   未经Mojang Studios或Microsoft批准，也不与之关联。" << std::endl;
    std::cout << std::endl;
    std::cout << "2. Minecraft版权归Mojang Studios所有。" << std::endl;
    std::cout << "   本程序仅提供服务器管理功能，不包含游戏本体。" << std::endl;
    std::cout << std::endl;
    std::cout << "3. 用户需自行购买正版Minecraft游戏。" << std::endl;
    std::cout << "   使用本程序即表示您已同意Minecraft EULA。" << std::endl;
    std::cout << std::endl;
    std::cout << "4. 本程序仅供学习和个人使用，请勿用于商业用途。" << std::endl;
    std::cout << std::endl;
    std::cout << "5. 使用本程序所产生的任何后果由用户自行承担。" << std::endl;
    std::cout << std::endl;

    setConsoleColor(COLOR_GREEN);
    std::cout << "Minecraft EULA: https://www.minecraft.net/eula" << std::endl;
    resetConsoleColor();
    std::cout << std::endl;
    showSeparator();
    std::cout << std::endl;

    char confirm;
    std::cout << "请仔细阅读以上内容，输入 y 表示同意并继续，输入 n 退出: ";
    std::cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "\n您已拒绝用户协议，程序退出。" << std::endl;
        exit(0);
    }

    saveEulaAgreed();
    showSuccess("已同意用户协议");
    std::cout << std::endl;
    std::cout << "按任意键继续...";
    std::cin.ignore();
    std::cin.get();
    clearScreen();
}

// ===== 文件系统工具函数 =====
bool directoryExists(const std::string& path) {
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool createDirectory(const std::string& path) {
    return CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool removeDirectory(const std::string& path) {
    std::string cmd = "rmdir /s /q \"" + path + "\"";
    return system(cmd.c_str()) == 0;
}

bool fileExists(const std::string& path) {
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

// ===== 文件选择对话框 =====
std::string openFileDialog(const std::string& title, const std::string& filter) {
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileNameA(&ofn)) {
        return std::string(fileName);
    }
    return "";
}

// ===== Java检测函数 =====
std::string detectJavaPath() {
    int highestVersion = 0;
    std::string bestJavaPath = "java";

    showInfo("正在检测Java版本...");

    // 使用where命令查找所有java
    std::string cmd = "where java 2>nul";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (pipe != NULL) {
        char buffer[256];
        std::vector<std::string> javaPaths;

        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            std::string path = buffer;
            if (!path.empty() && path[path.length() - 1] == '\n') {
                path.erase(path.length() - 1);
            }
            if (!path.empty() && path[path.length() - 1] == '\r') {
                path.erase(path.length() - 1);
            }
            if (!path.empty()) {
                javaPaths.push_back(path);
            }
        }
        _pclose(pipe);

        for (size_t i = 0; i < javaPaths.size(); i++) {
            std::string versionCmd = "\"" + javaPaths[i] + "\" -version 2>&1";
            FILE* versionPipe = _popen(versionCmd.c_str(), "r");
            if (versionPipe != NULL) {
                char versionBuffer[512];
                while (fgets(versionBuffer, sizeof(versionBuffer), versionPipe) != NULL) {
                    std::string line = versionBuffer;
                    size_t pos = line.find("version");
                    if (pos != std::string::npos) {
                        size_t start = line.find("\"", pos);
                        if (start != std::string::npos) {
                            size_t end = line.find("\"", start + 1);
                            if (end != std::string::npos) {
                                std::string verStr = line.substr(start + 1, end - start - 1);
                                size_t dotPos = verStr.find(".");
                                if (dotPos != std::string::npos) {
                                    int majorVer = atoi(verStr.substr(0, dotPos).c_str());
                                    if (majorVer > highestVersion) {
                                        highestVersion = majorVer;
                                        bestJavaPath = javaPaths[i];
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
                _pclose(versionPipe);
            }
        }
    }

    if (highestVersion == 0) {
        FILE* testPipe = _popen("java -version 2>&1", "r");
        if (testPipe != NULL) {
            char testBuffer[512];
            while (fgets(testBuffer, sizeof(testBuffer), testPipe) != NULL) {
                std::string line = testBuffer;
                if (line.find("version") != std::string::npos) {
                    bestJavaPath = "java";
                    highestVersion = 1;
                    break;
                }
            }
            _pclose(testPipe);
        }
    }

    g_javaVersion = highestVersion;

    if (highestVersion > 0) {
        showSuccess("找到Java版本: " + std::to_string(highestVersion));
        showInfo("Java路径: " + bestJavaPath);
        return bestJavaPath;
    }

    return "";
}

// ===== HTTP下载函数 =====
std::string downloadToString(const std::string& url) {
    showDebug("正在请求: " + url);

    HINTERNET hInternet = InternetOpenA(
        "MCServerManager/1.0",
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0
    );

    if (!hInternet) {
        showError("InternetOpen失败");
        return "";
    }

    DWORD timeout = 30000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE;

    HINTERNET hFile = InternetOpenUrlA(
        hInternet,
        url.c_str(),
        NULL,
        0,
        flags,
        0
    );

    if (!hFile) {
        DWORD error = GetLastError();
        showError("无法打开URL: " + url + " (错误代码: " + std::to_string(error) + ")");
        InternetCloseHandle(hInternet);
        return "";
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &statusCode, &statusCodeSize, NULL);

    if (statusCode != 200) {
        showError("HTTP错误: " + std::to_string(statusCode));
        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string result;
    char buffer[8192];
    DWORD bytesRead;

    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        result.append(buffer, bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    if (result.empty()) {
        showError("下载内容为空");
        return "";
    }

    showDebug("下载完成，大小: " + std::to_string(result.size()) + " 字节");
    return result;
}

// 格式化文件大小
std::string formatFileSize(DWORD size) {
    if (size >= 1024 * 1024) {
        double mb = static_cast<double>(size) / (1024 * 1024);
        char buf[32];
        sprintf_s(buf, "%.1f MB", mb);
        return std::string(buf);
    }
    else if (size >= 1024) {
        return std::to_string(size / 1024) + " KB";
    }
    else {
        return std::to_string(size) + " B";
    }
}

bool downloadFile(const std::string& url, const std::string& outputPath) {
    showInfo("开始下载...");

    HINTERNET hInternet = InternetOpenA(
        "MCServerManager/1.0",
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0
    );

    if (!hInternet) {
        showError("InternetOpen失败");
        return false;
    }

    DWORD timeout = 60000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE;

    HINTERNET hFile = InternetOpenUrlA(
        hInternet,
        url.c_str(),
        NULL,
        0,
        flags,
        0
    );

    if (!hFile) {
        DWORD error = GetLastError();
        showError("无法打开URL: " + url + " (错误代码: " + std::to_string(error) + ")");
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        &statusCode, &statusCodeSize, NULL);

    if (statusCode != 200) {
        showError("HTTP错误: " + std::to_string(statusCode));
        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);
        return false;
    }

    HANDLE hOutput = CreateFileA(
        outputPath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hOutput == INVALID_HANDLE_VALUE) {
        showError("无法创建文件: " + outputPath);
        InternetCloseHandle(hFile);
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[8192];
    DWORD bytesRead;
    DWORD totalBytes = 0;
    DWORD lastPrintTime = GetTickCount();
    DWORD lastTotalBytes = 0;
    int progressMarks = 0;
    bool downloadStarted = false;

    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        DWORD bytesWritten;
        WriteFile(hOutput, buffer, bytesRead, &bytesWritten, NULL);
        totalBytes += bytesRead;

        DWORD currentTime = GetTickCount();
        if (currentTime - lastPrintTime > 500 || totalBytes - lastTotalBytes > 1024 * 1024) {
            downloadStarted = true;

            int newMarks = totalBytes / (1024 * 1024);
            if (newMarks > progressMarks) {
                progressMarks = newMarks;
            }

            std::cout << "\r已下载: " << formatFileSize(totalBytes);

            int displayMarks = (progressMarks > 50) ? 50 : progressMarks;
            std::cout << " [";
            for (int i = 0; i < displayMarks; i++) {
                std::cout << "=";
            }
            if (displayMarks < 50) {
                std::cout << ">";
            }
            for (int i = displayMarks + 1; i < 50; i++) {
                std::cout << " ";
            }
            std::cout << "]";
            std::cout << std::flush;

            lastPrintTime = currentTime;
            lastTotalBytes = totalBytes;
        }
    }

    if (downloadStarted) {
        std::cout << "\r已下载: " << formatFileSize(totalBytes);
        std::cout << " [";
        for (int i = 0; i < 50; i++) std::cout << "=";
        std::cout << "]";
        std::cout << std::endl;
    }
    else {
        std::cout << std::endl;
    }

    CloseHandle(hOutput);
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    if (totalBytes == 0) {
        showError("下载失败，文件大小为0");
        DeleteFileA(outputPath.c_str());
        return false;
    }

    showSuccess("下载完成，大小: " + formatFileSize(totalBytes));
    return true;
}

// ===== 版本判断函数 =====
bool isReleaseVersion(const std::string& version) {
    std::string lowerV = version;
    for (char& c : lowerV) c = tolower(c);

    if (lowerV.find("rc") != std::string::npos ||
        lowerV.find("pre") != std::string::npos ||
        lowerV.find("snapshot") != std::string::npos ||
        lowerV.find("alpha") != std::string::npos ||
        lowerV.find("beta") != std::string::npos) {
        return false;
    }
    return true;
}

// ===== Minecraft版本相关函数 =====
std::string getVersionManifest() {
    std::string result = downloadToString(VERSION_MANIFEST_URL);
    if (result.empty()) {
        showError("无法获取版本清单");
        return "";
    }
    return result;
}

// 使用nlohmann/json解析版本信息并获取下载链接
std::string getServerDownloadUrl(const std::string& version) {
    showInfo("正在获取版本信息...");

    std::string manifestJson = getVersionManifest();
    if (manifestJson.empty()) {
        return "";
    }

    try {
        json manifest = json::parse(manifestJson);

        if (!manifest.contains("versions")) {
            showError("版本清单格式错误: 缺少versions字段");
            return "";
        }

        auto versions = manifest["versions"];
        if (!versions.is_array()) {
            showError("版本清单格式错误: versions不是数组");
            return "";
        }

        std::string exactMatchUrl = "";
        std::vector<std::string> matches;
        std::vector<bool> isRelease;

        for (const auto& v : versions) {
            if (!v.contains("id") || !v["id"].is_string()) continue;

            std::string id = v["id"];

            if (id == version) {
                if (v.contains("url") && v["url"].is_string()) {
                    exactMatchUrl = v["url"];
                }
            }

            if (id.find(version) != std::string::npos) {
                matches.push_back(id);
                isRelease.push_back(isReleaseVersion(id));
            }
        }

        if (!exactMatchUrl.empty()) {
            showDebug("精确匹配成功: " + version);
            std::string versionJson = downloadToString(exactMatchUrl);
            if (versionJson.empty()) {
                showError("无法获取版本详细信息");
                return "";
            }

            try {
                json versionInfo = json::parse(versionJson);

                if (versionInfo.contains("downloads") &&
                    versionInfo["downloads"].contains("server") &&
                    versionInfo["downloads"]["server"].contains("url")) {
                    std::string downloadUrl = versionInfo["downloads"]["server"]["url"];
                    showDebug("获取到下载链接: " + downloadUrl);
                    return downloadUrl;
                }
                else {
                    showError("该版本没有服务器jar，可能为客户端版本");
                    if (versionInfo.contains("downloads")) {
                        showDebug("downloads字段存在，但缺少server子字段");
                    }
                    else {
                        showDebug("版本信息中缺少downloads字段");
                    }
                    return "";
                }
            }
            catch (const json::parse_error& e) {
                showError("解析版本详细信息失败: " + std::string(e.what()));
                return "";
            }
        }

        if (matches.empty()) {
            showError("未找到版本: " + version);
            return "";
        }

        if (matches.size() == 1) {
            showInfo("找到匹配版本: " + matches[0]);
            return getServerDownloadUrl(matches[0]);
        }

        showInfo("找到匹配的版本:");
        showSeparator();

        int index = 1;
        std::vector<std::string> displayList;

        for (size_t i = 0; i < matches.size(); i++) {
            if (isRelease[i]) {
                setConsoleColor(COLOR_GREEN);
                std::cout << "  " << index << ". " << matches[i];
                resetConsoleColor();
                std::cout << " <推荐>" << std::endl;
                displayList.push_back(matches[i]);
                index++;
            }
        }

        for (size_t i = 0; i < matches.size(); i++) {
            if (!isRelease[i]) {
                setConsoleColor(COLOR_YELLOW);
                std::cout << "  " << index << ". " << matches[i];
                resetConsoleColor();
                std::cout << std::endl;
                displayList.push_back(matches[i]);
                index++;
            }
        }
        showSeparator();

        int choice;
        std::cout << "请选择要安装的版本 (输入编号): ";
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > static_cast<int>(displayList.size())) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            showError("输入无效，请输入有效的编号");
            return getServerDownloadUrl(version);
        }

        std::string selectedVersion = displayList[choice - 1];
        showInfo("已选择版本: " + selectedVersion);

        return getServerDownloadUrl(selectedVersion);

    }
    catch (const json::parse_error& e) {
        showError("JSON解析错误: " + std::string(e.what()));
        showDebug("错误位置: " + std::to_string(e.byte));
        return "";
    }
    catch (const std::exception& e) {
        showError("错误: " + std::string(e.what()));
        return "";
    }
}

bool validateServerProperty(const std::string& key, const std::string& value) {
    const char* validKeysArray[] = {
        "server-port", "max-players", "motd", "difficulty", "gamemode",
        "spawn-protection", "view-distance", "max-build-height",
        "online-mode", "allow-nether", "allow-flight", "level-name",
        "level-seed", "level-type", "generate-structures",
        "max-world-size", "enable-query", "enable-rcon", "rcon.password",
        "enable-command-block", "spawn-monsters", "spawn-animals",
        "enforce-whitelist", "resource-pack", "entity-broadcast-range-percentage"
    };
    int arraySize = sizeof(validKeysArray) / sizeof(validKeysArray[0]);

    for (int i = 0; i < arraySize; i++) {
        if (key == validKeysArray[i]) return true;
    }

    return true;
}

// ===== 模组加载器相关函数 =====

// 获取支持的加载器列表（界面仅显示Fabric，其他预留接口）
std::vector<ModLoaderInfo> getSupportedLoaders() {
    std::vector<ModLoaderInfo> loaders;

    // Fabric - 当前唯一支持的加载器
    ModLoaderInfo fabric;
    fabric.name = "Fabric";
    fabric.id = "fabric";
    fabric.installerUrl = "https://maven.fabricmc.net/net/fabricmc/fabric-installer/1.1.1/fabric-installer-1.1.1.jar";
    fabric.installerJar = "fabric-installer-1.1.1.jar";
    fabric.mainClass = "net.fabricmc.loader.impl.launch.server.FabricServerLauncher";
    fabric.generatedJar = "fabric-server-launch.jar";
    fabric.supported = true;
    loaders.push_back(fabric);

    return loaders;
}

// 获取加载器版本（不同加载器有不同的获取方式）
std::string getLoaderVersion(const std::string& loaderId) {
    if (loaderId == "fabric") {
        // Fabric: 从meta API获取
        std::string apiUrl = "https://meta.fabricmc.net/v2/versions/loader";
        std::string apiResponse = downloadToString(apiUrl);
        if (!apiResponse.empty()) {
            try {
                json loaderJson = json::parse(apiResponse);
                if (loaderJson.is_array() && !loaderJson.empty()) {
                    for (const auto& item : loaderJson) {
                        if (item.contains("version") && item["version"].is_string()) {
                            return item["version"];
                        }
                    }
                }
            }
            catch (const json::parse_error& e) {
                showDebug("解析加载器版本失败");
            }
        }
        return "0.16.10"; // 默认版本
    }
    return "";
}

// 构建安装命令（不同加载器命令不同）
std::string buildInstallCommand(const ModLoaderInfo& loader, const std::string& mcVersion, const std::string& loaderVersion, const std::string& installerPath) {
    if (loader.id == "fabric") {
        // 获取文件名（不含路径）
        size_t lastSlash = installerPath.find_last_of("/\\");
        std::string installerFileName = (lastSlash != std::string::npos) ?
            installerPath.substr(lastSlash + 1) :
            installerPath;
        return "java -jar \"" + installerFileName + "\" server -mcversion " + mcVersion + " -loader " + loaderVersion + " -downloadMinecraft";
    }
    return "";
}

// ===== 功能函数 =====

void listAvailableVersions() {
    clearScreen();
    showTitle("可用的Minecraft版本");

    showInfo("正在获取可用版本列表...");

    std::string manifestJson = getVersionManifest();
    if (manifestJson.empty()) {
        return;
    }

    try {
        json manifest = json::parse(manifestJson);

        if (!manifest.contains("versions") || !manifest["versions"].is_array()) {
            showError("版本清单格式错误");
            return;
        }

        auto versions = manifest["versions"];

        std::cout << std::endl;
        int count = 0;

        std::vector<std::pair<std::string, bool> > versionList;

        for (const auto& v : versions) {
            if (!v.contains("id") || !v["id"].is_string()) continue;

            std::string id = v["id"];
            bool isRelease = isReleaseVersion(id);
            versionList.push_back(std::make_pair(id, isRelease));
            count++;

            if (count >= 20) break;
        }

        std::cout << "最新版本 (绿色为推荐稳定版):" << std::endl;
        showSeparator();

        for (size_t i = 0; i < versionList.size(); i++) {
            if (versionList[i].second) {
                setConsoleColor(COLOR_GREEN);
                std::cout << "  - " << versionList[i].first;
                resetConsoleColor();
                std::cout << " <推荐>" << std::endl;
            }
            else {
                setConsoleColor(COLOR_YELLOW);
                std::cout << "  - " << versionList[i].first;
                resetConsoleColor();
                std::cout << std::endl;
            }
        }
        showSeparator();
        std::cout << std::endl;
        showInfo("显示最新的20个版本");
        std::cout << std::endl;

    }
    catch (const json::parse_error& e) {
        showError("JSON解析错误: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        showError("错误: " + std::string(e.what()));
    }
}

void loadAutoConfigs() {
    std::ifstream file(AUTO_CONFIG_FILE);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            autoConfigs[key] = value;
        }
    }
    file.close();
}

void saveAutoConfigs() {
    std::ofstream file(AUTO_CONFIG_FILE);
    if (!file.is_open()) {
        showError("无法保存自动配置文件");
        return;
    }

    for (std::map<std::string, std::string>::const_iterator it = autoConfigs.begin(); it != autoConfigs.end(); ++it) {
        file << it->first << "=" << it->second << std::endl;
    }
    file.close();
}

void loadServerConfigs() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return;

    std::string line;
    ServerVersion currentServer;
    bool inServer = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line.find("[Server:") == 0) {
            if (inServer) {
                servers[currentServer.version] = currentServer;
                currentServer = ServerVersion();
            }
            std::string version = line.substr(8, line.find(']') - 8);
            currentServer.version = version;
            inServer = true;
        }
        else if (line.find("jarPath=") == 0) {
            currentServer.jarPath = line.substr(8);
        }
        else if (line.find("minMemory=") == 0) {
            currentServer.minMemory = line.substr(10);
        }
        else if (line.find("maxMemory=") == 0) {
            currentServer.maxMemory = line.substr(10);
        }
        else if (line.find("javaPath=") == 0) {
            currentServer.javaPath = line.substr(9);
        }
        else if (line.find("useOptimization=") == 0) {
            currentServer.useOptimization = (line.substr(16) == "true");
        }
        else if (line.find("additionalJvmArgs=") == 0) {
            currentServer.additionalJvmArgs = line.substr(18);
        }
        else if (line.find("useNogui=") == 0) {
            currentServer.useNogui = (line.substr(9) == "true");
        }
        else if (line.find("property:") == 0) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(9, pos - 9);
                std::string value = line.substr(pos + 1);
                currentServer.properties[key] = value;
            }
        }
    }

    if (inServer) {
        servers[currentServer.version] = currentServer;
    }
    file.close();
}

void saveServerConfigs() {
    std::ofstream file(CONFIG_FILE);
    if (!file.is_open()) {
        showError("无法保存配置文件");
        return;
    }

    for (std::map<std::string, ServerVersion>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        const std::string& version = it->first;
        const ServerVersion& server = it->second;

        file << "[Server:" << version << "]" << std::endl;
        file << "jarPath=" << server.jarPath << std::endl;
        if (!server.minMemory.empty()) {
            file << "minMemory=" << server.minMemory << std::endl;
        }
        if (!server.maxMemory.empty()) {
            file << "maxMemory=" << server.maxMemory << std::endl;
        }
        file << "javaPath=" << server.javaPath << std::endl;
        file << "useOptimization=" << (server.useOptimization ? "true" : "false") << std::endl;
        if (!server.additionalJvmArgs.empty()) {
            file << "additionalJvmArgs=" << server.additionalJvmArgs << std::endl;
        }
        file << "useNogui=" << (server.useNogui ? "true" : "false") << std::endl;

        for (std::map<std::string, std::string>::const_iterator pit = server.properties.begin(); pit != server.properties.end(); ++pit) {
            file << "property:" << pit->first << "=" << pit->second << std::endl;
        }
        file << std::endl;
    }
    file.close();
}

void installServer() {
    clearScreen();
    showTitle("安装Minecraft服务器");

    std::string versionInput;
    std::cout << "请输入要安装的Minecraft版本 (例如: 1.20.1): ";
    std::cin >> versionInput;

    std::string downloadUrl = getServerDownloadUrl(versionInput);
    if (downloadUrl.empty()) {
        showError("无法获取下载链接，请检查版本号是否正确");
        showInfo("提示: 某些版本可能没有服务器端文件，请尝试选择其他版本");
        return;
    }

    std::string resolvedVersion = versionInput;

    std::string serverDir = BASE_DIR + "/" + resolvedVersion;
    if (directoryExists(serverDir)) {
        showError("版本 " + resolvedVersion + " 已存在");
        return;
    }

    showInfo("下载链接: " + downloadUrl);

    if (!createDirectory(serverDir)) {
        showError("无法创建服务器目录");
        return;
    }

    std::string jarPath = serverDir + "/server.jar";
    showInfo("正在下载服务器jar包...");
    showInfo("文件较大，请耐心等待...");

    if (!downloadFile(downloadUrl, jarPath)) {
        showError("下载失败");
        removeDirectory(serverDir);
        return;
    }

    ServerVersion server;
    server.version = resolvedVersion;
    server.jarPath = jarPath;
    server.useOptimization = false;
    server.useNogui = false;
    server.minMemory = "";
    server.maxMemory = "";

    if (!autoConfigs.empty()) {
        showInfo("正在应用自动配置...");
        for (std::map<std::string, std::string>::const_iterator it = autoConfigs.begin(); it != autoConfigs.end(); ++it) {
            if (validateServerProperty(it->first, it->second)) {
                server.properties[it->first] = it->second;
                showSuccess("应用配置: " + it->first + " = " + it->second);
            }
            else {
                showError("配置不支持: " + it->first + " (已跳过)");
            }
        }
    }

    servers[resolvedVersion] = server;
    saveServerConfigs();

    showSuccess("服务器安装完成！");
    showInfo("服务器位置: " + serverDir);
}

void startServer() {
    clearScreen();
    showTitle("启动Minecraft服务器");

    if (servers.empty()) {
        showError("没有已安装的服务器");
        return;
    }

    listInstalledServers();

    std::string version;
    std::cout << "请输入要启动的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        return;
    }

    ServerVersion& server = it->second;

    if (!fileExists(server.jarPath)) {
        showError("服务器jar文件不存在: " + server.jarPath);
        return;
    }

    std::string serverDir = BASE_DIR + "/" + version;
    std::string startScript = serverDir + "/start.bat";
    std::string eulaPath = serverDir + "/eula.txt";

    // ===== 检查并处理EULA =====
    bool eulaAccepted = false;
    bool eulaExists = fileExists(eulaPath);

    if (eulaExists) {
        std::ifstream eulaFile(eulaPath);
        std::string line;
        while (std::getline(eulaFile, line)) {
            // 去除空格
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (line.find("eula=true") != std::string::npos) {
                eulaAccepted = true;
                break;
            }
        }
        eulaFile.close();
    }

    // 如果EULA未同意或不存在，提示用户
    if (!eulaAccepted) {
        showSeparator();
        setConsoleColor(COLOR_YELLOW);
        std::cout << "  需要同意Minecraft EULA才能启动服务器" << std::endl;
        resetConsoleColor();
        std::cout << std::endl;
        std::cout << "Minecraft EULA: https://www.minecraft.net/eula" << std::endl;
        std::cout << std::endl;
        showSeparator();
        std::cout << std::endl;

        char confirm;
        std::cout << "你是否同意Minecraft最终用户许可协议? (y/n): ";
        std::cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            showInfo("用户拒绝同意EULA，取消启动");
            std::cout << "\n按任意键继续...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        // 用户同意，写入eula.txt
        std::ofstream eulaFile(eulaPath);
        if (eulaFile.is_open()) {
            eulaFile << "#By changing the setting below to TRUE you are indicating your agreement to our EULA (https://www.minecraft.net/eula)" << std::endl;
            eulaFile << "#" << std::endl;
            eulaFile << "eula=true" << std::endl;
            eulaFile.close();
            showSuccess("已同意EULA，eula.txt已创建");
            std::cout << std::endl;
        }
        else {
            showError("无法创建 eula.txt");
            return;
        }
    }

    // 获取当前目录
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::string rootDir = std::string(currentDir);

    std::string absServerDir = rootDir + "\\" + serverDir;
    std::string absStartScript = rootDir + "\\" + startScript;

    // 检测已安装的模组加载器
    std::vector<ModLoaderInfo> loaders = getSupportedLoaders();
    std::string jarFileName;
    bool hasModLoader = false;
    std::string loaderName = "";

    for (size_t i = 0; i < loaders.size(); i++) {
        if (!loaders[i].supported) continue;
        std::string loaderJar = serverDir + "/" + loaders[i].generatedJar;
        if (fileExists(loaderJar)) {
            hasModLoader = true;
            loaderName = loaders[i].name;
            jarFileName = loaders[i].generatedJar;
            showInfo("检测到 " + loaderName + " 模组加载器，将使用 " + jarFileName);
            break;
        }
    }

    if (!hasModLoader) {
        size_t lastSlash = server.jarPath.find_last_of("/\\");
        jarFileName = (lastSlash != std::string::npos) ?
            server.jarPath.substr(lastSlash + 1) :
            server.jarPath;
    }

    // ===== 生成启动脚本 =====
    std::ofstream script(startScript);
    if (!script.is_open()) {
        showError("无法创建启动脚本");
        return;
    }

    for (char& c : absServerDir) {
        if (c == '/') c = '\\';
    }
    for (char& c : absStartScript) {
        if (c == '/') c = '\\';
    }

    script << "@echo off" << std::endl;
    script << "title Minecraft Server " << version;
    if (hasModLoader) script << " (" << loaderName << ")";
    script << std::endl;
    script << "cd /d \"" << absServerDir << "\"" << std::endl;
    script << "echo ========================================" << std::endl;
    script << "echo   Minecraft Server " << version;
    if (hasModLoader) script << " (" << loaderName << ")";
    script << std::endl;
    script << "echo ========================================" << std::endl;
    script << "echo." << std::endl;
    script << "echo 正在启动服务器..." << std::endl;
    script << "echo." << std::endl;

    std::string javaCmd = "java";

    if (!server.minMemory.empty()) {
        javaCmd += " -Xms" + server.minMemory;
    }
    if (!server.maxMemory.empty()) {
        javaCmd += " -Xmx" + server.maxMemory;
    }
    if (!server.additionalJvmArgs.empty()) {
        javaCmd += " " + server.additionalJvmArgs;
    }

    if (server.useOptimization) {
        javaCmd += " -XX:+UseG1GC -XX:+ParallelRefProcEnabled -XX:MaxGCPauseMillis=200";
        javaCmd += " -XX:+UnlockExperimentalVMOptions -XX:+DisableExplicitGC";
        javaCmd += " -XX:+AlwaysPreTouch -XX:G1NewSizePercent=30 -XX:G1MaxNewSizePercent=40";
        javaCmd += " -XX:G1HeapRegionSize=8M -XX:G1ReservePercent=20 -XX:G1HeapWastePercent=5";
        javaCmd += " -XX:G1MixedGCCountTarget=4 -XX:InitiatingHeapOccupancyPercent=15";
        javaCmd += " -XX:G1MixedGCLiveThresholdPercent=90 -XX:G1RSetUpdatingPauseTimePercent=5";
        javaCmd += " -XX:SurvivorRatio=32 -XX:+PerfDisableSharedMem -XX:MaxTenuringThreshold=1";
        javaCmd += " -Dusing.aikars.flags=https://mcflags.emc.gs";
    }

    javaCmd += " -jar \"" + jarFileName + "\"";

    if (server.useNogui) {
        javaCmd += " nogui";
    }

    script << javaCmd << std::endl;
    script << "echo." << std::endl;
    script << "echo 服务器已停止" << std::endl;
    script << "pause" << std::endl;
    script.close();

    showInfo("正在启动服务器...");
    showInfo("启动脚本: " + startScript);

    std::string cmd = "start \"Minecraft Server " + version + "\" \"" + absStartScript + "\"";
    system(cmd.c_str());

    showSuccess("服务器已在新窗口启动！");
    showInfo("如需停止服务器，请在服务器窗口输入 stop 命令");
}

void deleteServer() {
    clearScreen();
    showTitle("删除Minecraft服务器");

    if (servers.empty()) {
        showError("没有已安装的服务器");
        return;
    }

    listInstalledServers();

    std::string version;
    std::cout << "请输入要删除的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        return;
    }

    char confirm;
    std::cout << "确认删除版本 " << version << " 吗? (y/n): ";
    std::cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        showInfo("取消删除");
        return;
    }

    std::string serverDir = BASE_DIR + "/" + version;
    showInfo("正在删除服务器目录...");

    if (!removeDirectory(serverDir)) {
        showError("删除目录失败，请手动删除: " + serverDir);
        return;
    }

    servers.erase(it);
    saveServerConfigs();

    showSuccess("服务器已删除！");
}

void configureServer() {
    clearScreen();
    showTitle("配置服务器 server.properties");

    if (servers.empty()) {
        showError("没有已安装的服务器");
        return;
    }

    listInstalledServers();

    std::string version;
    std::cout << "请输入要配置的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        return;
    }

    std::string serverDir = BASE_DIR + "/" + version;
    std::string propertiesPath = serverDir + "/server.properties";

    std::cout << "\n当前 server.properties 配置:" << std::endl;
    showSeparator();

    std::map<std::string, std::string> currentProps;
    if (fileExists(propertiesPath)) {
        std::ifstream file(propertiesPath);
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                currentProps[key] = value;
                std::cout << "  " << key << " = " << value << std::endl;
            }
        }
        file.close();
    }
    else {
        std::cout << "  server.properties 文件不存在，将创建默认配置" << std::endl;
        currentProps["server-port"] = "25565";
        currentProps["max-players"] = "20";
        currentProps["motd"] = "A Minecraft Server";
        currentProps["difficulty"] = "easy";
        currentProps["gamemode"] = "survival";
        currentProps["online-mode"] = "true";
        currentProps["level-name"] = "world";
        currentProps["view-distance"] = "10";
        currentProps["spawn-protection"] = "16";
        currentProps["allow-nether"] = "true";
        currentProps["allow-flight"] = "false";
        currentProps["enable-command-block"] = "false";
        currentProps["max-world-size"] = "29999984";
    }
    showSeparator();

    // 显示配置说明
    std::cout << "\n常用配置说明:" << std::endl;
    showSeparator();
    std::cout << "  online-mode    - 正版验证: true=开启(正版服务器), false=关闭(离线服务器)" << std::endl;
    std::cout << "  difficulty     - 难度: peaceful(和平), easy(简单), normal(普通), hard(困难)" << std::endl;
    std::cout << "  gamemode       - 游戏模式: survival(生存), creative(创造), adventure(冒险), spectator(旁观)" << std::endl;
    std::cout << "  server-port    - 服务器端口 (默认: 25565)" << std::endl;
    std::cout << "  max-players    - 最大玩家数 (默认: 20)" << std::endl;
    std::cout << "  motd           - 服务器欢迎语 (显示在服务器列表)" << std::endl;
    std::cout << "  view-distance  - 视距 (默认: 10, 越大越吃性能)" << std::endl;
    std::cout << "  allow-nether   - 是否允许进入下界 (true/false)" << std::endl;
    std::cout << "  allow-flight   - 是否允许飞行 (true/false)" << std::endl;
    std::cout << "  enable-command-block - 是否启用命令方块 (true/false)" << std::endl;
    std::cout << "  spawn-protection - 出生点保护范围 (0=关闭)" << std::endl;
    std::cout << "  level-name     - 世界文件夹名称 (默认: world)" << std::endl;
    showSeparator();

    std::string key, value;
    while (true) {
        std::cout << "\n请输入配置key (输入quit退出): ";
        std::cin >> key;

        if (key == "quit") {
            break;
        }

        std::string hint;
        if (key == "online-mode") {
            hint = "正版验证: true=开启(正版服务器), false=关闭(离线服务器)";
        }
        else if (key == "difficulty") {
            hint = "难度: peaceful(和平), easy(简单), normal(普通), hard(困难)";
        }
        else if (key == "gamemode") {
            hint = "游戏模式: survival(生存), creative(创造), adventure(冒险), spectator(旁观)";
        }
        else if (key == "server-port") {
            hint = "服务器端口, 默认25565";
        }
        else if (key == "max-players") {
            hint = "最大玩家数, 默认20";
        }
        else if (key == "motd") {
            hint = "服务器欢迎语, 显示在服务器列表";
        }
        else if (key == "view-distance") {
            hint = "视距, 默认10, 越大越吃性能";
        }
        else if (key == "spawn-protection") {
            hint = "出生点保护范围, 0=关闭, 默认16";
        }
        else if (key == "allow-nether") {
            hint = "允许下界: true/false";
        }
        else if (key == "allow-flight") {
            hint = "允许飞行: true/false";
        }
        else if (key == "enable-command-block") {
            hint = "允许命令方块: true/false";
        }
        else if (key == "level-name") {
            hint = "世界文件夹名称, 默认world";
        }
        else if (key == "max-world-size") {
            hint = "最大世界大小, 默认29999984";
        }
        else if (key == "generate-structures") {
            hint = "生成结构: true/false";
        }
        else if (key == "enforce-whitelist") {
            hint = "强制白名单: true/false";
        }
        else if (key == "resource-pack") {
            hint = "资源包下载链接";
        }
        else {
            hint = "自定义配置项";
        }

        std::cout << "  [提示] " << hint << std::endl;
        std::cout << "请输入配置value (留空则删除该配置): ";
        std::cin.ignore();
        std::getline(std::cin, value);

        if (value.empty()) {
            currentProps.erase(key);
            showSuccess("已删除配置: " + key);
        }
        else {
            currentProps[key] = value;
            showSuccess("配置已更新: " + key + " = " + value);
        }

        std::ofstream file(propertiesPath);
        if (file.is_open()) {
            file << "#Minecraft server.properties" << std::endl;
            file << "#" << std::endl;
            file << "#" << std::endl;
            for (std::map<std::string, std::string>::const_iterator it = currentProps.begin();
                it != currentProps.end(); ++it) {
                file << it->first << "=" << it->second << std::endl;
            }
            file.close();
            showSuccess("配置已保存到 server.properties");
        }
        else {
            showError("无法保存配置文件");
        }
    }

    showInfo("配置完成");
}

void configureAutoInstall() {
    clearScreen();
    showTitle("安装服务器自动配置");

    std::cout << "\n当前自动配置:" << std::endl;
    showSeparator();
    if (autoConfigs.empty()) {
        std::cout << "  无自动配置" << std::endl;
    }
    else {
        for (std::map<std::string, std::string>::const_iterator it = autoConfigs.begin(); it != autoConfigs.end(); ++it) {
            std::cout << "  " << it->first << " = " << it->second << std::endl;
        }
    }
    showSeparator();
    showInfo("这些配置将在安装新服务器时自动应用");

    std::string key, value;
    while (true) {
        std::cout << "\n请输入自动配置key (输入quit退出): ";
        std::cin >> key;

        if (key == "quit") {
            break;
        }

        std::cout << "请输入自动配置value (留空则删除该配置): ";
        std::cin.ignore();
        std::getline(std::cin, value);

        if (value.empty()) {
            autoConfigs.erase(key);
            showSuccess("已删除自动配置: " + key);
        }
        else {
            if (!validateServerProperty(key, value)) {
                showError("该配置可能不被服务器支持，是否继续? (y/n): ");
                char confirm;
                std::cin >> confirm;
                if (confirm != 'y' && confirm != 'Y') {
                    showInfo("已取消该配置");
                    continue;
                }
            }
            autoConfigs[key] = value;
            showSuccess("自动配置已更新: " + key + " = " + value);
        }

        saveAutoConfigs();
    }

    showInfo("自动配置完成");
}

void configureStartupOptions() {
    clearScreen();
    showTitle("配置启动选项");

    if (servers.empty()) {
        showError("没有已安装的服务器");
        return;
    }

    listInstalledServers();

    std::string version;
    std::cout << "请输入要配置启动选项的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        return;
    }

    clearScreen();
    showTitle("配置启动选项: " + version);

    std::cout << "\n当前启动配置:" << std::endl;
    showSeparator();
    std::cout << "  最小内存: " << (it->second.minMemory.empty() ? "未设置(默认)" : it->second.minMemory) << std::endl;
    std::cout << "  最大内存: " << (it->second.maxMemory.empty() ? "未设置(默认)" : it->second.maxMemory) << std::endl;
    std::cout << "  性能优化: " << (it->second.useOptimization ? "启用" : "禁用") << std::endl;
    std::cout << "  额外JVM参数: " << (it->second.additionalJvmArgs.empty() ? "无" : it->second.additionalJvmArgs) << std::endl;
    std::cout << "  nogui模式: " << (it->second.useNogui ? "启用" : "禁用") << std::endl;
    showSeparator();

    std::cout << "\n启动命令格式: java [内存参数] [JVM参数] -jar server.jar [nogui]" << std::endl;
    std::cout << std::endl;

    std::string input;

    std::cout << "最小内存 (例如: 1024M, 2G) [当前: " << (it->second.minMemory.empty() ? "未设置" : it->second.minMemory) << "]: ";
    std::cin.ignore();
    std::getline(std::cin, input);
    if (!input.empty()) {
        it->second.minMemory = input;
        showSuccess("最小内存已更新: " + input);
    }

    std::cout << "最大内存 (例如: 2048M, 4G) [当前: " << (it->second.maxMemory.empty() ? "未设置" : it->second.maxMemory) << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        it->second.maxMemory = input;
        showSuccess("最大内存已更新: " + input);
    }

    std::cout << "是否启用性能优化? (true/false) [当前: " << (it->second.useOptimization ? "true" : "false") << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        it->second.useOptimization = (input == "true" || input == "1" || input == "yes");
        showSuccess("性能优化设置已更新: " + std::string(it->second.useOptimization ? "启用" : "禁用"));
    }

    std::cout << "额外JVM参数 (例如: -XX:+UseG1GC) [当前: " << (it->second.additionalJvmArgs.empty() ? "无" : it->second.additionalJvmArgs) << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        it->second.additionalJvmArgs = input;
        showSuccess("额外JVM参数已更新: " + input);
    }

    std::cout << "是否启用nogui模式? (true/false) [当前: " << (it->second.useNogui ? "true" : "false") << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        it->second.useNogui = (input == "true" || input == "1" || input == "yes");
        showSuccess("nogui模式设置已更新: " + std::string(it->second.useNogui ? "启用" : "禁用"));
    }

    saveServerConfigs();

    std::cout << std::endl;
    std::cout << "生成的启动命令:" << std::endl;
    showSeparator();
    std::string cmd = "java";
    if (!it->second.minMemory.empty()) cmd += " -Xms" + it->second.minMemory;
    if (!it->second.maxMemory.empty()) cmd += " -Xmx" + it->second.maxMemory;
    if (!it->second.additionalJvmArgs.empty()) cmd += " " + it->second.additionalJvmArgs;
    if (it->second.useOptimization) {
        cmd += " -XX:+UseG1GC -XX:+ParallelRefProcEnabled -XX:MaxGCPauseMillis=200";
    }
    cmd += " -jar server.jar";
    if (it->second.useNogui) cmd += " nogui";
    std::cout << "  " << cmd << std::endl;
    showSeparator();

    showSuccess("启动选项已更新！");
}

void installModLoader() {
    clearScreen();
    showTitle("安装模组加载器");

    if (servers.empty()) {
        showError("没有已安装的服务器，请先安装服务器");
        return;
    }

    listInstalledServers();

    std::string version;
    std::cout << "请输入要安装模组加载器的服务器版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        return;
    }

    std::string serverDir = BASE_DIR + "/" + version;
    std::string serverJar = serverDir + "/server.jar";

    if (!fileExists(serverJar)) {
        showError("服务器jar文件不存在: " + serverJar);
        return;
    }

    // 获取支持的加载器列表
    std::vector<ModLoaderInfo> loaders = getSupportedLoaders();

    std::cout << "\n支持的模组加载器:" << std::endl;
    showSeparator();

    int index = 1;
    for (size_t i = 0; i < loaders.size(); i++) {
        if (loaders[i].supported) {
            setConsoleColor(COLOR_GREEN);
            std::cout << "  " << index << ". " << loaders[i].name << " (推荐)" << std::endl;
            resetConsoleColor();
            index++;
        }
    }
    showSeparator();

    int choice;
    std::cout << "请选择模组加载器 (输入编号): ";
    std::cin >> choice;

    if (choice < 1 || choice > static_cast<int>(loaders.size())) {
        showError("无效选择");
        return;
    }

    ModLoaderInfo selectedLoader = loaders[choice - 1];

    if (!selectedLoader.supported) {
        showInfo(selectedLoader.name + " 暂未支持，敬请期待");
        return;
    }

    // 检查是否已安装该加载器
    std::string loaderJarPath = serverDir + "/" + selectedLoader.generatedJar;
    if (fileExists(loaderJarPath)) {
        char confirm;
        std::cout << "检测到已安装 " << selectedLoader.name << "，是否重新安装? (y/n): ";
        std::cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            showInfo("取消安装");
            return;
        }
    }

    showInfo("正在安装 " + selectedLoader.name + " 模组加载器...");

    // 下载安装器
    std::string installerPath = serverDir + "/" + selectedLoader.installerJar;

    showInfo("下载安装器...");
    showInfo("下载链接: " + selectedLoader.installerUrl);
    if (!downloadFile(selectedLoader.installerUrl, installerPath)) {
        showError("下载安装器失败");
        return;
    }

    // 获取加载器版本
    showInfo("正在获取最新加载器版本...");
    std::string loaderVersion = getLoaderVersion(selectedLoader.id);
    showInfo("加载器版本: " + loaderVersion);

    // 构建安装命令
    std::string installCmd = buildInstallCommand(selectedLoader, version, loaderVersion, installerPath);

    showInfo("执行安装命令: " + installCmd);
    showInfo("请耐心等待，安装可能需要几分钟...");

    // 切换到服务器目录执行安装
    std::string cmd = "cd /d \"" + serverDir + "\" && " + installCmd;
    showInfo("完整命令: " + cmd);
    int result = system(cmd.c_str());

    if (result != 0) {
        showError(selectedLoader.name + " 安装失败，请检查网络连接");
        return;
    }

    // 检查是否生成了启动jar
    bool foundJar = false;
    std::string foundPath = "";

    // 1. 检查服务器目录
    if (fileExists(loaderJarPath)) {
        foundJar = true;
        foundPath = loaderJarPath;
    }

    // 2. 检查当前工作目录
    if (!foundJar) {
        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        std::string currentJar = std::string(currentDir) + "\\" + selectedLoader.generatedJar;
        if (fileExists(currentJar)) {
            foundJar = true;
            foundPath = currentJar;
            std::string targetPath = serverDir + "\\" + selectedLoader.generatedJar;
            if (MoveFileA(currentJar.c_str(), targetPath.c_str())) {
                showInfo("已将 " + selectedLoader.generatedJar + " 移动到服务器目录");
                foundPath = targetPath;
            }
        }
    }

    // 3. 通配符搜索
    if (!foundJar) {
        WIN32_FIND_DATAA findData;
        std::string searchPath = serverDir + "\\*" + selectedLoader.id + "*.jar";
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::string fileName = findData.cFileName;
                if (fileName.find(selectedLoader.id) != std::string::npos &&
                    fileName.find("installer") == std::string::npos) {
                    foundJar = true;
                    foundPath = serverDir + "\\" + fileName;
                    showInfo("找到文件: " + fileName);
                    break;
                }
            } while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }
    }

    if (foundJar) {
        showSuccess(selectedLoader.name + " 模组加载器安装成功！");
        showInfo("生成的启动文件: " + foundPath);
        showInfo("启动服务器将使用 " + selectedLoader.generatedJar);

        // 更新配置中的jarPath
        std::string relativePath = serverDir + "/" + selectedLoader.generatedJar;
        for (std::map<std::string, ServerVersion>::iterator it = servers.begin(); it != servers.end(); ++it) {
            if (it->first == version) {
                it->second.jarPath = relativePath;
                saveServerConfigs();
                showInfo("已更新配置，将使用 " + selectedLoader.name + " 启动");
                break;
            }
        }

        // 创建模组文件夹
        std::string modsDir = serverDir + "/mods";
        if (!directoryExists(modsDir)) {
            createDirectory(modsDir);
            showInfo("已创建 mods 文件夹，请将模组放入其中");
        }

        // 如果是Fabric，创建fabric文件夹
        if (selectedLoader.id == "fabric") {
            std::string fabricDir = serverDir + "/fabric";
            if (!directoryExists(fabricDir)) {
                createDirectory(fabricDir);
                showInfo("已创建 fabric 文件夹");
            }
        }
    }
    else {
        showError("未找到 " + selectedLoader.generatedJar);
        showInfo("请检查服务器目录下是否有相关文件");
        showInfo("安装日志显示安装成功，请手动查找启动文件");
    }

    // 清理安装器文件
    char confirm;
    std::cout << "\n是否删除安装器文件? (y/n): ";
    std::cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        DeleteFileA(installerPath.c_str());
        showSuccess("已删除安装器文件");
    }

    showInfo("安装完成！");
}

void installMod() {
    clearScreen();
    showTitle("安装模组");

    if (servers.empty()) {
        showError("没有已安装的服务器，请先安装服务器");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    // 显示服务器列表
    listInstalledServers();

    std::string version;
    std::cout << "请输入要安装模组的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    std::string serverDir = BASE_DIR + "/" + version;
    std::string modsDir = serverDir + "/mods";

    // 检查是否已安装Fabric
    std::string fabricJar = serverDir + "/fabric-server-launch.jar";
    if (!fileExists(fabricJar)) {
        showError("该版本未安装Fabric模组加载器！");
        showInfo("请先使用功能7安装Fabric模组加载器");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    // 创建mods文件夹
    if (!directoryExists(modsDir)) {
        createDirectory(modsDir);
        showInfo("已创建 mods 文件夹");
    }

    // 打开文件选择对话框
    std::string filter = "Java Archive (*.jar)\0*.jar\0All Files (*.*)\0*.*\0";
    std::string selectedFile = openFileDialog("选择模组文件 (.jar)", filter);

    if (selectedFile.empty()) {
        showInfo("未选择文件");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    // 获取文件名
    size_t lastSlash = selectedFile.find_last_of("/\\");
    std::string fileName = (lastSlash != std::string::npos) ?
        selectedFile.substr(lastSlash + 1) :
        selectedFile;

    std::string targetPath = modsDir + "/" + fileName;

    // 检查是否已存在
    if (fileExists(targetPath)) {
        char confirm;
        std::cout << "文件 " << fileName << " 已存在，是否覆盖? (y/n): ";
        std::cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            showInfo("取消安装");
            std::cout << "\n按任意键继续...";
            std::cin.ignore();
            std::cin.get();
            return;
        }
    }

    // 复制文件
    showInfo("正在复制模组文件...");
    if (CopyFileA(selectedFile.c_str(), targetPath.c_str(), FALSE)) {
        showSuccess("模组安装成功！");
        showInfo("文件: " + fileName);
        showInfo("位置: " + targetPath);
    }
    else {
        DWORD error = GetLastError();
        showError("复制文件失败，错误代码: " + std::to_string(error));
    }

    std::cout << "\n按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

void scanPlugins() {
    g_plugins.clear();

    std::string pluginDir = "plugin";
    if (!directoryExists(pluginDir)) {
        createDirectory(pluginDir);
        return;
    }

    WIN32_FIND_DATAA findData;
    std::string searchPath = pluginDir + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
        }

        std::string folderName = findData.cFileName;
        std::string configPath = pluginDir + "\\" + folderName + "\\config.json";

        if (!fileExists(configPath)) {
            continue;
        }

        std::ifstream file(configPath);
        if (!file.is_open()) {
            continue;
        }

        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line;
        }
        file.close();

        try {
            json config = json::parse(content);

            PluginInfo plugin;
            plugin.folderName = folderName;

            if (config.contains("name") && config["name"].is_string()) {
                plugin.name = config["name"];
            }
            else {
                plugin.name = folderName;
            }

            if (config.contains("level") && config["level"].is_number()) {
                plugin.level = config["level"];
            }
            else {
                plugin.level = 999;
            }

            if (config.contains("plugin") && config["plugin"].contains("name") && config["plugin"]["name"].is_string()) {
                plugin.exeName = config["plugin"]["name"];
            }
            else {
                plugin.exeName = folderName + ".exe";
            }

            plugin.exePath = pluginDir + "\\" + folderName + "\\" + plugin.exeName;

            if (fileExists(plugin.exePath)) {
                g_plugins.push_back(plugin);
            }

        }
        catch (const json::parse_error& e) {
            showDebug("解析插件配置失败: " + folderName);
        }
        catch (const std::exception& e) {
            showDebug("读取插件配置失败: " + folderName);
        }

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);

    for (size_t i = 0; i < g_plugins.size(); i++) {
        for (size_t j = i + 1; j < g_plugins.size(); j++) {
            if (g_plugins[j].level < g_plugins[i].level) {
                PluginInfo temp = g_plugins[i];
                g_plugins[i] = g_plugins[j];
                g_plugins[j] = temp;
            }
        }
    }
}

void showPluginMenu() {
    clearScreen();
    showTitle("插件");

    if (g_plugins.empty()) {
        showInfo("没有找到可用插件");
        showInfo("请将插件放在 plugin/<插件名>/ 目录下");
        showInfo("并确保有 config.json 和 <插件名>.exe");
        showInfo("插件安装和卸载请用户自行管理");
        std::cout << std::endl;
        return;
    }

    std::cout << "可用插件:" << std::endl;
    showSeparator();
    for (size_t i = 0; i < g_plugins.size(); i++) {
        setConsoleColor(COLOR_GREEN);
        std::cout << "  " << (i + 1) << ". ";
        resetConsoleColor();
        std::cout << g_plugins[i].name;
        std::cout << " (优先级: " << g_plugins[i].level << ")";
        std::cout << std::endl;
    }
    showSeparator();
    std::cout << std::endl;
}

void runPlugin() {
    clearScreen();
    showTitle("运行插件");

    if (g_plugins.empty()) {
        showError("没有可用插件");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    if (servers.empty()) {
        showError("没有已安装的服务器，请先安装服务器");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    showPluginMenu();

    int pluginChoice;
    std::cout << "请选择插件 (输入编号): ";
    std::cin >> pluginChoice;

    if (std::cin.fail() || pluginChoice < 1 || pluginChoice > static_cast<int>(g_plugins.size())) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        showError("输入无效");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    PluginInfo& selectedPlugin = g_plugins[pluginChoice - 1];

    std::cout << std::endl;
    listInstalledServers();

    std::string version;
    std::cout << "请输入要操作的版本: ";
    std::cin >> version;

    std::map<std::string, ServerVersion>::iterator it = servers.find(version);
    if (it == servers.end()) {
        showError("版本 " + version + " 不存在");
        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::string rootDir = std::string(currentDir);

    std::string cmd = "cd /d \"" + rootDir + "\" && start \"\" \"" + selectedPlugin.exePath + "\" -" + version;

    showInfo("正在启动插件: " + selectedPlugin.name);
    showInfo("工作目录: " + rootDir);
    showInfo("命令: " + cmd);

    system(cmd.c_str());

    showSuccess("插件已启动！");
    showInfo("插件运行于独立窗口，关闭插件窗口即可停止");

    std::cout << "\n按任意键继续...";
    std::cin.ignore();
    std::cin.get();
}

void listInstalledServers() {
    if (servers.empty()) {
        std::cout << "  没有已安装的服务器" << std::endl;
        return;
    }

    std::cout << "\n已安装的服务器:" << std::endl;
    showSeparator();
    for (std::map<std::string, ServerVersion>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        const std::string& version = it->first;
        const ServerVersion& server = it->second;

        std::string fabricJar = BASE_DIR + "/" + version + "/fabric-server-launch.jar";
        bool hasFabric = fileExists(fabricJar);

        if (hasFabric) {
            setConsoleColor(COLOR_GREEN);
            std::cout << "  - " << version << " [Fabric]";
            resetConsoleColor();
        }
        else {
            setConsoleColor(COLOR_YELLOW);
            std::cout << "  - " << version;
            resetConsoleColor();
            std::cout << " (不可安装模组)";
        }

        std::vector<ModLoaderInfo> loaders = getSupportedLoaders();
        bool hasLoader = false;
        for (size_t i = 0; i < loaders.size(); i++) {
            if (!loaders[i].supported) continue;
            std::string loaderJar = BASE_DIR + "/" + version + "/" + loaders[i].generatedJar;
            if (fileExists(loaderJar)) {
                hasLoader = true;
                break;
            }
        }

        std::vector<std::string> tags;
        if (!server.minMemory.empty() || !server.maxMemory.empty()) {
            std::string mem;
            if (!server.minMemory.empty()) mem += server.minMemory;
            if (!server.minMemory.empty() && !server.maxMemory.empty()) mem += "/";
            if (!server.maxMemory.empty()) mem += server.maxMemory;
            tags.push_back("内存: " + mem);
        }
        if (server.useOptimization) tags.push_back("优化启用");
        if (server.useNogui) tags.push_back("nogui模式");
        if (!server.additionalJvmArgs.empty()) tags.push_back("自定义JVM参数");

        if (!tags.empty()) {
            std::cout << " [";
            for (size_t i = 0; i < tags.size(); i++) {
                std::cout << tags[i];
                if (i < tags.size() - 1) std::cout << ", ";
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    showSeparator();
    std::cout << std::endl;
}

void showMenu() {
    clearScreen();
    showTitle("MC服务器管理程序");

    setConsoleColor(COLOR_CYAN);
    std::cout << "  1. 安装MC服务器" << std::endl;
    std::cout << "  2. 启动MC服务器" << std::endl;
    std::cout << "  3. 删除MC服务器" << std::endl;
    std::cout << "  4. 配置版本的配置文件" << std::endl;
    std::cout << "  5. 安装服务器自动配置" << std::endl;
    std::cout << "  6. 启动选项配置" << std::endl;
    std::cout << "  7. 安装模组加载器" << std::endl;
    std::cout << "  8. 安装模组" << std::endl;
    std::cout << "  9. 列出已安装的服务器" << std::endl;
    std::cout << "  10. 查看可用版本" << std::endl;
    std::cout << "  11. 插件" << std::endl;
    std::cout << "  12. 关于" << std::endl;
    std::cout << "  0. 退出" << std::endl;
    resetConsoleColor();
    showSeparator();
    std::cout << "请选择操作: ";
}
void about() {
    clearScreen();
    showTitle("MC服务器管理程序");

    std::cout << "MC服务器管理程序 v1.4" << std::endl;
    std::cout << "作者: skc" << std::endl;
    std::cout << "Github: https://github.com/skc-cmd/Minecraft-Server-Launcher"<<std::endl;
    std::cout << "CSDN: https://blog.csdn.net/weixin_42156143?spm=1000.2115.3001.5343" << std::endl;
    std::cout << "本程序使用GPL V3开源许可证" << std::endl;
}
int main() {
    // 创建基础目录
    if (!directoryExists(BASE_DIR)) {
        createDirectory(BASE_DIR);
    }

    // 加载配置
    loadAutoConfigs();
    loadServerConfigs();

    // ===== 检查EULA同意状态 =====
    if (!checkEulaAgreed()) {
        showDisclaimer();
    }

    // ===== 启动时检测Java =====
    showTitle("MC服务器管理程序");
    g_javaPath = detectJavaPath();
    if (g_javaPath.empty()) {
        showError("未找到Java!");
        showSeparator();
        showInfo("请安装Java以运行Minecraft服务器");
        showInfo("推荐下载以下版本:");
        setConsoleColor(COLOR_GREEN);
        std::cout << "  Java 26 (最新): https://download.oracle.com/java/26/latest/jdk-26_windows-x64_bin.msi" << std::endl;
        resetConsoleColor();
        setConsoleColor(COLOR_YELLOW);
        std::cout << "  Java 25: https://download.oracle.com/java/25/latest/jdk-25_windows-x64_bin.msi" << std::endl;
        std::cout << "  Java 21 (LTS推荐): https://download.oracle.com/java/21/latest/jdk-21_windows-x64_bin.msi" << std::endl;
        resetConsoleColor();
        showSeparator();
        showInfo("安装完成后，请重新启动本程序");
        std::cout << "\n按任意键退出...";
        std::cin.ignore();
        std::cin.get();
        return 1;
    }

    // ===== 扫描插件 =====
    scanPlugins();

    int choice;
    while (true) {
        showMenu();
        std::cin >> choice;

        switch (choice) {
        case 0:
            clearScreen();
            showTitle("程序退出");
            showInfo("感谢使用MC服务器管理程序");
            return 0;
        case 1:
            installServer();
            break;
        case 2:
            startServer();
            break;
        case 3:
            deleteServer();
            break;
        case 4:
            configureServer();
            break;
        case 5:
            configureAutoInstall();
            break;
        case 6:
            configureStartupOptions();
            break;
        case 7:
            installModLoader();
            break;
        case 8:
            installMod();
            break;
        case 9:
            clearScreen();
            showTitle("已安装的服务器");
            listInstalledServers();
            break;
        case 10:
            listAvailableVersions();
            break;
        case 11:
            runPlugin();
            break;
        case 12:
            about();
            break;
        default:
            showError("无效选择，请重试");
            break;
        }

        std::cout << "\n按任意键继续...";
        std::cin.ignore();
        //std::cin.get();
    }

    return 0;
}