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
#include <string>
#include <map>

namespace ai_gateway_core {

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

}
