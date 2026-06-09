/**
 * @file llm_provider.cpp
 * @brief 大语言模型供应商接口实现预留源文件。
 *
 * 后续实现方向：
 * - 实现 OpenAI、Claude、本地模型等具体 LLMProvider 子类。
 * - 将 ProviderChatRequest 中的 upstream_account、upstream_model_name、messages、parameters 和 stream
 *   转换为真实供应商请求。
 * - 将供应商响应统一转换为 ChatCompletionResponse 或流式 StreamCallback 片段。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免绑定具体模型供应商。
 */
#include "ai_gateway_core/providers/llm_provider.h"

namespace ai_gateway_core {

}
