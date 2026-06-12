/**
 * @file config_manager.h
 * @brief 定义网关启动配置数据结构和配置加载接口。
 *
 * 该文件描述网关运行所需的静态配置项，如监听地址、端口、数据库路径、日志级别和功能开关。
 * ConfigManager 负责从文件或环境变量加载这些配置，供 GatewayServer 初始化使用。
 *
 * 与其他文件的联系：
 * - gateway/gateway_server.h 的 GatewayServerOptions 通常由 GatewayConfig 转换而来。
 * - storage/storage.h 的 initialize 可使用 database_path 指定数据库文件位置。
 */
#pragma once

#include <string>

namespace ai_gateway_core {

/**
 * @brief 网关全局配置项。
 *
 * 字段说明：
 * - host：HTTP 服务监听地址。
 * - port：HTTP 服务监听端口。
 * - database_path：数据库文件路径，供存储层初始化使用。
 * - log_level：日志输出级别，如 DEBUG、INFO、WARN、ERROR。
 * - default_gateway_api_key：网关默认 API Key，用于内部组件间调用或初始认证。
 * - enable_mock_provider：是否启用模拟供应商，便于开发测试。
 * - enable_admin_api：是否开启管理接口。
 * - enable_playground：是否开启在线调试或体验页面。
 */
struct GatewayConfig {
    std::string host = "0.0.0.0";
    int port = 8080;
    std::string database_path = "ai_gateway.db";
    std::string log_level = "INFO";
    std::string default_gateway_api_key;
    bool enable_mock_provider = false;
    bool enable_admin_api = true;
    bool enable_playground = true;
};

/**
 * @brief 网关配置加载接口。
 */
class ConfigManager {
public:
    virtual ~ConfigManager() = default;

    /**
     * @brief 加载网关配置。
     * @return 成功时返回加载后的 GatewayConfig；失败时返回文件不存在或解析错误。
     */
    virtual GatewayConfig load() = 0;
};

/**
 * @brief 返回一份固定配置的简单实现。
 *
 * 设计意图：
 * - 适合测试、教学和手工构造配置场景。
 * - 不依赖文件或环境变量，直接返回构造时给定的配置。
 */
class StaticConfigManager : public ConfigManager {
public:
    /**
     * @brief 创建固定配置管理器。
     * @param config 后续每次 load 都返回这份配置。
     */
    explicit StaticConfigManager(GatewayConfig config);

    /// @brief 返回构造时保存的固定配置。
    GatewayConfig load() override;

private:
    /// @brief 固定配置副本。
    GatewayConfig config_;
};

/**
 * @brief 从环境变量读取配置的实现。
 *
 * 设计意图：
 * - 适合服务部署和命令行启动场景。
 * - 让网关可以不改代码就切换端口、路径和功能开关。
 */
class EnvironmentConfigManager : public ConfigManager {
public:
    EnvironmentConfigManager() = default;

    /// @brief 从环境变量装配并返回 GatewayConfig。
    GatewayConfig load() override;

private:
    /// @brief 读取字符串环境变量，不存在时返回默认值。
    std::string readEnv(const std::string& key, const std::string& default_value) const;
    /// @brief 读取整型环境变量，不存在或非法时返回默认值。
    int readEnvInt(const std::string& key, int default_value) const;
    /// @brief 读取布尔环境变量，不存在或非法时返回默认值。
    bool readEnvBool(const std::string& key, bool default_value) const;
};

}
