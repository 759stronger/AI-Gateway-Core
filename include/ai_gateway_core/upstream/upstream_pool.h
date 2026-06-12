/**
 * @file upstream_pool.h
 * @brief 定义上游账号池的管理、筛选和运行状态更新接口。
 *
 * 该文件把上游账号的增删改查、候选账号筛选、成功失败统计和并发占用封装为统一接口。
 * 路由模块通常先通过 UpstreamPool 获取候选账号，再根据策略选择最终账号。
 *
 * 与其他文件的联系：
 * - upstream/upstream_account.h 提供账号数据结构和状态枚举。
 * - routing/model_router.h 可调用 findCandidates 获取路由候选。
 * - health/health_checker.h 可通过 markSuccess/markFailure 更新健康相关统计。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace ai_gateway_core {

class Storage;

/**
 * @brief 上游账号池接口。
 */
class UpstreamPool {
public:
    virtual ~UpstreamPool() = default;

    /**
     * @brief 添加新的上游账号。
     * @param account 待添加的账号配置和初始状态。
     * @return 成功时表示账号已进入账号池；失败时返回重复或存储错误。
     */
    virtual Status addAccount(const UpstreamAccount& account) = 0;

    /**
     * @brief 更新已有上游账号。
     * @param account 包含 account_id 的完整账号新配置。
     * @return 成功时表示账号配置已更新；失败时返回不存在或存储错误。
     */
    virtual Status updateAccount(const UpstreamAccount& account) = 0;

    /**
     * @brief 禁用指定上游账号，使其不再参与路由。
     * @param account_id 需要禁用的上游账号唯一标识。
     * @return 成功时表示账号已禁用；失败时返回不存在或存储错误。
     */
    virtual Status disableAccount(const std::string& account_id) = 0;

    /**
     * @brief 从账号池中移除指定账号。
     * @param account_id 需要移除的上游账号唯一标识。
     * @return 成功时表示账号已被删除；失败时返回不存在或存储错误。
     */
    virtual Status removeAccount(const std::string& account_id) = 0;

    /**
     * @brief 获取指定上游账号。
     * @param account_id 需要查询的上游账号唯一标识。
     * @return 成功时返回账号信息；失败时返回不存在或存储错误。
     */
    virtual Result<UpstreamAccount> getAccount(const std::string& account_id) = 0;

    /**
     * @brief 列出账号池中的全部上游账号。
     * @return 成功时返回账号列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UpstreamAccount>> listAccounts() = 0;

    /**
     * @brief 根据模型映射筛选可用候选账号。
     * @param mapping 已命中的模型映射，包含供应商、真实模型名和能力信息。
     * @return 成功时返回满足模型、能力、启用状态和健康状态的候选账号列表。
     */
    virtual Result<std::vector<UpstreamAccount>> findCandidates(const ModelMapping& mapping) = 0;

    /**
     * @brief 标记指定账号一次请求成功。
     * @param account_id 成功完成请求的上游账号唯一标识。
     * @param latency_ms 本次请求延迟毫秒数，用于更新平均延迟和健康统计。
     * @return 成功时表示统计已更新；失败时返回不存在或存储错误。
     */
    virtual Status markSuccess(const std::string& account_id, int latency_ms) = 0;

    /**
     * @brief 标记指定账号一次请求失败。
     * @param account_id 请求失败的上游账号唯一标识。
     * @param error 本次失败的内部错误详情。
     * @return 成功时表示失败统计和最近错误已更新；失败时返回不存在或存储错误。
     */
    virtual Status markFailure(const std::string& account_id, const Error& error) = 0;

    /**
     * @brief 占用指定账号的一个并发请求槽位。
     * @param account_id 需要占用并发的上游账号唯一标识。
     * @return 成功时表示并发计数已增加；失败时返回并发已满或账号不可用。
     */
    virtual Status acquireConcurrency(const std::string& account_id) = 0;

    /**
     * @brief 释放指定账号的一个并发请求槽位。
     * @param account_id 需要释放并发的上游账号唯一标识。
     * @return 成功时表示并发计数已减少；失败时返回不存在或状态不一致错误。
     */
    virtual Status releaseConcurrency(const std::string& account_id) = 0;
};

/**
 * @brief UpstreamPool 的内存版实现。
 *
 * 设计意图：
 * - 统一管理上游账号的启用状态、并发占用和简单统计信息。
 * - 先用内存结构支持路由和健康检查流程，后面可替换为持久化实现。
 */
class InMemoryUpstreamPool : public UpstreamPool {
public:
    /**
     * @brief 创建一个内存版上游账号池。
     * @param storage 可选的存储实现；如果提供，则更新账号状态时同步保存。
     */
    explicit InMemoryUpstreamPool(std::shared_ptr<Storage> storage = nullptr);

    /// @brief 添加新的上游账号到池中。
    Status addAccount(const UpstreamAccount& account) override;
    /// @brief 更新已有上游账号配置。
    Status updateAccount(const UpstreamAccount& account) override;
    /// @brief 禁用指定上游账号。
    Status disableAccount(const std::string& account_id) override;
    /// @brief 从池中移除指定上游账号。
    Status removeAccount(const std::string& account_id) override;
    /// @brief 读取指定上游账号。
    Result<UpstreamAccount> getAccount(const std::string& account_id) override;
    /// @brief 列出全部上游账号。
    Result<std::vector<UpstreamAccount>> listAccounts() override;
    /// @brief 根据模型映射筛选候选账号。
    Result<std::vector<UpstreamAccount>> findCandidates(const ModelMapping& mapping) override;
    /// @brief 记录指定上游账号一次成功调用。
    Status markSuccess(const std::string& account_id, int latency_ms) override;
    /// @brief 记录指定上游账号一次失败调用。
    Status markFailure(const std::string& account_id, const Error& error) override;
    /// @brief 占用一个并发槽位。
    Status acquireConcurrency(const std::string& account_id) override;
    /// @brief 释放一个并发槽位。
    Status releaseConcurrency(const std::string& account_id) override;

private:
    /**
     * @brief 如果配置了存储，则将账号状态同步保存到 Storage。
     * @param account 需要同步的完整账号对象。
     * @return 成功时表示已同步或无需同步；失败时返回存储错误。
     */
    Status saveToStorageIfPresent(const UpstreamAccount& account);

    /**
     * @brief 判断账号是否支持某条模型映射所要求的真实模型。
     * @param account 待检查的上游账号。
     * @param mapping 命中的模型映射。
     * @return 支持返回 true，否则返回 false。
     */
    bool supportsModel(const UpstreamAccount& account, const ModelMapping& mapping) const;

    /**
     * @brief 判断账号是否支持指定能力。
     * @param account 待检查的上游账号。
     * @param capability 本次请求所需的能力。
     * @return 支持返回 true，否则返回 false。
     */
    bool supportsCapability(const UpstreamAccount& account, Capability capability) const;

    /**
     * @brief 判断账号当前是否适合参与路由。
     * @param account 待检查的上游账号。
     * @return 可路由返回 true，否则返回 false。
     */
    bool isRoutable(const UpstreamAccount& account) const;

    /// @brief 可选的存储实现，用于同步账号配置和状态。
    std::shared_ptr<Storage> storage_;
    /// @brief 上游账号池主体，key 为 account_id。
    std::unordered_map<std::string, UpstreamAccount> accounts_;
    /// @brief 保护账号池状态的互斥锁。
    mutable std::mutex mutex_;
};

}
