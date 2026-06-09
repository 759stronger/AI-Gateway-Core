# AI Gateway Core 模块索引

## 核心层

- `core/types.h`：消息、会话、模型、token、请求上下文等通用类型。
- `core/result.h`：统一错误码、错误对象、Result、Status。

## 网关层

- `gateway/gateway_server.h`：HTTP 服务入口边界。
- `adapters/openai_adapter.h`：OpenAI-compatible 协议适配。

## 权限和用户

- `auth/auth_manager.h`：下游 API Key 鉴权。
- `users/user_manager.h`：用户状态、余额、额度。

## 上游和路由

- `upstream/upstream_account.h`：上游账号数据结构。
- `upstream/upstream_pool.h`：上游号池管理。
- `routing/model_mapping.h`：公开模型到真实模型的映射。
- `routing/model_router.h`：模型路由和 fallback 决策。

## Provider

- `providers/llm_provider.h`：聊天模型 Provider 抽象。
- `providers/provider_manager.h`：Provider 注册与获取。

## 运营治理

- `usage/usage_recorder.h`：请求日志与用量统计。
- `billing/billing_policy.h`：额度和计费策略。
- `limits/rate_limiter.h`：限流与并发控制。
- `health/health_checker.h`：渠道健康检查。

## 扩展能力

- `images/image_gateway.h`：生图能力抽象。

## 基础设施

- `storage/storage.h`：持久化抽象。
- `config/config_manager.h`：配置读取。
- `observability/observability.h`：trace、日志上下文。
- `admin/admin_api.h`：管理后台 API 边界。

## 聚合入口

- `ai_gateway_core.h`：统一 include 入口。
