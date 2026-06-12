/**
 * @file llm_provider.h
 * @brief 定义大语言模型供应商的统一调用接口。
 *
 * 该文件将不同供应商的聊天模型调用抽象为统一 Provider 接口，网关路由完成后，
 * 可以通过 provider_name 找到对应实现，并把上游账号、真实模型名和消息发送给供应商。
 *
 * 与其他文件的联系：
 * - providers/provider_manager.h 管理 LLMProvider 实例注册和查找。
 * - routing/model_router.h 输出的 RoutingDecision 会提供 upstream_account 和 upstream_model_name。
 * - adapters/openai_adapter.h 提供 ChatCompletionResponse 作为内部响应格式。
 */
#pragma once

#include "ai_gateway_core/adapters/openai_adapter.h"
#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include <string>
#include <functional>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief 发送给具体 LLM 供应商的聊天请求。
 *
 * 字段说明：
 * - upstream_account：已选中的上游账号，包含 base_url、认证信息、状态和限制。
 * - upstream_model_name：供应商真实模型名。
 * - messages：需要发送给模型的聊天上下文。
 * - parameters：温度、top_p、最大输出长度等供应商参数。
 * - stream：是否使用流式输出。
 */
struct ProviderChatRequest {
    UpstreamAccount upstream_account;
    std::string upstream_model_name;
    std::vector<Message> messages;
    std::map<std::string, std::string> parameters;
    bool stream = false;
};

/**
 * @brief 流式聊天响应回调函数类型。
 *
 * 参数说明：
 * - chunk：当前收到的聊天响应片段。
 * - done：是否为最后一个片段，true 表示流式响应结束。
 */
using StreamCallback = std::function<void(const ChatCompletionResponse& chunk, bool done)>;

/**
 * @brief 大语言模型供应商接口。
 */
class LLMProvider {
public:
    virtual ~LLMProvider() = default;

    /**
     * @brief 返回供应商名称。
     * @return 供应商唯一名称，应与 UpstreamAccount::provider_name 和 ModelMapping::upstream_provider_name 对齐。
     */
    virtual std::string providerName() const = 0;

    /**
     * @brief 返回供应商支持的能力列表。
     * @return Capability 列表，用于注册、路由和能力校验。
     */
    virtual std::vector<Capability> capabilities() const = 0;

    /**
     * @brief 对指定上游账号执行健康检查。
     * @param account 需要检查的上游账号，包含连接地址和认证信息。
     * @return 成功时表示账号可用；失败时返回网络、认证或供应商不可用错误。
     */
    virtual Status healthCheck(const UpstreamAccount& account) = 0;

    /**
     * @brief 发送非流式聊天请求。
     * @param request 供应商聊天请求，包含账号、真实模型名、消息和参数。
     * @return 成功时返回完整聊天响应；失败时返回网络、限流或响应解析错误。
     */
    virtual Result<ChatCompletionResponse> sendChat(const ProviderChatRequest& request) = 0;

    /**
     * @brief 发送流式聊天请求。
     * @param request 供应商聊天请求，stream 通常应为 true。
     * @param callback 每收到一个响应片段时调用的回调函数。
     * @return 成功时表示流式请求正常结束；失败时返回发送、网络或中途解析错误。
     */
    virtual Status sendStreamChat(const ProviderChatRequest& request, StreamCallback callback) = 0;
};

/**
 * @brief 用于开发和教学的模拟大模型供应商。
 *
 * 设计意图：
 * - 不依赖真实上游网络，就能让路由、协议适配和服务链路跑起来。
 * - 适合第一阶段验证网关逻辑，也适合单元测试。
 */
class MockLLMProvider : public LLMProvider {
public:
    /**
     * @brief 创建一个模拟 Provider。
     * @param provider_name 这个模拟供应商对外暴露的名称。
     */
    explicit MockLLMProvider(std::string provider_name = "mock");

    /// @brief 返回模拟供应商名称。
    std::string providerName() const override;
    /// @brief 返回模拟供应商支持的能力列表。
    std::vector<Capability> capabilities() const override;
    /// @brief 模拟对上游账号执行健康检查。
    Status healthCheck(const UpstreamAccount& account) override;
    /// @brief 生成一条完整的模拟聊天响应。
    Result<ChatCompletionResponse> sendChat(const ProviderChatRequest& request) override;
    /// @brief 按流式回调方式生成模拟聊天片段。
    Status sendStreamChat(const ProviderChatRequest& request, StreamCallback callback) override;

private:
    /// @brief 构造模拟聊天响应对象。
    ChatCompletionResponse buildResponse(const ProviderChatRequest& request,
                                         const std::string& content,
                                         bool stream_chunk) const;
    /// @brief 提取请求里最后一条用户消息，作为模拟回复素材。
    std::string extractLastUserMessage(const ProviderChatRequest& request) const;

    /// @brief 模拟供应商的对外名称。
    std::string provider_name_;
};

}
