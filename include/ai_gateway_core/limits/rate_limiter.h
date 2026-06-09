/**
 * @file rate_limiter.h
 * @brief 定义用户、API Key、模型和上游账号维度的限流接口。
 *
 * 该文件用于在请求执行前判断是否超过各类访问限制，并在选择上游后消费对应额度。
 * 限流可以覆盖用户、密钥、模型能力和具体上游账号，保护网关和供应商资源。
 *
 * 与其他文件的联系：
 * - core/types.h 提供 GatewayContext 和 Capability 作为限流维度。
 * - upstream/upstream_account.h 中的 rpm_limit、tpm_limit 可作为上游限流配置来源。
 * - gateway/gateway_server.h 的实现通常在路由前后调用这些检查。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include <string>

namespace ai_gateway_core {

/**
 * @brief 一次限流检查的决策结果。
 *
 * 字段说明：
 * - allowed：是否允许当前请求继续执行。
 * - retry_after_seconds：被限流时建议客户端等待的秒数。
 * - reason：允许或拒绝的说明，便于日志和错误响应展示。
 */
struct LimitDecision {
    bool allowed = false;
    int retry_after_seconds = 0;
    std::string reason;
};

/**
 * @brief 限流器接口。
 */
class RateLimiter {
public:
    virtual ~RateLimiter() = default;

    /**
     * @brief 检查用户维度限流或配额。
     * @param context 请求上下文，主要使用 user_id、public_model_name、capability 和 stream。
     * @return 成功时返回是否允许请求；失败时返回限流后端或配置错误。
     */
    virtual Result<LimitDecision> checkUserLimit(const GatewayContext& context) = 0;

    /**
     * @brief 检查 API Key 维度限流。
     * @param context 请求上下文，主要使用 api_key_id、user_id、模型和能力信息。
     * @return 成功时返回是否允许请求；失败时返回限流后端或配置错误。
     */
    virtual Result<LimitDecision> checkApiKeyLimit(const GatewayContext& context) = 0;

    /**
     * @brief 检查公共模型维度限流。
     * @param context 请求上下文，主要使用 public_model_name、capability 和 stream。
     * @return 成功时返回是否允许请求；失败时返回限流后端或配置错误。
     */
    virtual Result<LimitDecision> checkModelLimit(const GatewayContext& context) = 0;

    /**
     * @brief 检查指定上游账号和能力维度的限流。
     * @param upstream_account_id 需要检查的上游账号唯一标识。
     * @param capability 本次请求需要的模型能力。
     * @return 成功时返回是否允许调用该上游账号；失败时返回限流后端或账号配置错误。
     */
    virtual Result<LimitDecision> checkUpstreamLimit(const std::string& upstream_account_id, Capability capability) = 0;

    /**
     * @brief 在请求确定执行后消费限流计数或配额。
     * @param context 请求上下文，提供用户、API Key、模型和能力维度。
     * @param upstream_account_id 实际被调用的上游账号唯一标识。
     * @return 成功时表示限流计数已消费；失败时返回并发冲突、配额不足或后端错误。
     */
    virtual Status consume(const GatewayContext& context, const std::string& upstream_account_id) = 0;
};

}
