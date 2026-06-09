/**
 * @file model_router.cpp
 * @brief 模型路由接口实现预留源文件。
 *
 * 后续实现方向：
 * - 根据 RoutingRequest 中的 user_id、session_id、public_model_name、capability、policy 和 stream 选择上游。
 * - 结合 ModelMapping、UpstreamPool、HealthSnapshot 和限流结果生成 RoutingDecision。
 * - 在 selectFallback 中根据 failed_account_id 和 Error 选择备用账号。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定具体路由算法。
 */
#include "ai_gateway_core/routing/model_router.h"

namespace ai_gateway_core {

}
