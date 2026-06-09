/**
 * @file storage.h
 * @brief 定义网关核心数据的统一持久化接口。
 *
 * 该文件抽象用户、API Key、上游账号、模型映射和用量记录的存储访问。
 * 所有需要持久化的数据都应通过本接口读写，便于替换底层数据库实现。
 *
 * 与其他文件的联系：
 * - auth/auth_manager.h 和 users/user_manager.h 通常通过本接口持久化身份和用户数据。
 * - upstream/upstream_account.h、routing/model_mapping.h 的配置数据由本接口保存。
 * - usage/usage_recorder.h 的用量审计记录通过 saveUsageRecord 持久化。
 */
#pragma once

#include "ai_gateway_core/auth/auth_manager.h"
#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include "ai_gateway_core/usage/usage_recorder.h"
#include "ai_gateway_core/users/user_manager.h"
#include <string>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief 网关核心数据存储接口。
 */
class Storage {
public:
    virtual ~Storage() = default;

    /**
     * @brief 初始化存储系统，创建表结构或连接数据库。
     * @return 成功时表示存储已就绪；失败时返回数据库连接或初始化错误。
     */
    virtual Status initialize() = 0;

    /**
     * @brief 根据用户标识读取用户账户。
     * @param user_id 需要查询的用户唯一标识。
     * @return 成功时返回用户账户；失败时返回不存在或存储错误。
     */
    virtual Result<UserAccount> getUser(const std::string& user_id) = 0;

    /**
     * @brief 保存或更新用户账户信息。
     * @param user 待保存的用户账户完整信息。
     * @return 成功时表示用户已持久化；失败时返回存储错误。
     */
    virtual Status saveUser(const UserAccount& user) = 0;

    /**
     * @brief 根据 API Key 哈希值查找对应的 API Key 身份。
     * @param key_hash API Key 明文经过哈希后的值，用于安全查找。
     * @return 成功时返回 API Key 身份；失败时返回不存在或存储错误。
     */
    virtual Result<ApiKeyIdentity> getApiKeyByHash(const std::string& key_hash) = 0;

    /**
     * @brief 保存 API Key 身份及其哈希值。
     * @param identity API Key 对应的用户、启用状态和过期时间等身份信息。
     * @param key_hash API Key 明文的哈希值，作为后续查找键。
     * @return 成功时表示密钥已持久化；失败时返回存储错误。
     */
    virtual Status saveApiKey(const ApiKeyIdentity& identity, const std::string& key_hash) = 0;

    /**
     * @brief 根据账号标识读取上游账号。
     * @param account_id 需要查询的上游账号唯一标识。
     * @return 成功时返回上游账号；失败时返回不存在或存储错误。
     */
    virtual Result<UpstreamAccount> getUpstreamAccount(const std::string& account_id) = 0;

    /**
     * @brief 列出所有已配置的上游账号。
     * @return 成功时返回上游账号列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UpstreamAccount>> listUpstreamAccounts() = 0;

    /**
     * @brief 保存或更新上游账号配置。
     * @param account 待保存的上游账号完整信息。
     * @return 成功时表示账号已持久化；失败时返回存储错误。
     */
    virtual Status saveUpstreamAccount(const UpstreamAccount& account) = 0;

    /**
     * @brief 根据公共模型名和能力查询模型映射。
     * @param public_model_name 客户端请求使用的公共模型名。
     * @param capability 需要查询的能力类型。
     * @return 成功时返回模型映射；失败时返回不存在或存储错误。
     */
    virtual Result<ModelMapping> getModelMapping(const std::string& public_model_name, Capability capability) = 0;

    /**
     * @brief 列出所有已配置的模型映射。
     * @return 成功时返回模型映射列表；失败时返回存储错误。
     */
    virtual Result<std::vector<ModelMapping>> listModelMappings() = 0;

    /**
     * @brief 保存或更新模型映射配置。
     * @param mapping 待保存的模型映射完整信息。
     * @return 成功时表示映射已持久化；失败时返回存储错误。
     */
    virtual Status saveModelMapping(const ModelMapping& mapping) = 0;

    /**
     * @brief 保存一次请求的用量审计记录。
     * @param record 完整的用量记录，包含身份、模型、上游、用量、成本和状态。
     * @return 成功时表示记录已持久化；失败时返回存储错误。
     */
    virtual Status saveUsageRecord(const UsageRecord& record) = 0;
};

}
