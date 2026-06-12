/**
 * @file user_manager.h
 * @brief 定义用户账户状态、余额和额度管理接口。
 *
 * 该文件封装用户维度的访问控制和额度扣减逻辑。网关在认证 API Key 后，
 * 通常会通过 UserManager 检查用户是否可继续请求，并在请求完成后扣减费用或额度。
 *
 * 与其他文件的联系：
 * - auth/auth_manager.h 的 ApiKeyIdentity 通过 user_id 关联到 UserAccount。
 * - billing/billing_policy.h 计算费用后可调用 deductQuota 扣减用户额度。
 * - storage/storage.h 负责用户账户数据的读取和保存。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include <ctime>
#include <memory>
#include <string>

namespace ai_gateway_core {

class Storage;

/**
 * @brief 用户账户当前状态。
 */
enum class UserStatus {
    Active,         // 用户正常可用，可以发起请求。
    Disabled,       // 用户已被禁用，不能继续使用网关服务。
    QuotaExceeded,  // 用户额度已用尽，需要充值、重置额度或等待额度恢复。
    Expired         // 用户账号已过期，不能继续发起请求。
};

/**
 * @brief 网关用户账户信息。
 *
 * 字段说明：
 * - user_id：用户唯一标识。
 * - username：用户展示名或登录名。
 * - status：用户当前状态，决定是否允许请求。
 * - balance：账户余额，可用于费用扣减。
 * - quota_limit：额度上限。
 * - used_quota：已使用额度。
 * - created_at：账户创建时间戳。
 * - updated_at：账户最后更新时间戳。
 */
struct UserAccount {
    std::string user_id;
    std::string username;
    UserStatus status = UserStatus::Active;
    double balance = 0.0;
    double quota_limit = 0.0;
    double used_quota = 0.0;
    std::time_t created_at = 0;
    std::time_t updated_at = 0;
};

/**
 * @brief 用户账户管理接口。
 */
class UserManager {
public:
    virtual ~UserManager() = default;

    /**
     * @brief 获取指定用户账户信息。
     * @param user_id 需要查询的用户唯一标识。
     * @return 成功时返回用户账户；失败时返回不存在或存储错误。
     */
    virtual Result<UserAccount> getUser(const std::string& user_id) = 0;

    /**
     * @brief 检查指定用户是否可以发起模型请求。
     * @param user_id 需要校验的用户唯一标识。
     * @return 成功时表示允许请求；失败时返回禁用、过期或额度不足等错误。
     */
    virtual Status ensureUserCanRequest(const std::string& user_id) = 0;

    /**
     * @brief 从用户账户扣减指定额度或费用。
     * @param user_id 需要扣减的用户唯一标识。
     * @param amount 扣减数量，通常来自计费模块计算结果。
     * @return 成功时表示扣减已持久化；失败时返回余额不足或存储错误。
     */
    virtual Status deductQuota(const std::string& user_id, double amount) = 0;
};

/**
 * @brief UserManager 的默认实现。
 *
 * 设计意图：
 * - 负责用户维度的请求可用性校验和额度扣减。
 * - 把“认证后还能不能继续请求”的业务规则集中在这一层。
 * - 具体账户数据仍然存放在 Storage 中。
 */
class DefaultUserManager : public UserManager {
public:
    /**
     * @brief 创建一个默认用户管理器。
     * @param storage 用于读取和保存用户账户的存储实现。
     */
    explicit DefaultUserManager(std::shared_ptr<Storage> storage);

    /// @brief 获取指定用户账户。
    Result<UserAccount> getUser(const std::string& user_id) override;

    /// @brief 检查指定用户当前是否允许发起请求。
    Status ensureUserCanRequest(const std::string& user_id) override;

    /// @brief 从用户账户扣减额度或余额。
    Status deductQuota(const std::string& user_id, double amount) override;

private:
    /**
     * @brief 校验扣费参数是否合法。
     * @param user_id 需要扣费的用户标识。
     * @param amount 计划扣减的金额或额度。
     * @return 成功时表示参数可继续处理；失败时返回 InvalidArgument。
     */
    Status validateQuotaArguments(const std::string& user_id, double amount) const;

    /**
     * @brief 校验用户当前状态是否允许请求。
     * @param user 需要检查的用户账户对象。
     * @return 成功时表示用户状态允许请求；失败时返回禁用、过期或额度错误。
     */
    Status validateUserStatus(const UserAccount& user) const;

    /// @brief 用于读取和更新用户账户的存储实现。
    std::shared_ptr<Storage> storage_;
};

}
