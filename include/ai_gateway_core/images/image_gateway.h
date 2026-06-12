/**
 * @file image_gateway.h
 * @brief 定义图片生成供应商接口和图片网关入口接口。
 *
 * 该文件覆盖图片生成能力的供应商请求格式和网关级生成接口。图片网关通常复用认证、
 * 路由、限流、计费、用量记录和 OpenAI 兼容响应结构，但能力类型为 ImageGeneration。
 *
 * 与其他文件的联系：
 * - adapters/openai_adapter.h 提供 ImageGenerationRequest/Response 协议 DTO。
 * - upstream/upstream_account.h 提供调用供应商所需的账号与端点信息。
 * - routing/model_mapping.h 可将公共图片模型映射为上游图片模型。
 */
#pragma once

#include "ai_gateway_core/adapters/openai_adapter.h"
#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include <memory>
#include <string>
#include <vector>

namespace ai_gateway_core {

class BillingPolicy;
class ModelRouter;
class RateLimiter;
class UsageRecorder;

/**
 * @brief 发送给图片供应商的图片生成请求。
 *
 * 字段说明：
 * - upstream_account：已选中的上游账号，包含连接地址和认证信息。
 * - upstream_model_name：供应商真实图片模型名。
 * - prompt：图片生成提示词。
 * - size：期望图片尺寸。
 * - count：需要生成的图片数量。
 */
struct ProviderImageRequest {
    UpstreamAccount upstream_account;
    std::string upstream_model_name;
    std::string prompt;
    std::string size;
    int count = 1;
};

/**
 * @brief 图片生成供应商接口。
 */
class ImageProvider {
public:
    virtual ~ImageProvider() = default;

    /**
     * @brief 返回图片供应商名称。
     * @return 供应商唯一名称，应与上游账号和模型映射中的供应商名称一致。
     */
    virtual std::string providerName() const = 0;

    /**
     * @brief 对指定图片上游账号执行健康检查。
     * @param account 需要检查的上游账号，包含连接地址和认证信息。
     * @return 成功时表示图片供应商账号可用；失败时返回认证、网络或供应商不可用错误。
     */
    virtual Status healthCheck(const UpstreamAccount& account) = 0;

    /**
     * @brief 调用供应商生成图片。
     * @param request 图片供应商请求，包含账号、真实模型名、提示词、尺寸和数量。
     * @return 成功时返回图片 URL 或 Base64 内容；失败时返回网络、限流或解析错误。
     */
    virtual Result<ImageGenerationResponse> generateImage(const ProviderImageRequest& request) = 0;
};

/**
 * @brief 图片生成网关接口。
 */
class ImageGateway {
public:
    virtual ~ImageGateway() = default;

    /**
     * @brief 处理一次客户端图片生成请求。
     * @param context 请求上下文，包含用户、API Key、公共模型、能力和链路追踪信息。
     * @param request 客户端图片生成请求，包含公共模型、提示词、尺寸、数量和扩展参数。
     * @return 成功时返回图片生成结果；失败时返回认证、路由、限流、计费或供应商错误。
     */
    virtual Result<ImageGenerationResponse> generate(const GatewayContext& context, const ImageGenerationRequest& request) = 0;
};

/**
 * @brief 图片网关的基础实现声明。
 *
 * 设计意图：
 * - 让图片能力复用认证、路由、限流、计费和用量记录链路。
 * - 与聊天网关保持相似结构，降低后续维护成本。
 */
class BasicImageGateway : public ImageGateway {
public:
    /**
     * @brief 创建基础图片网关。
     * @param model_router 用于选择图片上游模型和账号。
     * @param rate_limiter 用于图片请求限流。
     * @param billing_policy 用于图片请求计费。
     * @param usage_recorder 用于记录图片请求审计信息。
     */
    BasicImageGateway(std::shared_ptr<ModelRouter> model_router,
                      std::shared_ptr<RateLimiter> rate_limiter,
                      std::shared_ptr<BillingPolicy> billing_policy,
                      std::shared_ptr<UsageRecorder> usage_recorder);

    /// @brief 执行一次图片生成主链路。
    Result<ImageGenerationResponse> generate(const GatewayContext& context,
                                             const ImageGenerationRequest& request) override;

private:
    /// @brief 模型路由依赖。
    std::shared_ptr<ModelRouter> model_router_;
    /// @brief 限流依赖。
    std::shared_ptr<RateLimiter> rate_limiter_;
    /// @brief 计费依赖。
    std::shared_ptr<BillingPolicy> billing_policy_;
    /// @brief 用量记录依赖。
    std::shared_ptr<UsageRecorder> usage_recorder_;
};

}
