/**
 * @file result.h
 * @brief 定义统一错误码、错误对象和函数返回封装。
 *
 * 该文件为所有接口提供一致的返回模型：带业务值的函数使用 Result<T>，
 * 只表示成功或失败的函数使用 Status。这样调用方可以通过 ok 判断是否成功，
 * 并在失败时读取 error.code 和 error.message。
 *
 * 与其他文件的联系：
 * - 几乎所有 manager、router、provider、storage 接口都会返回 Result<T> 或 Status。
 * - routing/model_router.h 和 upstream/upstream_pool.h 会在失败路径中传递 Error。
 * - adapters/openai_adapter.h 可将 Error 转换为对客户端返回的错误响应。
 */
#pragma once

#include <string>
#include <utility>

namespace ai_gateway_core {

/**
 * @brief 网关内部通用错误码。
 */
enum class ErrorCode {
    Ok,                   // 操作成功，没有错误。
    InvalidArgument,      // 参数非法，例如必填字段为空、格式错误或取值超出范围。
    Unauthorized,         // 未认证，例如缺少 API Key、Token 无效或 Token 已过期。
    Forbidden,            // 已认证但无权限，例如用户没有访问某模型或管理接口的权限。
    QuotaExceeded,        // 用户、账号或组织额度已经用尽。
    RateLimited,          // 请求触发限流，需要等待后重试。
    NotFound,             // 请求的用户、模型、上游账号、记录等资源不存在。
    NotInitialized,       // 组件尚未初始化，例如服务、存储或配置未准备好。
    ProviderUnavailable,  // 模型供应商整体不可用，例如供应商服务宕机或未注册。
    UpstreamUnavailable,  // 具体上游账号或端点不可用，例如禁用、不健康或并发已满。
    NetworkError,         // 网络错误，例如连接失败、DNS 失败或 TLS 失败。
    Timeout,              // 请求超时，例如调用上游供应商超过时间限制。
    ResponseParseError,   // 响应解析失败，例如供应商返回 JSON 格式不符合预期。
    StorageError,         // 存储错误，例如数据库连接、查询或写入失败。
    InternalError         // 内部错误，用于无法归类的程序异常或逻辑错误。
};

/**
 * @brief 单次失败的错误详情。
 *
 * 字段说明：
 * - code：机器可读的错误类型，用于流程判断或映射 HTTP 状态码。
 * - message：面向日志、调试或客户端的可读错误说明。
 */
struct Error {
    ErrorCode code = ErrorCode::Ok;
    std::string message;
};

/**
 * @brief 带返回值的接口结果。
 *
 * 模板参数说明：
 * - T：成功时返回的业务值类型。
 *
 * 字段说明：
 * - ok：结果是否成功。
 * - value：成功时的业务值，失败时不应依赖该字段。
 * - error：失败时的错误详情。
 */
template <typename T>
struct Result {
    bool ok = false;
    T value{};
    Error error;
};

/**
 * @brief 只表示执行状态、不携带业务值的接口结果。
 *
 * 字段说明：
 * - ok：操作是否成功。
 * - error：失败时的错误详情。
 */
struct Status {
    bool ok = false;
    Error error;
};

/**
 * @brief 将错误码转换为稳定的英文标识字符串。
 * @param code 需要转换的错误码。
 * @return 对应错误码名称，便于日志、调试和协议错误映射。
 */
const char* toString(ErrorCode code);

/**
 * @brief 创建一个成功的带值结果。
 *
 * 模板参数说明：
 * - T：成功结果中携带的值类型。
 *
 * @param value 成功时返回给调用方的业务值。
 * @return ok 为 true 的 Result<T>。
 */
template <typename T>
Result<T> successResult(T value) {
    Result<T> result;
    result.ok = true;
    result.value = std::move(value);
    result.error = {ErrorCode::Ok, {}};
    return result;
}

/**
 * @brief 创建一个失败的带值结果。
 *
 * 模板参数说明：
 * - T：失败结果原本期望携带的业务值类型。
 *
 * @param error 失败原因，包含错误码和错误说明。
 * @return ok 为 false 的 Result<T>。
 */
template <typename T>
Result<T> failureResult(Error error) {
    Result<T> result;
    result.ok = false;
    result.error = std::move(error);
    return result;
}

/**
 * @brief 创建一个成功状态。
 * @return ok 为 true 的 Status。
 */
Status successStatus();

/**
 * @brief 创建一个失败状态。
 * @param error 失败原因，包含错误码和错误说明。
 * @return ok 为 false 的 Status。
 */
Status failureStatus(Error error);

}
