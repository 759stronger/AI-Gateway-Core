/**
 * @file model_mapping.h
 * @brief 定义公共模型名到上游供应商模型名的映射关系。
 *
 * 该文件描述客户端可见模型与真实供应商模型之间的转换规则。网关通过该映射隐藏
 * 上游供应商差异，并可以按能力、价格倍率和启用状态控制模型是否可用。
 *
 * 与其他文件的联系：
 * - routing/model_router.h 使用 ModelMapping 生成最终路由决策。
 * - billing/billing_policy.h 使用 price_multiplier 参与费用计算。
 * - storage/storage.h 和 admin/admin_api.h 负责映射配置的持久化与管理。
 */
#pragma once

#include "ai_gateway_core/core/types.h"
#include <string>

namespace ai_gateway_core {

/**
 * @brief 单条模型映射配置。
 *
 * 字段说明：
 * - mapping_id：映射规则唯一标识。
 * - public_model_name：客户端请求时使用的公共模型名。
 * - upstream_model_name：真实供应商侧模型名。
 * - upstream_provider_name：真实供应商名称，关联 providers/provider_manager.h。
 * - capability：该映射适用的能力类型。
 * - enabled：该映射是否可被路由使用。
 * - price_multiplier：价格倍率，用于在基础成本上做业务侧加价或折扣。
 */
struct ModelMapping {
    std::string mapping_id;
    std::string public_model_name;
    std::string upstream_model_name;
    std::string upstream_provider_name;
    Capability capability = Capability::ChatCompletion;
    bool enabled = false;
    double price_multiplier = 1.0;
};

}
