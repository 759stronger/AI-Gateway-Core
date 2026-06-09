/**
 * @file upstream_pool.cpp
 * @brief 上游账号池接口实现预留源文件。
 *
 * 后续实现方向：
 * - 管理 UpstreamAccount 的新增、更新、禁用、查询和候选筛选。
 * - 根据 supported_models、capabilities、enabled、status 和并发计数筛选可用账号。
 * - 维护成功失败统计、平均延迟和并发槽位。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定账号池存储和并发模型。
 */
#include "ai_gateway_core/upstream/upstream_pool.h"

namespace ai_gateway_core {

}
