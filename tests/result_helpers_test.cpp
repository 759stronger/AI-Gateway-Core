/**
 * @file result_helpers_test.cpp
 * @brief 第一课：验证 Result<T>、Status 和 ErrorCode 辅助函数。
 *
 * 本测试演示三件事：
 * - 成功时 Result<T> 如何携带业务值。
 * - 失败时 Result<T> 和 Status 如何携带 Error。
 * - ErrorCode 如何转换为稳定字符串，便于日志和调试。
 */
#include "ai_gateway_core/core/result.h"

#include <cassert>
#include <string>

int main() {
    using namespace ai_gateway_core;

    auto success = successResult<std::string>("hello gateway");
    assert(success.ok);
    assert(success.value == "hello gateway");
    assert(success.error.code == ErrorCode::Ok);

    auto failed = failureResult<int>({ErrorCode::InvalidArgument, "model is required"});
    assert(!failed.ok);
    assert(failed.error.code == ErrorCode::InvalidArgument);
    assert(failed.error.message == "model is required");

    auto status = successStatus();
    assert(status.ok);
    assert(status.error.code == ErrorCode::Ok);

    auto failed_status = failureStatus({ErrorCode::Unauthorized, "missing api key"});
    assert(!failed_status.ok);
    assert(failed_status.error.code == ErrorCode::Unauthorized);
    assert(toString(failed_status.error.code) == std::string("Unauthorized"));

    return 0;
}
