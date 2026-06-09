/**
 * @file provider_manager.h
 * @brief 定义模型供应商实现的注册、查找和枚举接口。
 *
 * 该文件用于维护 provider_name 到 LLMProvider 实例的映射。网关根据路由结果中的
 * upstream_provider_name 或 UpstreamAccount::provider_name 查找具体供应商实现。
 *
 * 与其他文件的联系：
 * - providers/llm_provider.h 定义被管理的供应商接口类型。
 * - routing/model_mapping.h 和 upstream/upstream_account.h 中的供应商名称应能在此处查到。
 * - gateway/gateway_server.h 的实现通常在初始化时注册所有供应商。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/providers/llm_provider.h"
#include <memory>
#include <string>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief LLM 供应商管理接口。
 */
class ProviderManager {
public:
    virtual ~ProviderManager() = default;

    /**
     * @brief 注册一个 LLM 供应商实现。
     * @param provider 供应商实例，providerName() 返回值会作为查找键。
     * @return 成功时表示供应商已可用于后续路由调用；失败时返回空指针或重复注册错误。
     */
    virtual Status registerProvider(std::shared_ptr<LLMProvider> provider) = 0;

    /**
     * @brief 根据供应商名称获取 LLM 供应商实现。
     * @param provider_name 供应商唯一名称。
     * @return 成功时返回供应商实例；失败时返回未注册错误。
     */
    virtual Result<std::shared_ptr<LLMProvider>> getProvider(const std::string& provider_name) = 0;

    /**
     * @brief 列出当前已注册的供应商名称。
     * @return 成功时返回供应商名称列表；失败时返回内部状态错误。
     */
    virtual Result<std::vector<std::string>> listProviderNames() const = 0;
};

}
