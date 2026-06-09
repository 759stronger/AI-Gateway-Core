/**
 * @file auth_manager.cpp
 * @brief API Key 认证管理接口实现预留源文件。
 *
 * 后续实现方向：
 * - 解析 Authorization Bearer 头并计算 API Key 哈希。
 * - 通过 Storage 查询 ApiKeyIdentity 并校验 enabled、expires_at 和 user_id。
 * - 创建 API Key 时只返回一次明文，并持久化安全哈希值。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免引入未确定的哈希算法。
 */
#include "ai_gateway_core/auth/auth_manager.h"

namespace ai_gateway_core {

}
