/**
 * @file billing_policy.h
 * @brief 定义请求计费预检查、费用计算和费用提交接口。
 *
 * 该文件封装计费策略：请求前可预估成本并判断是否允许执行，请求后根据实际 Token
 * 用量计算真实成本，并将成本提交到账户或账单系统。
 *
 * 与其他文件的联系：
 * - core/types.h 提供 GatewayContext 和 TokenUsage。
 * - routing/model_mapping.h 提供价格倍率和上游模型信息。
 * - users/user_manager.h 可在 commitCost 阶段扣减用户余额或额度。
 * - usage/usage_recorder.h 可记录最终成本到 UsageRecord。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace ai_gateway_core {

class UserManager;

/**
 * @brief 计费预检查结果。
 *
 * 字段说明：
 * - allowed：是否允许继续执行请求。
 * - estimated_cost：预估费用，用于请求前余额或预算判断。
 */
struct BillingDecision {
    bool allowed = false;
    double estimated_cost = 0.0;
};

/**
 * @brief 单个模型的基础价格配置。
 *
 * 字段说明：
 * - prompt_price_per_1k_tokens：每 1000 个输入 token 的价格。
 * - completion_price_per_1k_tokens：每 1000 个输出 token 的价格。
 * - request_base_price：每次请求的固定基础成本。
 */
struct ModelPricing {
    double prompt_price_per_1k_tokens = 0.0;
    double completion_price_per_1k_tokens = 0.0;
    double request_base_price = 0.0;
};

/**
 * @brief 计费策略接口。
 */
class BillingPolicy {
public:
    virtual ~BillingPolicy() = default;

    /**
     * @brief 在请求发送到供应商前执行计费预检查。
     * @param context 请求上下文，包含用户、API Key、公共模型和能力信息。
     * @param mapping 路由命中的模型映射，包含上游模型和价格倍率。
     * @return 成功时返回是否允许请求以及预估费用；失败时返回计费配置或账户错误。
     */
    virtual Result<BillingDecision> preCheck(const GatewayContext& context, const ModelMapping& mapping) = 0;

    /**
     * @brief 根据实际用量计算请求成本。
     * @param context 请求上下文，提供用户、模型和能力维度。
     * @param mapping 路由命中的模型映射，提供上游模型和价格倍率。
     * @param usage 本次请求实际 Token 用量。
     * @return 成功时返回最终成本；失败时返回费率缺失或计算错误。
     */
    virtual Result<double> calculateCost(const GatewayContext& context, const ModelMapping& mapping, const TokenUsage& usage) = 0;

    /**
     * @brief 提交最终成本并完成扣费或账单落账。
     * @param context 请求上下文，包含需要扣费的用户和 API Key 信息。
     * @param cost 已计算出的最终成本。
     * @return 成功时表示成本已提交；失败时返回余额不足、并发冲突或存储错误。
     */
    virtual Status commitCost(const GatewayContext& context, double cost) = 0;
};

/**
 * @brief 一个简单、清晰的默认计费策略实现。
 *
 * 设计意图：
 * - 先根据基础费率和映射倍率完成一套可运行的计费逻辑。
 * - 让你在不引入复杂账单系统的前提下，先把“预检查 -> 计算 -> 扣费”主链路走通。
 */
class SimpleBillingPolicy : public BillingPolicy {
public:
    /**
     * @brief 创建默认计费策略。
     * @param user_manager 用于最终执行扣费或额度扣减的用户管理器。
     */
    explicit SimpleBillingPolicy(std::shared_ptr<UserManager> user_manager);

    /// @brief 在请求执行前判断是否允许继续调用。
    Result<BillingDecision> preCheck(const GatewayContext& context, const ModelMapping& mapping) override;
    /// @brief 根据实际 token 用量计算最终成本。
    Result<double> calculateCost(const GatewayContext& context,
                                 const ModelMapping& mapping,
                                 const TokenUsage& usage) override;
    /// @brief 将成本提交给用户账户系统。
    Status commitCost(const GatewayContext& context, double cost) override;

    /// @brief 设置没有模型专属配置时使用的默认费率。
    void setDefaultPricing(ModelPricing pricing);
    /// @brief 为某个公共模型设置专属费率。
    void setModelPricing(const std::string& public_model_name, ModelPricing pricing);

private:
    /// @brief 解析某个公共模型最终应使用的费率配置。
    ModelPricing resolvePricing(const std::string& public_model_name) const;

    /// @brief 负责执行最终扣费的用户管理器。
    std::shared_ptr<UserManager> user_manager_;
    /// @brief 默认费率。
    ModelPricing default_pricing_;
    /// @brief 模型专属费率表，key 为 public_model_name。
    std::unordered_map<std::string, ModelPricing> model_pricing_;
};

}
