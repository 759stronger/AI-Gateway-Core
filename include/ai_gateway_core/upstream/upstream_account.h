/**
 * @file upstream_account.h
 * @brief 定义上游供应商账号、账号类型和运行状态。
 *
 * 该文件描述网关可调用的供应商账号或本地端点，包括认证信息、支持模型、能力、
 * 并发限制、速率限制、配额和健康统计。路由模块会基于这些信息选择可用账号。
 *
 * 与其他文件的联系：
 * - routing/model_router.h 和 upstream/upstream_pool.h 使用 UpstreamAccount 作为路由目标。
 * - providers/llm_provider.h 和 images/image_gateway.h 使用账号信息调用真实供应商。
 * - health/health_checker.h 使用 UpstreamStatus 和统计字段记录健康状态。
 */
#pragma once

#include "ai_gateway_core/core/types.h"
#include <string>
#include <vector>
#include <ctime>

namespace ai_gateway_core {

/**
 * @brief 上游账号认证或连接类型。
 */
enum class UpstreamAccountType {
    ApiKey,
    OAuth,
    Session,
    LocalEndpoint
};

/**
 * @brief 上游账号当前运行状态。
 */
enum class UpstreamStatus {
    Available,
    Disabled,
    RateLimited,
    Exhausted,
    Unhealthy,
    CoolingDown,
    Unknown
};

/**
 * @brief 可被网关路由到的上游账号或端点。
 *
 * 字段说明：
 * - account_id：上游账号唯一标识。
 * - name：账号展示名称。
 * - provider_name：供应商名称，关联 providers/provider_manager.h 中注册的 Provider。
 * - account_type：账号认证或连接方式。
 * - base_url：供应商 API 或本地端点基础地址。
 * - encrypted_api_key：加密存储的上游 API Key。
 * - supported_models：该账号支持的上游模型名列表。
 * - capabilities：该账号支持的能力集合。
 * - enabled：账号是否允许参与路由。
 * - status：账号当前健康和可用状态。
 * - priority：优先级，供 PriorityFirst 等策略使用。
 * - weight：权重，供 Weighted 等策略使用。
 * - max_concurrency：最大并发请求数。
 * - current_concurrency：当前占用并发数。
 * - rpm_limit：每分钟请求数限制。
 * - tpm_limit：每分钟 Token 数限制。
 * - daily_quota：每日可用额度或预算。
 * - used_quota：当日已用额度或预算。
 * - success_count：累计成功请求数。
 * - failure_count：累计失败请求数。
 * - average_latency_ms：平均请求延迟毫秒数。
 * - last_error：最近一次失败错误信息。
 * - last_checked_at：最近一次健康检查时间戳。
 * - last_used_at：最近一次被请求使用的时间戳。
 */
struct UpstreamAccount {
    std::string account_id;
    std::string name;
    std::string provider_name;
    UpstreamAccountType account_type = UpstreamAccountType::ApiKey;
    std::string base_url;
    std::string encrypted_api_key;
    std::vector<std::string> supported_models;
    std::vector<Capability> capabilities;
    bool enabled = false;
    UpstreamStatus status = UpstreamStatus::Unknown;
    int priority = 0;
    int weight = 1;
    int max_concurrency = 1;
    int current_concurrency = 0;
    int rpm_limit = 0;
    int tpm_limit = 0;
    double daily_quota = 0.0;
    double used_quota = 0.0;
    int64_t success_count = 0;
    int64_t failure_count = 0;
    int average_latency_ms = 0;
    std::string last_error;
    std::time_t last_checked_at = 0;
    std::time_t last_used_at = 0;
};

}
