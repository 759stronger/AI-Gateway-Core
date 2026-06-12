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
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
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
     * @brief 根据 API Key 内部标识查找身份信息。
     * @param api_key_id API Key 的内部唯一标识。
     * @return 成功时返回 API Key 身份；失败时返回不存在或存储错误。
     */
    virtual Result<ApiKeyIdentity> getApiKeyById(const std::string& api_key_id) = 0;

    /**
     * @brief 保存 API Key 身份及其哈希值。
     * @param identity API Key 对应的用户、启用状态和过期时间等身份信息。
     * @param key_hash API Key 明文的哈希值，作为后续查找键。
     * @return 成功时表示密钥已持久化；失败时返回存储错误。
     */
    virtual Status saveApiKey(const ApiKeyIdentity& identity, const std::string& key_hash) = 0;

    /**
     * @brief 更新已有 API Key 的身份信息。
     * @param identity 包含 api_key_id 的完整新状态。
     * @return 成功时表示更新已持久化；失败时返回不存在或存储错误。
     */
    virtual Status updateApiKey(const ApiKeyIdentity& identity) = 0;

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

    /**
     * @brief 根据记录标识读取用量审计记录。
     * @param record_id 需要查询的审计记录唯一标识。
     * @return 成功时返回对应记录；失败时返回不存在或存储错误。
     */
    virtual Result<UsageRecord> getUsageRecord(const std::string& record_id) = 0;

    /**
     * @brief 列出全部用量审计记录。
     * @return 成功时返回记录列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UsageRecord>> listUsageRecords() = 0;
};

/**
 * @brief Storage 的内存版实现。
 *
 * 设计意图：
 * - 先用内存容器把主链路跑通，降低第一阶段实现难度。
 * - 这一版适合本地开发、单元测试和教学演示。
 * - 以后可以在不改上层接口的情况下替换成 SQLiteStorage 等持久化实现。
 */
class InMemoryStorage : public Storage {
public:
    InMemoryStorage() = default;

    /**
     * @brief 初始化内存存储。
     * @return 成功时将 initialized_ 置为 true；失败时返回初始化错误。
     */
    Status initialize() override;

    /// @brief 读取指定用户账户。
    Result<UserAccount> getUser(const std::string& user_id) override;

    /// @brief 保存或覆盖指定用户账户。
    Status saveUser(const UserAccount& user) override;

    /// @brief 通过哈希值查找 API Key 身份。
    Result<ApiKeyIdentity> getApiKeyByHash(const std::string& key_hash) override;

    /// @brief 通过内部标识查找 API Key 身份。
    Result<ApiKeyIdentity> getApiKeyById(const std::string& api_key_id) override;

    /// @brief 保存新的 API Key 身份及其哈希。
    Status saveApiKey(const ApiKeyIdentity& identity, const std::string& key_hash) override;

    /// @brief 更新已有 API Key 身份信息。
    Status updateApiKey(const ApiKeyIdentity& identity) override;

    /// @brief 读取指定上游账号。
    Result<UpstreamAccount> getUpstreamAccount(const std::string& account_id) override;

    /// @brief 列出全部上游账号。
    Result<std::vector<UpstreamAccount>> listUpstreamAccounts() override;

    /// @brief 保存或覆盖上游账号。
    Status saveUpstreamAccount(const UpstreamAccount& account) override;

    /// @brief 通过公共模型名和能力查询模型映射。
    Result<ModelMapping> getModelMapping(const std::string& public_model_name, Capability capability) override;

    /// @brief 列出全部模型映射。
    Result<std::vector<ModelMapping>> listModelMappings() override;

    /// @brief 保存或覆盖模型映射。
    Status saveModelMapping(const ModelMapping& mapping) override;

    /// @brief 保存一条用量审计记录。
    Status saveUsageRecord(const UsageRecord& record) override;

    /// @brief 读取指定用量审计记录。
    Result<UsageRecord> getUsageRecord(const std::string& record_id) override;

    /// @brief 列出全部用量审计记录。
    Result<std::vector<UsageRecord>> listUsageRecords() override;

private:
    /**
     * @brief 检查存储是否已经初始化。
     * @return 成功时表示可以安全访问内部容器；失败时返回 NotInitialized。
     */
    Status ensureInitialized() const;

    /**
     * @brief 为模型映射生成唯一查找键。
     * @param public_model_name 对外暴露的公共模型名。
     * @param capability 该映射对应的能力类型。
     * @return 组合后的稳定键值，供 model_mappings_ 使用。
     */
    std::string makeModelMappingKey(const std::string& public_model_name, Capability capability) const;

    /// @brief 标记当前存储是否已经完成 initialize。
    bool initialized_ = false;

    /// @brief 用户账户表，key 为 user_id。
    std::unordered_map<std::string, UserAccount> users_;

    /// @brief API Key 表，key 为 key_hash。
    std::unordered_map<std::string, ApiKeyIdentity> api_keys_by_hash_;

    /// @brief API Key 反向索引，key 为 api_key_id，value 为 key_hash。
    std::unordered_map<std::string, std::string> api_key_hash_by_id_;

    /// @brief 上游账号表，key 为 account_id。
    std::unordered_map<std::string, UpstreamAccount> upstream_accounts_;

    /// @brief 模型映射表，key 为公共模型名与能力组合键。
    std::unordered_map<std::string, ModelMapping> model_mappings_;

    /// @brief 用量审计记录表，key 为 record_id。
    std::unordered_map<std::string, UsageRecord> usage_records_;

    /// @brief 保护内部容器的互斥锁。
    mutable std::mutex mutex_;
};

/**
 * @brief 创建一个默认的内存版 Storage。
 * @return 指向 InMemoryStorage 的多态智能指针。
 */
std::shared_ptr<Storage> createInMemoryStorage();

}
