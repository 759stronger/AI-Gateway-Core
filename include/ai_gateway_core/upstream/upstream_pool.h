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
#include "ai_gateway_core/upstream/upstream_account.h"
#include <string>
#include <vector>

namespace ai_gateway_core {

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
     * @brief 根据公共模型和能力筛选可用候选账号。
     * @param public_model_name 客户端请求的公共模型名。
     * @param capability 本次请求需要的模型能力。
     * @return 成功时返回满足模型、能力、启用状态和健康状态的候选账号列表。
     */
    virtual Result<std::vector<UpstreamAccount>> findCandidates(const std::string& public_model_name, Capability capability) = 0;

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

}
