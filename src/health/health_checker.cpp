/**
 * @file health_checker.cpp
 * @brief 健康检查接口实现预留源文件。
 *
 * 后续实现方向：
 * - 调用 LLMProvider 或 ImageProvider 对上游账号执行健康检查。
 * - 生成 HealthSnapshot 并更新 UpstreamStatus、latency_ms 和 last_error。
 * - 为路由模块提供可用率和延迟数据。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定供应商探测方式。
 */
#include "ai_gateway_core/health/health_checker.h"

namespace ai_gateway_core {

}
