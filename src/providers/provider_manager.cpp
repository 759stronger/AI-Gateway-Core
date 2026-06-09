/**
 * @file provider_manager.cpp
 * @brief 供应商管理接口实现预留源文件。
 *
 * 后续实现方向：
 * - 维护 providerName 到 LLMProvider 实例的映射表。
 * - 在 registerProvider 中校验空指针、重复名称和能力列表。
 * - 在 getProvider 中为路由结果提供具体供应商实例。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免过早固定容器和线程模型。
 */
#include "ai_gateway_core/providers/provider_manager.h"

namespace ai_gateway_core {

}
