cmake_minimum_required(VERSION 3.10)

project(AIGatewayCore VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ai_gateway_core
    src/ai_gateway_core.cpp
    src/adapters/openai_adapter.cpp
    src/admin/admin_api.cpp
    src/auth/auth_manager.cpp
    src/billing/billing_policy.cpp
    src/config/config_manager.cpp
    src/core/result.cpp
    src/core/types.cpp
    src/gateway/gateway_server.cpp
    src/health/health_checker.cpp
    src/images/image_gateway.cpp
    src/limits/rate_limiter.cpp
    src/observability/observability.cpp
    src/providers/llm_provider.cpp
    src/providers/provider_manager.cpp
    src/routing/model_mapping.cpp
    src/routing/model_router.cpp
    src/storage/storage.cpp
    src/upstream/upstream_account.cpp
    src/upstream/upstream_pool.cpp
    src/usage/usage_recorder.cpp
    src/users/user_manager.cpp
)

target_include_directories(ai_gateway_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
