/**
 * @file result.cpp
 * @brief 实现统一结果类型的非模板辅助函数。
 *
 * 本文件负责实现 ErrorCode 到字符串的转换，以及 Status 成功/失败结果构造函数。
 * Result<T> 是模板类型，相关构造辅助函数必须放在 result.h 中，才能让调用方在编译期实例化。
 */
#include "ai_gateway_core/core/result.h"

#include <utility>

namespace ai_gateway_core {

const char* toString(ErrorCode code) {
    switch (code) {
    case ErrorCode::Ok:
        return "Ok";
    case ErrorCode::InvalidArgument:
        return "InvalidArgument";
    case ErrorCode::Unauthorized:
        return "Unauthorized";
    case ErrorCode::Forbidden:
        return "Forbidden";
    case ErrorCode::QuotaExceeded:
        return "QuotaExceeded";
    case ErrorCode::RateLimited:
        return "RateLimited";
    case ErrorCode::NotFound:
        return "NotFound";
    case ErrorCode::NotInitialized:
        return "NotInitialized";
    case ErrorCode::ProviderUnavailable:
        return "ProviderUnavailable";
    case ErrorCode::UpstreamUnavailable:
        return "UpstreamUnavailable";
    case ErrorCode::NetworkError:
        return "NetworkError";
    case ErrorCode::Timeout:
        return "Timeout";
    case ErrorCode::ResponseParseError:
        return "ResponseParseError";
    case ErrorCode::StorageError:
        return "StorageError";
    case ErrorCode::InternalError:
        return "InternalError";
    }

    return "InternalError";
}

Status successStatus() {
    return {true, {ErrorCode::Ok, {}}};
}

Status failureStatus(Error error) {
    return {false, std::move(error)};
}

}
