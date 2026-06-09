# AI Gateway Core

这是 AIChatSDK 的中转网关核心骨架目录。

当前目录只提供架构、模块边界、类型定义和接口声明，不实现具体业务逻辑。

## 目标

```text
C++ AI Gateway Core
├── OpenAI-compatible API
├── 下游 API Key 鉴权
├── 上游 API Key 号池
├── 模型映射和路由
├── fallback
├── 限流和并发控制
├── token 与用量统计
├── 渠道健康检查
├── 生图能力接口
├── 管理后台 API
└── 可观测性
```

## 目录

```text
include/ai_gateway_core/
├── core              通用类型、错误、结果对象
├── gateway           网关入口
├── adapters          OpenAI-compatible 协议适配
├── auth              下游 API Key 鉴权
├── users             用户与额度
├── upstream          上游账号池
├── routing           模型映射和路由
├── providers         模型 Provider 抽象
├── usage             请求日志与用量统计
├── billing           额度和计费策略
├── limits            限流和并发控制
├── health            渠道健康检查
├── images            生图能力
├── storage           持久化抽象
├── config            配置管理
├── observability     trace 与日志上下文
└── admin             管理后台 API
```

## 实现原则

- 先实现 MockProvider。
- 再实现 `/v1/models` 和 `/v1/chat/completions`。
- 再实现下游 API Key 鉴权。
- 再接入 UpstreamPool、ModelRouter、UsageRecorder。
- 真实上游调用、数据库 SQL、HTTP 路由、限流算法后续逐步填充。
