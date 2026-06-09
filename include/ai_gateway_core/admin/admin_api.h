/**
 * @file admin_api.h
 * @brief 定义网关管理后台的查询和管理接口。
 *
 * 该文件提供管理端需要的 CRUD 操作，覆盖上游账号、模型映射、通道状态和用量记录。
 * 运维人员可通过该接口在运行时增删改查网关配置和查看运行状态，而不需要直接操作存储层。
 *
 * 与其他文件的联系：
 * - upstream/upstream_account.h 和 upstream/upstream_pool.h 提供上游账号数据结构和管理能力。
 * - routing/model_mapping.h 提供模型映射数据结构。
 * - health/health_checker.h 提供 HealthSnapshot 表达通道健康状态。
 * - usage/usage_recorder.h 提供 UsageRecord 表达用量审计记录。
 * - 这些接口的实现通常委托给 storage/storage.h、upstream/upstream_pool.h、health/health_checker.h
 *   和 usage/usage_recorder.h 的对应方法。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/health/health_checker.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include "ai_gateway_core/usage/usage_recorder.h"
#include <string>
#include <vector>

namespace ai_gateway_core {

/**
 * @brief 管理后台接口。
 */
class AdminAPI {
public:
    virtual ~AdminAPI() = default;

    /**
     * @brief 列出所有已配置的上游账号。
     * @return 成功时返回上游账号列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UpstreamAccount>> listUpstreams() = 0;

    /**
     * @brief 创建新的上游账号。
     * @param account 待创建的账号完整配置，account_id 需唯一。
     * @return 成功时表示账号已创建并进入账号池；失败时返回重复、参数非法或存储错误。
     */
    virtual Status createUpstream(const UpstreamAccount& account) = 0;

    /**
     * @brief 更新已有上游账号。
     * @param account 包含 account_id 的完整新配置。
     * @return 成功时表示账号已更新；失败时返回不存在、参数非法或存储错误。
     */
    virtual Status updateUpstream(const UpstreamAccount& account) = 0;

    /**
     * @brief 删除指定上游账号。
     * @param account_id 需要删除的上游账号唯一标识。
     * @return 成功时表示账号已从账号池移除；失败时返回不存在或存储错误。
     */
    virtual Status deleteUpstream(const std::string& account_id) = 0;

    /**
     * @brief 列出所有已配置的模型映射。
     * @return 成功时返回模型映射列表；失败时返回存储错误。
     */
    virtual Result<std::vector<ModelMapping>> listModelMappings() = 0;

    /**
     * @brief 创建新的模型映射。
     * @param mapping 待创建的映射完整配置，mapping_id 需唯一。
     * @return 成功时表示映射已创建并可被路由使用；失败时返回重复、参数非法或存储错误。
     */
    virtual Status createModelMapping(const ModelMapping& mapping) = 0;

    /**
     * @brief 更新已有模型映射。
     * @param mapping 包含 mapping_id 的完整新映射配置。
     * @return 成功时表示映射已更新；失败时返回不存在或存储错误。
     */
    virtual Status updateModelMapping(const ModelMapping& mapping) = 0;

    /**
     * @brief 列出所有上游账号的当前健康快照。
     * @return 成功时返回通道状态列表；失败时返回存储错误。
     */
    virtual Result<std::vector<HealthSnapshot>> listChannelStatus() = 0;

    /**
     * @brief 列出所有用量审计记录。
     * @return 成功时返回用量记录列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UsageRecord>> listUsageRecords() = 0;
};

}
