/**
 * @file usage_recorder.h
 * @brief 定义模型请求用量记录结构和写入查询接口。
 *
 * 该文件用于记录每次网关请求的审计数据，包括用户、API Key、公共模型、上游账号、
 * Token 用量、成本、延迟、状态和错误信息。该记录可用于账单、分析、排障和管理后台展示。
 *
 * 与其他文件的联系：
 * - core/types.h 提供 Capability、TokenUsage 和 Error。
 * - billing/billing_policy.h 计算出的 cost 会写入 UsageRecord。
 * - admin/admin_api.h 可列出 UsageRecord 供管理端查看。
 * - storage/storage.h 负责持久化 UsageRecord。
 */
#pragma once

#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/core/types.h"
#include <ctime>
#include <memory>
#include <string>
#include <vector>

namespace ai_gateway_core {

class Storage;

/**
 * @brief 一次请求最终执行状态。
 */
enum class RequestStatus {
    Success,    // 请求完整成功，模型调用、计费和记录流程都正常完成。
    Failed,     // 请求失败，没有产生可用的完整业务结果。
    Partial,    // 请求部分成功，例如流式响应中途失败但已经返回部分内容。
    Cancelled   // 请求被客户端、服务端或超时控制主动取消。
};

/**
 * @brief 单次模型或图片请求的用量审计记录。
 *
 * 字段说明：
 * - record_id：用量记录唯一标识。
 * - trace_id：链路追踪标识，关联 observability/observability.h 中的事件。
 * - user_id：请求用户标识。
 * - api_key_id：请求使用的 API Key 标识。
 * - public_model_name：客户端请求的公共模型名。
 * - upstream_account_id：实际调用的上游账号标识。
 * - upstream_model_name：实际调用的上游模型名。
 * - capability：本次请求使用的模型能力。
 * - usage：Token 用量统计。
 * - cost：本次请求最终成本。
 * - latency_ms：端到端请求延迟毫秒数。
 * - status：请求最终状态。
 * - error：失败或部分成功时的错误详情。
 * - stream：请求是否使用流式响应。
 * - created_at：记录创建时间戳。
 */
struct UsageRecord {
    std::string record_id;
    std::string trace_id;
    std::string user_id;
    std::string api_key_id;
    std::string public_model_name;
    std::string upstream_account_id;
    std::string upstream_model_name;
    Capability capability = Capability::ChatCompletion;
    TokenUsage usage;
    double cost = 0.0;
    int latency_ms = 0;
    RequestStatus status = RequestStatus::Success;
    Error error;
    bool stream = false;
    std::time_t created_at = 0;
};

/**
 * @brief 用量记录写入和查询接口。
 */
class UsageRecorder {
public:
    virtual ~UsageRecorder() = default;

    /**
     * @brief 保存一次请求用量记录。
     * @param record 待保存的完整用量记录，包含身份、模型、上游、用量、成本和状态信息。
     * @return 成功时表示记录已持久化；失败时返回存储错误。
     */
    virtual Status recordUsage(const UsageRecord& record) = 0;

    /**
     * @brief 根据记录标识查询用量记录。
     * @param record_id 需要查询的用量记录唯一标识。
     * @return 成功时返回用量记录；失败时返回不存在或存储错误。
     */
    virtual Result<UsageRecord> getRecord(const std::string& record_id) = 0;

    /**
     * @brief 列出全部用量记录。
     * @return 成功时返回完整记录列表；失败时返回存储错误。
     */
    virtual Result<std::vector<UsageRecord>> listRecords() = 0;
};

/**
 * @brief UsageRecorder 的默认实现。
 *
 * 设计意图：
 * - 把网关请求结束后的审计记录统一写入 Storage。
 * - 让上层只负责组织 UsageRecord，不关心底层保存方式。
 */
class DefaultUsageRecorder : public UsageRecorder {
public:
    /**
     * @brief 创建一个默认用量记录器。
     * @param storage 用于保存和查询用量记录的存储实现。
     */
    explicit DefaultUsageRecorder(std::shared_ptr<Storage> storage);

    /// @brief 保存一条用量记录。
    Status recordUsage(const UsageRecord& record) override;
    /// @brief 读取一条用量记录。
    Result<UsageRecord> getRecord(const std::string& record_id) override;
    /// @brief 列出全部用量记录。
    Result<std::vector<UsageRecord>> listRecords() override;

private:
    /// @brief 底层存储实现。
    std::shared_ptr<Storage> storage_;
};

}
