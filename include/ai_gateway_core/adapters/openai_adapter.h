/**
 * @file openai_adapter.h
 * @brief 定义 OpenAI 兼容接口的请求解析与响应构建抽象。
 *
 * 该文件把外部 HTTP/JSON 协议与网关内部 DTO 隔离开：入口层可以通过
 * OpenAIAdapter 将请求体解析为内部请求，再将模型响应或错误转换为 OpenAI 兼容响应。
 *
 * 与其他文件的联系：
 * - gateway/gateway_server.h 的实现通常会调用本接口处理 HTTP body。
 * - providers/llm_provider.h 使用 ChatCompletionRequest/Response 与供应商进行聊天补全交互。
 * - images/image_gateway.h 使用 ImageGenerationRequest/Response 处理图片生成。
 * - core/result.h 的 Error 可通过 buildErrorResponse 转换成协议响应。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include <map>
#include <string>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief OpenAI 兼容聊天补全请求的内部表示。
 *
 * 字段说明：
 * - public_model_name：客户端传入的公共模型名，后续由路由模块映射到上游模型。
 * - messages：聊天上下文消息列表。
 * - parameters：温度、top_p 等协议扩展参数的字符串键值表示。
 * - stream：是否请求流式输出。
 */
struct ChatCompletionRequest {
    std::string public_model_name;
    std::vector<Message> messages;
    std::map<std::string, std::string> parameters;
    bool stream = false;
};

/**
 * @brief OpenAI 兼容聊天补全响应的内部表示。
 *
 * 字段说明：
 * - id：响应唯一标识。
 * - public_model_name：客户端请求的公共模型名。
 * - assistant_message：模型生成的助手消息。
 * - usage：本次响应的 Token 用量。
 * - finish_reason：模型停止生成的原因。
 */
struct ChatCompletionResponse {
    std::string id;
    std::string public_model_name;
    Message assistant_message;
    TokenUsage usage;
    std::string finish_reason;
};

/**
 * @brief OpenAI 兼容图片生成请求的内部表示。
 *
 * 字段说明：
 * - public_model_name：客户端传入的公共图片模型名。
 * - prompt：图片生成提示词。
 * - size：期望图片尺寸。
 * - count：生成图片数量。
 * - parameters：图片生成的扩展参数。
 */
struct ImageGenerationRequest {
    std::string public_model_name;
    std::string prompt;
    std::string size;
    int count = 1;
    std::map<std::string, std::string> parameters;
};

/**
 * @brief 图片生成响应的内部表示。
 *
 * 字段说明：
 * - id：响应唯一标识。
 * - image_urls：远程图片地址列表。
 * - image_base64：Base64 编码图片内容列表。
 */
struct ImageGenerationResponse {
    std::string id;
    std::vector<std::string> image_urls;
    std::vector<std::string> image_base64;
};

/**
 * @brief OpenAI 兼容协议适配器接口。
 */
class OpenAIAdapter {
public:
    virtual ~OpenAIAdapter() = default;

    /**
     * @brief 将聊天补全 HTTP 请求体解析为内部请求对象。
     * @param body 原始 HTTP 请求体，通常是 OpenAI 兼容 JSON。
     * @return 成功时返回 ChatCompletionRequest；失败时返回解析或参数错误。
     */
    virtual Result<ChatCompletionRequest> parseChatCompletionRequest(const std::string& body) = 0;

    /**
     * @brief 将图片生成 HTTP 请求体解析为内部请求对象。
     * @param body 原始 HTTP 请求体，通常是 OpenAI 兼容 JSON。
     * @return 成功时返回 ImageGenerationRequest；失败时返回解析或参数错误。
     */
    virtual Result<ImageGenerationRequest> parseImageGenerationRequest(const std::string& body) = 0;

    /**
     * @brief 将完整聊天响应构建为 OpenAI 兼容响应体。
     * @param response 内部聊天响应对象，包含消息、模型名和 Token 用量。
     * @return 成功时返回可写入 HTTP 响应的字符串。
     */
    virtual Result<std::string> buildChatCompletionResponse(const ChatCompletionResponse& response) = 0;

    /**
     * @brief 将流式聊天响应片段构建为 OpenAI 兼容增量响应体。
     * @param response 当前聊天响应片段或最终响应对象。
     * @param done 是否为流式输出结束片段。
     * @return 成功时返回可写入 SSE 或流式 HTTP 响应的字符串。
     */
    virtual Result<std::string> buildChatCompletionChunk(const ChatCompletionResponse& response, bool done) = 0;

    /**
     * @brief 将图片生成结果构建为 OpenAI 兼容响应体。
     * @param response 内部图片生成响应对象。
     * @return 成功时返回可写入 HTTP 响应的字符串。
     */
    virtual Result<std::string> buildImageGenerationResponse(const ImageGenerationResponse& response) = 0;

    /**
     * @brief 将内部错误构建为 OpenAI 兼容错误响应体。
     * @param error 内部错误对象，包含错误码和可读错误信息。
     * @return 成功时返回可写入 HTTP 响应的错误字符串。
     */
    virtual Result<std::string> buildErrorResponse(const Error& error) = 0;
};

/**
 * @brief OpenAI 兼容协议的默认实现。
 *
 * 设计意图：
 * - 把 JSON 协议细节集中在这一层，避免业务模块直接操作外部协议格式。
 * - 上层拿到的都是内部 DTO，下层输出的都是标准 OpenAI 兼容字符串。
 */
class DefaultOpenAIAdapter : public OpenAIAdapter {
public:
    /// @brief 解析聊天补全请求体。
    Result<ChatCompletionRequest> parseChatCompletionRequest(const std::string& body) override;
    /// @brief 解析图片生成请求体。
    Result<ImageGenerationRequest> parseImageGenerationRequest(const std::string& body) override;
    /// @brief 构建完整聊天响应体。
    Result<std::string> buildChatCompletionResponse(const ChatCompletionResponse& response) override;
    /// @brief 构建流式聊天响应片段。
    Result<std::string> buildChatCompletionChunk(const ChatCompletionResponse& response, bool done) override;
    /// @brief 构建图片生成响应体。
    Result<std::string> buildImageGenerationResponse(const ImageGenerationResponse& response) override;
    /// @brief 构建 OpenAI 兼容错误响应体。
    Result<std::string> buildErrorResponse(const Error& error) override;

private:
    /// @brief 将协议中的角色文本解析为内部 Role 枚举。
    Result<Role> parseRole(const std::string& role_text) const;
    /// @brief 将内部 Role 枚举转换回协议字符串。
    std::string roleToString(Role role) const;
    /// @brief 组装聊天响应 JSON 字符串，可同时支持完整和流式场景。
    std::string makeChatResponseObject(const ChatCompletionResponse& response, bool stream_mode, bool done) const;
};
}
