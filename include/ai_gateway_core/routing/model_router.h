/**
 * @file model_router.h
 * @brief 定义模型请求路由策略、路由输入和路由结果。
 *
 * 该文件负责抽象“某次客户端模型请求应该发送到哪个上游账号和哪个真实模型”。
 * 路由实现通常结合模型映射、上游账号池、健康状态、限流和策略权重来做选择。
 *
 * 与其他文件的联系：
 * - routing/model_mapping.h 提供公共模型到上游模型的映射。
 * - upstream/upstream_account.h 提供可选上游账号及其状态。
 * - upstream/upstream_pool.h 通常为路由器提供候选账号列表。
 * - observability/observability.h 可记录路由决策和降级切换事件。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include <string>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief 上游账号选择策略。
 */
enum class RoutingPolicy {
    RoundRobin,
    Weighted,
    LeastLatency,
    LeastUsed,
    LocalFirst,
    PriorityFirst,
    StickySession
};

/**
 * @brief 一次模型路由请求的输入条件。
 *
 * 字段说明：
 * - user_id：发起请求的用户标识，可用于用户级灰度、粘性路由或配额策略。
 * - session_id：会话标识，可用于 StickySession 策略保持同一会话走同一上游。
 * - public_model_name：客户端请求的公共模型名。
 * - capability：本次请求需要的模型能力。
 * - policy：本次路由希望采用的选择策略。
 * - stream：是否为流式请求，部分上游可能不支持流式能力。
 */
struct RoutingRequest {
    std::string user_id;
    std::string session_id;
    std::string public_model_name;
    Capability capability = Capability::ChatCompletion;
    RoutingPolicy policy = RoutingPolicy::RoundRobin;
    bool stream = false;
};

/**
 * @brief 一次路由选择的结果。
 *
 * 字段说明：
 * - mapping：命中的模型映射，说明公共模型实际对应哪个上游模型和供应商。
 * - upstream_account：最终选择的上游账号，后续供应商调用会使用其地址和认证信息。
 */
struct RoutingDecision {
    ModelMapping mapping;
    UpstreamAccount upstream_account;
};

/**
 * @brief 模型路由器接口。
 */
class ModelRouter {
public:
    virtual ~ModelRouter() = default;

    /**
     * @brief 为请求选择首选上游账号和模型映射。
     * @param request 路由输入条件，包括用户、会话、公共模型、能力、策略和流式标记。
     * @return 成功时返回路由决策；失败时返回无可用模型、无可用账号或策略错误。
     */
    virtual Result<RoutingDecision> selectUpstream(const RoutingRequest& request) = 0;

    /**
     * @brief 在首选上游失败后选择备用上游。
     * @param request 原始路由输入条件。
     * @param failed_account_id 已失败的上游账号标识，应从备选结果中排除或降权。
     * @param error 首选上游失败的错误信息，用于决定是否允许降级或选择特定备选。
     * @return 成功时返回备用路由决策；失败时返回无可用备用账号或不可重试错误。
     */
    virtual Result<RoutingDecision> selectFallback(const RoutingRequest& request, const std::string& failed_account_id, const Error& error) = 0;
};

}
