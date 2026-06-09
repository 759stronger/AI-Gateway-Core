/**
 * @file observability.cpp
 * @brief 可观测性接口实现预留源文件。
 *
 * 后续实现方向：
 * - 实现 trace_id 生成策略。
 * - 将请求开始、路由决策、降级切换和请求结束事件写入日志或指标系统。
 * - 将 GatewayContext 中的变量转换为可观测性标签。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免绑定具体日志或追踪系统。
 */
#include "ai_gateway_core/observability/observability.h"

namespace ai_gateway_core {

}
