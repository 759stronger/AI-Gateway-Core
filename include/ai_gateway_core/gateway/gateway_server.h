/**
 * @file gateway_server.h
 * @brief 定义网关服务进程的启动、停止和运行状态接口。
 *
 * 该文件描述 AI Gateway 对外提供 HTTP/API 服务时的生命周期控制。
 * 实现类通常会组合认证、路由、限流、计费、供应商调用和协议适配等模块。
 *
 * 与其他文件的联系：
 * - config/config_manager.h 的 GatewayConfig 可转换为 GatewayServerOptions。
 * - adapters/openai_adapter.h 通常由服务入口用于解析请求和构建响应。
 * - auth、routing、providers、usage、billing 等模块会被服务实现串联成完整请求链路。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include <map>
#include <memory>
#include <string>

namespace ai_gateway_core {

class AuthManager;
class BillingPolicy;
class ModelRouter;
class Observability;
class OpenAIAdapter;
class ProviderManager;
class RateLimiter;
class UsageRecorder;
class UserManager;
class UpstreamPool;

/**
 * @brief 网关服务启动配置。
 *
 * 字段说明：
 * - host：服务监听地址。
 * - port：服务监听端口。
 * - enable_admin_api：是否启用管理接口。
 * - enable_playground：是否启用调试或体验页面。
 */
struct GatewayServerOptions {
    std::string host = "0.0.0.0";
    int port = 8080;
    bool enable_admin_api = true;
    bool enable_playground = true;
};

/**
 * @brief 网关服务运行所需的核心依赖集合。
 *
 * 字段说明：
 * - openai_adapter：协议解析与响应构建。
 * - auth_manager：下游 API Key 鉴权。
 * - user_manager：用户状态和额度校验。
 * - rate_limiter：请求限流。
 * - model_router：模型映射与上游选择。
 * - provider_manager：Provider 注册表。
 * - usage_recorder：请求用量记录。
 * - billing_policy：请求计费策略。
 * - observability：链路追踪和事件记录。
 * - upstream_pool：上游账号池，供并发控制和状态回写使用。
 */
struct GatewayDependencies {
    std::shared_ptr<OpenAIAdapter> openai_adapter;
    std::shared_ptr<AuthManager> auth_manager;
    std::shared_ptr<UserManager> user_manager;
    std::shared_ptr<RateLimiter> rate_limiter;
    std::shared_ptr<ModelRouter> model_router;
    std::shared_ptr<ProviderManager> provider_manager;
    std::shared_ptr<UsageRecorder> usage_recorder;
    std::shared_ptr<BillingPolicy> billing_policy;
    std::shared_ptr<Observability> observability;
    std::shared_ptr<UpstreamPool> upstream_pool;
};

/**
 * @brief 网关服务生命周期接口。
 */
class GatewayServer {
public:
    virtual ~GatewayServer() = default;

    /**
     * @brief 使用指定选项初始化服务。
     * @param options 服务监听地址、端口和功能开关配置。
     * @return 成功时表示服务已完成初始化；失败时返回配置或依赖初始化错误。
     */
    virtual Status initialize(const GatewayServerOptions& options) = 0;

    /**
     * @brief 启动网关服务并开始接受请求。
     * @return 成功时表示服务已进入运行状态；失败时返回绑定端口或运行时错误。
     */
    virtual Status start() = 0;

    /**
     * @brief 停止网关服务并释放运行资源。
     */
    virtual void stop() = 0;

    /**
     * @brief 查询服务是否处于运行状态。
     * @return 运行中返回 true，否则返回 false。
     */
    virtual bool isRunning() const = 0;
};

/**
 * @brief GatewayServer 的默认实现声明。
 *
 * 设计意图：
 * - 统一承载“初始化 -> 启动 -> 请求处理 -> 停止”这条服务生命周期。
 * - 通过依赖注入把各个模块串成完整请求链路。
 */
class DefaultGatewayServer : public GatewayServer {
public:
    /**
     * @brief 创建默认网关服务。
     * @param dependencies 网关运行所需的依赖集合。
     */
    explicit DefaultGatewayServer(GatewayDependencies dependencies);

    /// @brief 使用监听配置和开关完成服务初始化。
    Status initialize(const GatewayServerOptions& options) override;
    /// @brief 启动服务。
    Status start() override;
    /// @brief 停止服务。
    void stop() override;
    /// @brief 查询当前服务是否处于运行状态。
    bool isRunning() const override;

    /**
     * @brief 处理一次聊天补全请求。
     * @param authorization_header 客户端 Authorization 请求头。
     * @param request_body 原始聊天补全请求体。
     * @return 成功时返回 OpenAI 兼容响应字符串；失败时返回协议或业务错误。
     */
    Result<std::string> handleChatCompletion(const std::string& authorization_header,
                                             const std::string& request_body);

private:
    /// @brief 检查启动前依赖是否齐全且可用。
    Status validateDependencies() const;

    /// @brief 网关依赖集合。
    GatewayDependencies dependencies_;
    /// @brief 当前服务选项。
    GatewayServerOptions options_;
    /// @brief 服务是否已经完成 initialize。
    bool initialized_ = false;
    /// @brief 服务当前是否处于运行状态。
    bool running_ = false;
};

}
