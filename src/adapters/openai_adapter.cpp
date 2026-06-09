/**
 * @file openai_adapter.cpp
 * @brief OpenAI 兼容协议适配接口实现预留源文件。
 *
 * 后续实现方向：
 * - 将 HTTP JSON 请求体解析为 ChatCompletionRequest 或 ImageGenerationRequest。
 * - 将 ChatCompletionResponse、ImageGenerationResponse 和 Error 构建为 OpenAI 兼容 JSON。
 * - 在流式响应中根据 done 变量输出普通 chunk 或结束标记。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定 JSON 库和 HTTP 框架。
 */
#include "ai_gateway_core/adapters/openai_adapter.h"

namespace ai_gateway_core {

}
