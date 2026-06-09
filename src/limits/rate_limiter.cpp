/**
 * @file rate_limiter.cpp
 * @brief 限流接口实现预留源文件。
 *
 * 后续实现方向：
 * - 实现用户、API Key、公共模型和上游账号维度的限流检查。
 * - 使用 GatewayContext 中的 user_id、api_key_id、public_model_name、capability 和 stream 作为限流变量。
 * - 在 consume 中扣减请求计数、Token 计数或并发额度。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定限流算法或后端。
 */
#include "ai_gateway_core/limits/rate_limiter.h"

namespace ai_gateway_core {

}
