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

namespace ai_gateway_core {

/**
 * @brief 网关内部通用错误码。
 */
enum class ErrorCode {
    Ok,
    InvalidArgument,
    Unauthorized,
    Forbidden,
    QuotaExceeded,
    RateLimited,
    NotFound,
    NotInitialized,
    ProviderUnavailable,
    UpstreamUnavailable,
    NetworkError,
    Timeout,
    ResponseParseError,
    StorageError,
    InternalError
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

}
