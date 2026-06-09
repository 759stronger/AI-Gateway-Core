/**
 * @file ai_gateway_core.h
 * @brief AI Gateway Core 的公共聚合头文件。
 *
 * 该文件集中包含网关核心库对外暴露的主要模块，调用方只需要包含本文件，
 * 就可以访问核心类型、网关服务、认证、用户、上游账号、模型路由、供应商、
 * 用量计费、限流、健康检查、图片网关、存储、配置、可观测性和管理接口。
 *
 * 与其他文件的联系：
 * - core/types.h 和 core/result.h 提供全项目共享的数据类型与错误返回模型。
 * - gateway/gateway_server.h 定义服务生命周期入口。
 * - adapters/openai_adapter.h 定义 OpenAI 兼容协议的请求和响应转换。
 * - 其余头文件分别定义网关流水线中的认证、路由、供应商调用、计费和运维能力。
 */
#pragma once

#include "ai_gateway_core/core/types.h"
#include "ai_gateway_core/core/result.h"
#include "ai_gateway_core/gateway/gateway_server.h"
#include "ai_gateway_core/adapters/openai_adapter.h"
#include "ai_gateway_core/auth/auth_manager.h"
#include "ai_gateway_core/users/user_manager.h"
#include "ai_gateway_core/upstream/upstream_account.h"
#include "ai_gateway_core/upstream/upstream_pool.h"
#include "ai_gateway_core/routing/model_mapping.h"
#include "ai_gateway_core/routing/model_router.h"
#include "ai_gateway_core/providers/llm_provider.h"
#include "ai_gateway_core/providers/provider_manager.h"
#include "ai_gateway_core/usage/usage_recorder.h"
#include "ai_gateway_core/billing/billing_policy.h"
#include "ai_gateway_core/limits/rate_limiter.h"
#include "ai_gateway_core/health/health_checker.h"
#include "ai_gateway_core/images/image_gateway.h"
#include "ai_gateway_core/storage/storage.h"
#include "ai_gateway_core/config/config_manager.h"
#include "ai_gateway_core/observability/observability.h"
#include "ai_gateway_core/admin/admin_api.h"
