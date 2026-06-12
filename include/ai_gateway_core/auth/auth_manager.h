/**
 * @file auth_manager.h
 * @brief 定义 API Key 认证与密钥生命周期管理接口。
 *
 * 该文件负责把外部 Authorization 头转换为内部身份信息，并提供创建、撤销和更新
 * API Key 使用状态的抽象能力。认证成功后的身份会进入 GatewayContext，供限流、计费和用量记录使用。
 *
 * 与其他文件的联系：
 * - storage/storage.h 负责持久化 ApiKeyIdentity 和密钥哈希。
 * - core/types.h 的 GatewayContext 会保存 api_key_id 和 user_id。
 * - users/user_manager.h 可基于 user_id 继续校验用户状态和额度。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include <ctime>
#include <memory>
#include <string>

namespace ai_gateway_core {

class Storage;

/**
 * @brief API Key 对应的内部身份信息。
 *
 * 字段说明：
 * - api_key_id：API Key 的内部唯一标识。
 * - user_id：API Key 归属用户标识。
 * - name：API Key 的展示名称或用途说明。
 * - enabled：该 API Key 是否可用于认证。
 * - expires_at：过期时间戳，值为 0 时通常表示不过期。
 */
struct ApiKeyIdentity {
    std::string api_key_id;
    std::string user_id;
    std::string name;
    bool enabled = false;
    std::time_t expires_at = 0;
    std::time_t created_at = 0;
    std::time_t last_used_at = 0;
};

/**
 * @brief API Key 认证管理接口。
 */
class AuthManager {
public:
    virtual ~AuthManager() = default;

    /**
     * @brief 根据 Bearer Authorization 头认证请求方身份。
     * @param authorization_header HTTP Authorization 头内容，通常格式为 Bearer <token>。
     * @return 成功时返回 API Key 身份；失败时返回未授权、过期或禁用等错误。
     */
    virtual Result<ApiKeyIdentity> authenticateBearerToken(const std::string& authorization_header) = 0;

    /**
     * @brief 为指定用户创建新的 API Key。
     * @param user_id API Key 归属的用户标识。
     * @param name API Key 的展示名称或用途说明。
     * @return 成功时返回明文 API Key；实现方应只持久化其哈希值。
     */
    virtual Result<std::string> createApiKey(const std::string& user_id, const std::string& name) = 0;

    /**
     * @brief 撤销指定 API Key，使其无法继续认证。
     * @param api_key_id 需要撤销的 API Key 内部标识。
     * @return 成功时表示密钥已被禁用或删除；失败时返回不存在或存储错误。
     */
    virtual Status revokeApiKey(const std::string& api_key_id) = 0;

    /**
     * @brief 更新 API Key 的最近使用状态。
     * @param api_key_id 已认证或已使用的 API Key 内部标识。
     * @return 成功时表示使用痕迹已记录；失败时返回存储错误。
     */
    virtual Status touchApiKey(const std::string& api_key_id) = 0;
};

/**
 * @brief AuthManager 的默认实现。
 *
 * 设计意图：
 * - 这一层只处理认证规则，不负责底层存储细节。
 * - Bearer 头解析、API Key 生成、哈希和状态校验都放在这里。
 * - 实际的数据读写委托给 Storage，便于以后替换为数据库实现。
 */
class DefaultAuthManager : public AuthManager {
public:
    /**
     * @brief 创建一个默认认证管理器。
     * @param storage 用于查询和更新 API Key 数据的存储实现。
     */
    explicit DefaultAuthManager(std::shared_ptr<Storage> storage);

    /**
     * @brief 认证 Bearer Token 并返回内部身份。
     * @param authorization_header HTTP Authorization 请求头。
     * @return 成功时返回对应的 ApiKeyIdentity；失败时返回鉴权错误。
     */
    Result<ApiKeyIdentity> authenticateBearerToken(const std::string& authorization_header) override;

    /**
     * @brief 为某个用户创建一把新的 API Key。
     * @param user_id API Key 归属的用户标识。
     * @param name 这把 API Key 的用途说明或展示名称。
     * @return 成功时返回一次性的明文 API Key。
     */
    Result<std::string> createApiKey(const std::string& user_id, const std::string& name) override;

    /**
     * @brief 撤销指定 API Key。
     * @param api_key_id 需要撤销的 API Key 内部标识。
     * @return 成功时表示撤销完成；失败时返回不存在或存储错误。
     */
    Status revokeApiKey(const std::string& api_key_id) override;

    /**
     * @brief 更新 API Key 的最近使用时间。
     * @param api_key_id 已使用的 API Key 内部标识。
     * @return 成功时表示更新完成；失败时返回不存在或存储错误。
     */
    Status touchApiKey(const std::string& api_key_id) override;

private:
    /**
     * @brief 从 Authorization 头中提取 Bearer Token。
     * @param authorization_header 原始请求头内容。
     * @return 成功时返回明文 token；失败时返回参数或未认证错误。
     */
    Result<std::string> parseBearerToken(const std::string& authorization_header) const;

    /**
     * @brief 计算 API Key 的稳定哈希值。
     * @param api_key 明文 API Key。
     * @return 可用于存储和查找的哈希字符串。
     */
    std::string hashApiKey(const std::string& api_key) const;

    /**
     * @brief 生成新的 API Key 内部标识。
     * @return 新的 api_key_id。
     */
    std::string generateApiKeyId() const;

    /**
     * @brief 生成返回给用户的一次性明文 API Key。
     * @return 可直接复制使用的明文密钥。
     */
    std::string generatePlaintextApiKey() const;

    /**
     * @brief 获取当前时间戳。
     * @return 当前时间对应的 std::time_t。
     */
    std::time_t now() const;

    /// @brief 用于读取和持久化 API Key 数据的存储实现。
    std::shared_ptr<Storage> storage_;
};

}
