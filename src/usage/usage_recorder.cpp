/**
 * @file usage_recorder.cpp
 * @brief 用量记录接口实现预留源文件。
 *
 * 后续实现方向：
 * - 将 UsageRecord 写入 Storage 或专用审计表。
 * - 支持按 record_id 查询请求用量明细。
 * - 与 BillingPolicy 的 cost 和 Observability 的 trace_id 保持一致。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定具体审计存储方式。
 */
#include "ai_gateway_core/usage/usage_recorder.h"

namespace ai_gateway_core {

}
