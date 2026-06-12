/**
 * @file health_checker.h
 * @brief 定义上游账号健康检查快照和健康检查接口。
 *
 * 该文件用于探测和保存上游账号的可用性、延迟、错误和检查时间。路由模块可以基于
 * 健康状态规避不可用账号，管理后台也可以通过快照展示各通道状态。
 *
 * 与其他文件的联系：
 * - upstream/upstream_account.h 提供 UpstreamAccount 和 UpstreamStatus。
 * - routing/model_router.h 可参考健康快照选择低延迟或可用上游。
 * - admin/admin_api.h 可列出 HealthSnapshot 供运维查看。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include <ctime>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ai_gateway_core {

class ProviderManager;
class UpstreamPool;

/**
 * @brief 单个上游账号的健康状态快照。
 *
 * 字段说明：
 * - upstream_account_id：被检查的上游账号唯一标识。
 * - status：检查后得到的账号状态。
 * - latency_ms：最近一次健康检查延迟毫秒数。
 * - availability_rate：可用率，通常由历史成功率计算得到。
 * - last_error：最近一次健康检查或请求失败的错误信息。
 * - checked_at：快照生成或最后检查时间戳。
 */
struct HealthSnapshot {
    std::string upstream_account_id;
    UpstreamStatus status = UpstreamStatus::Unknown;
    int latency_ms = 0;
    double availability_rate = 0.0;
    std::string last_error;
    std::time_t checked_at = 0;
};

/**
 * @brief 上游健康检查接口。
 */
class HealthChecker {
public:
    virtual ~HealthChecker() = default;

    /**
     * @brief 检查指定上游账号的健康状态。
     * @param account 需要检查的上游账号，包含供应商、地址和认证信息。
     * @return 成功时表示检查已完成；失败时返回检查过程错误。
     */
    virtual Status checkAccount(const UpstreamAccount& account) = 0;

    /**
     * @brief 检查所有已配置上游账号。
     * @return 成功时表示全部账号检查流程已完成；失败时返回整体调度或存储错误。
     */
    virtual Status checkAll() = 0;

    /**
     * @brief 查询指定上游账号的健康快照。
     * @param upstream_account_id 需要查询的上游账号唯一标识。
     * @return 成功时返回健康快照；失败时返回不存在或存储错误。
     */
    virtual Result<HealthSnapshot> getSnapshot(const std::string& upstream_account_id) = 0;

    /**
     * @brief 列出所有上游账号的健康快照。
     * @return 成功时返回健康快照列表；失败时返回存储错误。
     */
    virtual Result<std::vector<HealthSnapshot>> listSnapshots() = 0;
};

/**
 * @brief 默认健康检查实现。
 *
 * 设计意图：
 * - 统一驱动所有上游账号的健康探测流程。
 * - 保存最近一次健康快照，供路由和管理后台复用。
 */
class BasicHealthChecker : public HealthChecker {
public:
    /**
     * @brief 创建默认健康检查器。
     * @param upstream_pool 提供上游账号列表和状态更新能力。
     * @param provider_manager 提供对应供应商实现，用于实际探测。
     */
    BasicHealthChecker(std::shared_ptr<UpstreamPool> upstream_pool,
                       std::shared_ptr<ProviderManager> provider_manager);

    /// @brief 对单个账号执行健康检查。
    Status checkAccount(const UpstreamAccount& account) override;
    /// @brief 对所有账号执行健康检查。
    Status checkAll() override;
    /// @brief 读取指定账号的健康快照。
    Result<HealthSnapshot> getSnapshot(const std::string& upstream_account_id) override;
    /// @brief 列出所有健康快照。
    Result<std::vector<HealthSnapshot>> listSnapshots() override;

private:
    /// @brief 组装一份健康快照对象。
    HealthSnapshot makeSnapshot(const UpstreamAccount& account,
                                UpstreamStatus status,
                                int latency_ms,
                                const std::string& last_error) const;

    /// @brief 上游账号池依赖。
    std::shared_ptr<UpstreamPool> upstream_pool_;
    /// @brief Provider 注册表依赖。
    std::shared_ptr<ProviderManager> provider_manager_;
    /// @brief 最近一次检查结果快照表，key 为 upstream_account_id。
    std::unordered_map<std::string, HealthSnapshot> snapshots_;
};

}
