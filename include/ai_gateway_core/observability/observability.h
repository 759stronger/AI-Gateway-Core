/**
 * @file observability.h
 * @brief 定义链路追踪和请求事件记录的接口。
 *
 * 该文件为网关提供可观测性能力，包括创建追踪标识、记录请求开始/结束、路由决策
 * 和降级切换等关键事件。这些事件可用于日志、指标、分布式追踪系统或调试排障。
 *
 * 与其他文件的联系：
 * - core/types.h 的 GatewayContext 通过 trace_id 与可观测性事件关联。
 * - routing/model_router.h 在路由选择和降级时可调用 recordRouteDecision 和 recordFallback。
 * - gateway/gateway_server.h 的实现通常在请求入口和出口记录生命周期事件。
 */
#pragma once

#include "ai_gateway_core/core/types.h"
#include <string>
#include <map>

namespace ai_gateway_core {

/**
 * @brief 一次链路追踪的范围标识和扩展属性。
 *
 * 字段说明：
 * - trace_id：链路唯一标识，贯穿整个请求生命周期。
 * - attributes：附加键值属性，可用于记录用户、模型、来源等业务标签。
 */
struct TraceScope {
    std::string trace_id;
    std::map<std::string, std::string> attributes;
};

/**
 * @brief 可观测性接口。
 */
class Observability {
public:
    virtual ~Observability() = default;

    /**
     * @brief 创建新的链路追踪标识。
     * @return 新生成的唯一 trace_id 字符串。
     */
    virtual std::string createTraceId() = 0;

    /**
     * @brief 记录一次请求的受理开始事件。
     * @param context 请求上下文，包含 trace_id、user_id、api_key_id、public_model_name 等。
     */
    virtual void recordRequestStart(const GatewayContext& context) = 0;

    /**
     * @brief 记录上游账号路由决策事件。
     * @param context 请求上下文，标识被路由的请求。
     * @param upstream_account_id 被选中的上游账号唯一标识。
     */
    virtual void recordRouteDecision(const GatewayContext& context, const std::string& upstream_account_id) = 0;

    /**
     * @brief 记录上游账号降级切换事件。
     * @param context 请求上下文。
     * @param from_account_id 降级前的上游账号标识。
     * @param to_account_id 降级后的备选上游账号标识。
     */
    virtual void recordFallback(const GatewayContext& context, const std::string& from_account_id, const std::string& to_account_id) = 0;

    /**
     * @brief 记录一次请求的结束事件。
     * @param context 请求上下文。
     * @param latency_ms 从请求开始到结束的端到端延迟毫秒数。
     */
    virtual void recordRequestEnd(const GatewayContext& context, int latency_ms) = 0;
};

}
