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
#include <string>
#include <vector>
#include <ctime>

namespace ai_gateway_core {

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

}
