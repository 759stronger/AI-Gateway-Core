/**
 * @file types.h
 * @brief 定义 AI Gateway Core 全局共享的基础业务类型。
 *
 * 该文件保存模型能力、消息角色、会话、模型信息、Token 用量和请求上下文等基础 DTO。
 * 这些类型会被认证、路由、供应商调用、计费、限流、用量记录和可观测性模块共同引用。
 *
 * 与其他文件的联系：
 * - routing/model_mapping.h 使用 Capability 描述公共模型到上游模型的能力维度。
 * - adapters/openai_adapter.h 和 providers/llm_provider.h 使用 Message、TokenUsage 表达聊天请求和响应。
 * - limits/rate_limiter.h、billing/billing_policy.h、observability/observability.h 使用 GatewayContext 获取请求身份和链路信息。
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace ai_gateway_core {

/**
 * @brief 模型或上游账号支持的能力类型。
 */
enum class Capability {
    ChatCompletion,      // 聊天补全能力，用于普通对话、问答和多轮聊天请求。
    ImageGeneration,     // 图片生成能力，用于根据文字提示词生成图片。
    Embedding,           // 文本向量化能力，用于语义搜索、相似度匹配和 RAG 检索。
    AudioTranscription,  // 语音转文字能力，用于把音频内容识别成文本。
    TextToSpeech,        // 文字转语音能力，用于把文本内容合成为音频。
    ToolCalling,         // 工具调用能力，用于让模型决定是否调用外部函数或 API。
    Rerank               // 重排序能力，用于对搜索结果、候选答案或文档列表重新排序。
};

/**
 * @brief 聊天消息在对话中的发送方角色。
 */
enum class Role {
    System,     // 系统角色，用于设置模型行为规则、身份和全局指令。
    User,       // 用户角色，表示终端用户输入的问题或请求内容。
    Assistant,  // 助手角色，表示模型生成的回复内容。
    Tool        // 工具角色，表示外部工具或函数调用返回给模型的结果。
};

/**
 * @brief 单条对话消息。
 *
 * 字段说明：
 * - id：消息唯一标识，用于存储、追踪或前端去重。
 * - role：消息角色，决定内容在模型上下文中的语义。
 * - content：消息正文。
 * - tool_call_id：工具调用消息关联的调用标识。
 * - metadata：扩展键值信息，用于保存协议外业务属性。
 * - timestamp：消息创建时间戳。
 */
struct Message {
    std::string id;
    Role role = Role::User;
    std::string content;
    std::string tool_call_id;
    std::map<std::string, std::string> metadata;
    std::time_t timestamp = 0;
};

/**
 * @brief 用户与某个公共模型之间的一段会话。
 *
 * 字段说明：
 * - session_id：会话唯一标识。
 * - user_id：会话归属用户，关联 users/user_manager.h 中的 UserAccount。
 * - public_model_name：用户侧看到的模型名称，关联 routing/model_mapping.h。
 * - title：会话标题。
 * - system_prompt：会话级系统提示词。
 * - messages：会话内的消息列表。
 * - metadata：会话扩展属性。
 * - created_at：会话创建时间戳。
 * - updated_at：会话最后更新时间戳。
 */
struct Session {
    std::string session_id;
    std::string user_id;
    std::string public_model_name;
    std::string title;
    std::string system_prompt;
    std::vector<Message> messages;
    std::map<std::string, std::string> metadata;
    std::time_t created_at = 0;
    std::time_t updated_at = 0;
};

/**
 * @brief 可对外展示或注册的模型信息。
 *
 * 字段说明：
 * - public_model_name：客户端请求使用的模型名称。
 * - upstream_model_name：供应商真实模型名称。
 * - provider_name：模型所属供应商名称，关联 providers/provider_manager.h。
 * - description：模型描述信息。
 * - capabilities：模型支持的能力集合。
 * - enabled：模型是否可被路由和调用。
 */
struct ModelInfo {
    std::string public_model_name;
    std::string upstream_model_name;
    std::string provider_name;
    std::string description;
    std::vector<Capability> capabilities;
    bool enabled = false;
};

/**
 * @brief 一次模型调用产生的 Token 用量统计。
 *
 * 字段说明：
 * - prompt_tokens：输入提示词消耗的 Token 数。
 * - completion_tokens：模型输出消耗的 Token 数。
 * - total_tokens：总 Token 数，通常等于输入和输出之和。
 */
struct TokenUsage {
    int prompt_tokens = 0;
    int completion_tokens = 0;
    int total_tokens = 0;
};

/**
 * @brief 网关处理单次请求时贯穿各模块的上下文。
 *
 * 字段说明：
 * - trace_id：链路追踪标识，供 observability/observability.h 记录全流程事件。
 * - user_id：当前用户标识，关联 users/user_manager.h 和计费限额逻辑。
 * - api_key_id：当前 API Key 标识，关联 auth/auth_manager.h。
 * - public_model_name：客户端请求的公共模型名，供 routing/model_router.h 选择上游模型。
 * - capability：本次请求需要的模型能力。
 * - stream：请求是否使用流式响应，影响供应商调用和响应构建。
 */
struct GatewayContext {
    std::string trace_id;
    std::string user_id;
    std::string api_key_id;
    std::string public_model_name;
    Capability capability = Capability::ChatCompletion;
    bool stream = false;
};
}
