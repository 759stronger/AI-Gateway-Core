# AI Gateway Core 测试规划

## unit

- AuthManager
- UpstreamPool
- ModelRouter
- UsageRecorder
- BillingPolicy
- RateLimiter
- HealthChecker

## integration

- `/v1/models`
- `/v1/chat/completions`
- API Key 鉴权
- fallback
- request_logs

## e2e

- Web 对话 Playground
- 生图 Playground

## performance

- 并发请求
- 流式响应
- 号池并发控制
