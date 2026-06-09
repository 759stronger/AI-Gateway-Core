# AIChatSDK 中转网关学习扩展路线图

> 目标：把当前 C++ 项目从“多模型聊天 SDK + HTTP 服务”逐步演进为 **C++ AI API 中转网关**。
>
> 主线：中转站 / API Gateway 为核心，Web 对话是内置 Playground，生图、RAG、工具调用、Agent 是后续可插拔能力。
>
> 技术建议：C++ 写后端网关核心，Vue/React/HTML 写管理后台和网页对话，SQLite 起步，后续可升级 PostgreSQL + Redis。

---

## 1. 新项目定位

原项目更像：

```text
Web 对话页面
    -> chatServer
    -> chat_sdk
    -> LLMProvider
    -> DeepSeek / ChatGPT / Gemini / Ollama
```

新目标应该是：

```text
Cursor / Claude Code / OpenAI SDK / 网页聊天 / 生图页面 / 自动化脚本
        |
        v
C++ AI Gateway
        |
        +--> 下游 API Key 鉴权
        +--> 用户额度 / 限流 / 并发控制
        +--> OpenAI-compatible 协议适配
        +--> 上游账号池 / API Key 池
        +--> 模型映射 / 模型路由 / fallback
        +--> token 统计 / 请求日志 / 成本计算
        +--> Chat / Image / Embedding / RAG / Tools / Agent
        |
        v
OpenAI / DeepSeek / Gemini / Claude / Ollama / ComfyUI / Stability / 其他上游
```

一句话定位：

```text
中转站为主，网页版对话为内置客户端，生图/RAG/Agent 是后续能力模块。
```

---

## 2. C++ 能不能做中转站

可以。

中转站本质不是语言问题，而是后端工程问题：

- 接收 HTTP 请求。
- 校验下游 API Key。
- 解析 OpenAI-compatible 请求。
- 根据模型和策略选择上游账号。
- 转发请求到上游模型。
- 支持同步和流式响应。
- 记录请求日志、token、费用、错误。
- 做限流、并发控制、fallback。
- 给 Web 管理后台提供 API。

C++ 的优势：

- 性能好。
- 资源可控。
- 适合做本地自托管网关。
- 适合学习网络、并发、资源池、状态管理。
- 作为项目作品有辨识度。

C++ 的挑战：

- Web 生态不如 Go / Node / Python 方便。
- 后台管理、ORM、鉴权、支付等要自己设计更多。
- 前端不建议用 C++ 写。

推荐组合：

```text
C++：网关核心、Provider、路由、号池、统计、SSE、数据库
Vue/React/HTML：管理后台、网页对话、生图页面
SQLite：本地学习阶段
PostgreSQL：平台化阶段
Redis：后期做限流、缓存、分布式状态
```

---

## 3. 什么是号池

你截图里看到的“渠道状态”“可用率”“PING 延迟”“过去 60 次记录”，本质就是 **上游渠道池 / 账号池 / API Key 池** 的可视化。

号池不是一个神秘功能，而是：

```text
多个上游账号或 API Key
        |
        v
统一纳入池子管理
        |
        v
每次请求从池子里挑一个可用账号
        |
        v
调用上游模型
        |
        v
记录成功率、延迟、消耗、错误、限流状态
```

### 3.1 正规 API Key 池

最适合你先做。

例子：

```text
DeepSeek Key A
DeepSeek Key B
OpenAI Key A
Gemini Key A
Ollama 本地 endpoint
```

每个上游账号需要记录：

```text
id
provider_name
model_name
base_url
api_key_encrypted
enabled
status
weight
priority
max_concurrency
current_concurrency
rpm_limit
tpm_limit
daily_quota
used_quota
success_count
failure_count
average_latency_ms
last_error
last_checked_at
last_used_at
```

优点：

- 合规性更稳。
- 技术实现清晰。
- 适合学习中转网关。
- 可以稳定测试。

### 3.2 订阅/OAuth/Session 账号池

Sub2API 这类项目常说的“订阅转 API”，可能会涉及：

```text
Claude Pro 账号
ChatGPT Plus 账号
Gemini Advanced 账号
OAuth Token
Session Key
Cookie
网页会话
```

这种也能做，但复杂度和风险高很多：

- 登录态维护。
- token 刷新。
- cookie 失效。
- 上游风控。
- 粘性会话。
- 账号封禁风险。
- 服务条款风险。

建议路线：

```text
先做正规 API Key 池
    -> OpenAI-compatible 中转
    -> 限流和统计
    -> 管理后台
    -> 再研究 OAuth/Session 类账号池
```

---

## 4. 号池运行流程

一次请求进入中转站后：

```text
1. 下游请求进入 /v1/chat/completions
2. 校验下游 API Key
3. 检查用户额度、权限、限流
4. 解析请求里的 public model
5. 根据 public model 找到候选上游账号
6. 过滤 disabled / exhausted / rate_limited / unhealthy 账号
7. 根据调度策略选择一个账号
8. 增加 current_concurrency
9. 转发请求到上游
10. 收到响应或错误
11. 记录延迟、token、状态、错误
12. 减少 current_concurrency
13. 如果失败，按策略 fallback 到下一个账号
14. 返回 OpenAI-compatible 响应给下游
```

常见调度策略：

```text
RoundRobin      轮询
Weighted        权重
LeastLatency    最低延迟
LeastUsed       最少使用
LocalFirst      本地优先
PriorityFirst   高优先级优先
StickySession   粘性会话
```

---

## 5. 推荐目录规划

```text
AIChatSDK/
├── gateway/                    # 中转网关核心
│   ├── include/
│   │   ├── gateway_server.h
│   │   ├── openai_adapter.h
│   │   ├── auth_manager.h
│   │   ├── user_manager.h
│   │   ├── upstream_account.h
│   │   ├── upstream_pool.h
│   │   ├── model_router.h
│   │   ├── usage_recorder.h
│   │   ├── rate_limiter.h
│   │   ├── billing_policy.h
│   │   ├── health_checker.h
│   │   ├── image_gateway.h
│   │   └── admin_api.h
│   └── src/
├── chatsdk/                    # 保留 Provider 和模型调用核心
│   ├── include/
│   └── src/
├── chatServer/                 # 逐步演进为 gateway server + playground
│   ├── main.cpp
│   ├── chatServer.cpp
│   └── www/
├── web/                        # 后期新增 Vue/React 管理后台
│   ├── admin/
│   └── playground/
├── test/
│   ├── unit/
│   ├── integration/
│   ├── e2e/
│   └── performance/
└── AIChatSDK_学习扩展路线图.md
```

如果暂时不想大改目录，可以先在现有 `chatsdk` / `chatServer` 内新增网关类，稳定后再拆分。

---

## 6. 功能模块总览与职责分工

### 6.1 GatewayServer：网关入口模块

职责：

- 启动 HTTP 服务。
- 注册 `/v1/*` OpenAI-compatible API。
- 注册 `/admin/*` 管理后台 API。
- 注册 `/api/*` 兼容旧 Web 对话接口或逐步废弃旧接口。
- 统一设置 CORS、请求大小限制、超时、错误响应。
- 给每个请求生成 `trace_id`。

边界：

- 不直接调用具体模型。
- 不直接操作上游 API Key。
- 只负责 HTTP 层入口和出口。

### 6.2 OpenAIAdapter：协议适配模块

职责：

- 解析 `/v1/chat/completions` 请求。
- 把 OpenAI `messages` 转成内部请求。
- 把内部响应包装成 OpenAI-compatible JSON。
- 处理 `stream=true` 的 SSE chunk。
- 后续支持 `/v1/images/generations`、`/v1/embeddings`。

核心价值：

```text
外部协议和内部实现解耦。
```

### 6.3 AuthManager：下游鉴权模块

职责：

- 解析 `Authorization: Bearer xxx`。
- 校验下游 API Key。
- 判断 Key 是否启用、过期、额度是否足够。
- 返回当前请求对应的用户身份。
- 记录 API Key 最近使用时间。

边界：

```text
下游 API Key 用于访问你的网关。
上游 API Key 用于你的网关访问模型厂商。
二者必须隔离。
```

### 6.4 UserManager：用户与额度模块

职责：

- 管理用户。
- 管理用户状态：正常、禁用、欠费、过期。
- 管理余额或额度。
- 查询用户用量。
- 后续对接充值、套餐、邀请、订单。

学习阶段可以先做：

```text
一个默认管理员
一个默认测试用户
一个固定开发 API Key
```

### 6.5 UpstreamPool：上游号池模块

职责：

- 管理多个上游账号或 API Key。
- 查询某个模型有哪些可用上游。
- 维护账号状态：可用、禁用、限流、冷却、失败。
- 维护并发数、成功率、失败率、平均延迟。
- 为 `ModelRouter` 提供候选账号列表。

它不负责最终选择账号，最终选择交给 `ModelRouter`。

### 6.6 ModelRouter：模型路由模块

职责：

- 根据下游请求的 `model` 找公开模型配置。
- 根据能力类型选择候选上游：chat / image / embedding。
- 根据策略选择一个上游账号。
- 支持 round_robin、priority_first、local_first、weighted、least_latency。
- 上游失败时选择 fallback 账号。

它是中转站的“大脑”。

输入示例：

```text
public_model = gpt-4o-mini
capability = chat
user_id = 1001
stream = true
```

输出示例：

```text
selected upstream account = deepseek-key-a
upstream model = deepseek-chat
base_url = https://api.deepseek.com
```

### 6.7 ProviderManager：模型调用模块

职责：

- 根据上游账号创建或选择对应 Provider。
- 调用 OpenAI / DeepSeek / Gemini / Ollama / ComfyUI 等上游。
- 屏蔽不同厂商 API 差异。
- 支持同步响应和流式响应。
- 返回统一内部响应。

Provider 不关心：

- 请求属于哪个用户。
- 用户有没有额度。
- 账号池怎么调度。
- 请求怎么计费。

这些属于网关层。

### 6.8 UsageRecorder：用量记录模块

职责：

- 记录每一次请求。
- 记录 prompt tokens、completion tokens、total tokens。
- 记录用户、API Key、公开模型、上游账号、耗时、错误。
- 给管理后台提供统计数据。
- 给额度扣减和计费模块提供依据。

### 6.9 BillingPolicy：价格与额度模块

职责：

- 配置模型倍率。
- 根据 token 计算费用。
- 扣减用户额度。
- 判断余额是否足够。
- 后续支持套餐、充值、订单、邀请返利。

学习阶段可以简化成：

```text
每次请求扣 1 点额度
或每 1000 token 扣固定额度
```

### 6.10 RateLimiter：限流与并发模块

职责：

- 用户级 RPM / TPM。
- API Key 级 RPM / TPM。
- 上游账号级 RPM / TPM。
- 上游账号最大并发。
- 模型级并发限制。

第一版用内存计数器即可，后续多实例部署再接 Redis。

### 6.11 HealthChecker：渠道健康检查模块

职责：

- 定时检查上游账号是否可用。
- 记录 PING 延迟。
- 记录最近 N 次请求成功/失败。
- 计算渠道可用率。
- 自动把异常账号标记为冷却或不可用。

截图里的“渠道状态”“过去 60 次记录”“可用率 97%”就是这个模块在管理端的可视化。

### 6.12 AdminAPI：管理后台接口模块

职责：

- 管理上游账号。
- 管理下游 API Key。
- 管理模型映射。
- 查看渠道状态。
- 查看请求日志。
- 查看用户用量。
- 修改系统配置。

这些接口必须有管理员鉴权。

### 6.13 WebAdmin：管理后台前端

职责：

- 展示仪表盘。
- 展示渠道状态。
- 添加/禁用上游账号。
- 生成下游 API Key。
- 查看日志、额度、错误率。
- 配置模型映射和价格倍率。

技术建议：

```text
早期：HTML + JS
中期：Vue 3
后期：Vue 3 + UI 组件库
```

### 6.14 Playground：网页对话和生图页面

职责：

- 提供内置 Web 对话。
- 提供模型选择。
- 提供流式输出展示。
- 提供生图输入和结果展示。
- 用于调试网关能力。

原则：

```text
Playground 必须走 /v1 API。
不要绕过网关直接调用 chat_sdk。
```

### 6.15 ImageGateway：生图能力模块

职责：

- 实现 `/v1/images/generations`。
- 抽象 ImageProvider。
- 接 OpenAI Images / Gemini Image / Stability / ComfyUI。
- 记录生图任务、消耗、失败原因。
- 后续支持图片编辑、局部重绘、图生图。

生图不应该塞进 ChatProvider，而应该作为独立能力。

### 6.16 Storage：持久化模块

职责：

- 封装 SQLite / PostgreSQL。
- 管理用户、API Key、上游账号、模型映射、请求日志。
- 后续支持数据库迁移。

学习阶段 SQLite 足够，平台化阶段建议 PostgreSQL。

### 6.17 ConfigManager：配置模块

职责：

- 读取命令行参数。
- 读取配置文件。
- 读取环境变量。
- 管理服务端口、日志级别、默认 API Key、数据库路径、上游配置。

优先级建议：

```text
命令行参数 > 环境变量 > 配置文件 > 默认值
```

### 6.18 Observability：日志与追踪模块

职责：

- 统一 trace_id。
- 记录请求入口和出口。
- 记录路由选择。
- 记录上游账号调用。
- 记录 fallback。
- 记录错误码。
- 记录耗时。

没有这个模块，后续号池和限流很难调试。

### 6.19 模块协作关系

聊天请求：

```text
GatewayServer
    -> OpenAIAdapter
    -> AuthManager
    -> RateLimiter
    -> BillingPolicy
    -> ModelRouter
    -> UpstreamPool
    -> ProviderManager
    -> UsageRecorder
    -> Observability
```

管理端查看渠道状态：

```text
WebAdmin
    -> AdminAPI
    -> AuthManager(admin)
    -> UpstreamPool
    -> HealthChecker
    -> UsageRecorder
```

生图请求：

```text
GatewayServer
    -> ImageAdapter
    -> AuthManager
    -> RateLimiter
    -> ModelRouter
    -> UpstreamPool
    -> ImageGateway
    -> UsageRecorder
```

---

## 7. 核心数据库设计草案

### 7.1 users：下游用户表

```text
users
├── id
├── username
├── status
├── balance
├── quota_limit
├── used_quota
├── created_at
└── updated_at
```

### 7.2 api_keys：下游 API Key 表

```text
api_keys
├── id
├── user_id
├── key_hash
├── name
├── enabled
├── expires_at
├── quota_limit
├── used_quota
├── created_at
└── last_used_at
```

注意：不要明文保存下游 API Key，只保存 hash。

### 7.3 upstream_accounts：上游账号池表

```text
upstream_accounts
├── id
├── name
├── provider
├── account_type
├── base_url
├── api_key_encrypted
├── enabled
├── status
├── priority
├── weight
├── max_concurrency
├── current_concurrency
├── rpm_limit
├── tpm_limit
├── daily_quota
├── used_quota
├── success_count
├── failure_count
├── average_latency_ms
├── last_error
├── last_checked_at
└── created_at
```

注意：上游 API Key 要加密保存，至少不要明文打印日志。

### 7.4 model_mappings：模型映射表

```text
model_mappings
├── id
├── public_model_name
├── upstream_model_name
├── upstream_provider
├── capability
├── enabled
├── price_multiplier
└── created_at
```

例子：

```text
下游请求：gpt-4o-mini
实际上游：deepseek-chat / gemini-flash / ollama-qwen
```

### 7.5 request_logs：请求日志表

```text
request_logs
├── id
├── trace_id
├── user_id
├── api_key_id
├── public_model
├── upstream_account_id
├── upstream_model
├── capability
├── prompt_tokens
├── completion_tokens
├── total_tokens
├── cost
├── latency_ms
├── status
├── error_code
├── error_message
├── stream
└── created_at
```

---

## 8. 核心流程总览

### 8.1 聊天中转完整流程

```text
1. 客户端请求 POST /v1/chat/completions
2. GatewayServer 生成 trace_id
3. OpenAIAdapter 解析请求体
4. AuthManager 校验下游 API Key
5. UserManager 查询用户状态和额度
6. RateLimiter 检查用户、Key、模型、上游限制
7. BillingPolicy 预估本次请求是否允许执行
8. ModelRouter 根据 public model 查模型映射
9. UpstreamPool 返回候选上游账号
10. ModelRouter 根据策略选择账号 A
11. ProviderManager 使用账号 A 调用真实上游
12. 如果成功：返回模型响应
13. 如果失败：判断是否可 fallback
14. 如果可 fallback：选择账号 B 再试
15. UsageRecorder 记录请求日志、token、耗时、错误
16. BillingPolicy 扣减额度
17. OpenAIAdapter 包装为 OpenAI-compatible 响应
18. GatewayServer 返回给客户端
```

### 8.2 流式聊天流程

```text
1. 客户端请求 stream=true
2. GatewayServer 设置 text/event-stream
3. ProviderManager 调用上游流式接口
4. 每收到一个 chunk，转换成 OpenAI SSE chunk
5. 立即写回客户端
6. 上游结束后发送 [DONE]
7. UsageRecorder 记录完整响应和 token
8. 如果中途失败，记录 error 和 partial output
```

流式场景要注意：

- 客户端断开连接怎么办。
- 上游断流怎么办。
- 已经输出了一半时是否还能 fallback。
- 如何统计最终 token。
- 如何保证连接正确关闭。

### 8.3 生图中转流程

```text
1. 客户端请求 POST /v1/images/generations
2. GatewayServer 生成 trace_id
3. ImageAdapter 解析 prompt、size、n、model
4. AuthManager 校验下游 API Key
5. RateLimiter 检查生图频率和并发
6. ModelRouter 选择支持 image capability 的上游
7. ImageGateway 调用 OpenAI Images / ComfyUI / Stability
8. 保存图片 URL 或 base64 结果
9. UsageRecorder 记录生图任务和消耗
10. 返回 OpenAI-compatible image response
```

### 8.4 管理端查看渠道状态流程

```text
1. 管理员打开 WebAdmin 渠道状态页面
2. WebAdmin 请求 GET /admin/upstreams/status
3. AdminAPI 校验管理员身份
4. 查询 UpstreamPool 中所有账号状态
5. 查询 HealthChecker 最近健康检查结果
6. 查询 UsageRecorder 最近请求成功率和延迟
7. 返回渠道卡片数据
8. 前端展示可用率、PING、最近 N 次成功/失败
```

普通用户不走这个流程，也不应该看到真实上游账号。

### 8.5 健康检查流程

```text
1. HealthChecker 定时扫描 enabled 的上游账号
2. 对每个账号发起轻量检查
3. 记录响应延迟
4. 如果连续失败，标记 Unhealthy
5. 如果遇到 429，标记 RateLimited 或 CoolingDown
6. 如果恢复成功，标记 Available
7. 更新管理端渠道状态
```

### 8.6 额度扣减流程

```text
1. 请求进入后先做额度预检查
2. 请求完成后读取真实 token usage
3. 没有 usage 时使用估算 token
4. 根据模型倍率计算 cost
5. 从用户额度扣减
6. 写入 request_logs
7. 更新 api_keys.used_quota 和 users.used_quota
```

第一版可以不做金钱，只做额度点数。

### 8.7 fallback 流程

```text
1. 账号 A 调用失败
2. 判断错误类型
3. 401 / 参数错误 / 模型不存在：不 fallback
4. 网络错误 / 超时 / 429 / 5xx：允许 fallback
5. UpstreamPool 标记账号 A 失败
6. ModelRouter 选择账号 B
7. 账号 B 调用成功则返回
8. 日志记录 fallback_from=A, fallback_to=B
```

---

## 9. 管理端和用户端分工

### 9.1 普通用户端能看到

```text
可用模型
自己的 API Key
自己的余额或额度
自己的调用记录
自己的 token 消耗
自己的 Web 对话记录
自己的生图任务记录
```

普通用户不应该看到：

```text
真实上游账号
真实上游 API Key
所有用户日志
所有渠道状态
fallback 细节
账号池调度策略
```

### 9.2 管理端能看到

```text
所有用户
所有下游 API Key
所有上游账号池
所有模型映射
渠道可用率
渠道延迟
渠道错误日志
请求总量
费用统计
限流状态
fallback 状态
健康检查记录
```

原因：

- 保护上游账号和密钥安全。
- 避免普通用户知道真实路由细节。
- 管理端负责运维和调度。
- 用户端只负责使用。

---

## 10. 分阶段实战路线

### 10.1 阶段 0：确认方向和边界

目标：

```text
C++ AI Gateway 为主线
Web Chat 是内置测试客户端
Image / RAG / Agent 是后续插件能力
```

任务：

- 保留现有聊天功能。
- 不急着删除旧代码。
- 先新增 Gateway 思维的模块和接口。
- 在文档中明确新定位。

完成标准：

```text
用户 Key != 上游 Key
公开模型名 != 实际上游模型名
聊天页面 != 网关核心
Provider != 账号池
```

### 10.2 阶段 1：OpenAI-compatible 最小网关闭环

目标：

```text
POST /v1/chat/completions
    -> 调用现有 Provider
    -> 返回 OpenAI 风格响应
```

新增接口：

```text
GET  /v1/models
POST /v1/chat/completions
```

任务：

1. 新建 OpenAI 请求结构解析逻辑。
2. 把 OpenAI `messages` 转成当前 `Message`。
3. 根据 `model` 找 Provider。
4. 调用同步或流式接口。
5. 把结果包装成 OpenAI-compatible JSON。

完成标准：

- 能用 OpenAI 风格请求调用服务。
- 支持 `stream=false`。
- 支持 `stream=true`。
- 旧 Web 对话页仍可用。

### 10.3 阶段 2：下游 API Key 鉴权

目标：

```text
用户请求你的网关：Authorization: Bearer sk-your-gateway-key
你的网关内部再使用上游 Key 调模型
```

任务：

1. 新增 `api_keys` 表。
2. 生成下游 API Key。
3. 保存 key hash。
4. 请求进入时解析 Authorization header。
5. 校验 key 是否存在、启用、过期。
6. 校验通过后继续请求。

完成标准：

- 无 Key 返回 401。
- 错误 Key 返回 401。
- 正确 Key 可以调用。
- 日志不打印完整 Key。

### 10.4 阶段 3：正规 API Key 号池

目标：

```text
同一个模型可以配置多个上游 Key，并能轮询调用。
```

任务：

1. 新增 `UpstreamAccount` 数据结构。
2. 新增 `UpstreamPool` 管理类。
3. 支持添加多个上游账号。
4. 按模型筛选可用账号。
5. 实现最简单轮询策略。
6. 请求成功后记录成功次数和延迟。
7. 请求失败后记录错误。

推荐类设计：

```text
UpstreamPool
├── addAccount(account)
├── disableAccount(account_id)
├── listAccounts()
├── selectAccount(model, capability)
├── markSuccess(account_id, latency_ms)
└── markFailure(account_id, error)
```

完成标准：

- 同一个模型可以配置多个上游 Key。
- 多次请求能轮流使用不同账号。
- 失败账号能被标记。
- 管理接口可以查看账号状态。

### 10.5 阶段 4：模型映射和路由策略

目标：

```text
下游看到的模型名和真实上游模型解耦。
```

例子：

```text
下游请求：gpt-4o-mini
实际可路由到：
    - OpenAI gpt-4o-mini
    - DeepSeek deepseek-chat
    - Gemini gemini-flash
    - Ollama qwen2.5
```

任务：

1. 新增模型映射表。
2. 支持 public model name。
3. 支持 upstream model name。
4. 支持 capability：chat / image / embedding。
5. 实现 `ModelRouter`。
6. 支持 local_first、priority_first、round_robin。

完成标准：

- `/v1/models` 返回公开模型。
- 请求公开模型时能路由到真实上游。
- 可以通过配置切换上游，不改客户端。

### 10.6 阶段 5：用量统计、额度和计费雏形

目标：

```text
记录每次请求用了多少资源。
```

任务：

1. 新增 request_logs。
2. 记录 trace_id。
3. 记录用户、API Key、模型、上游账号。
4. 记录 latency、status、error。
5. 如果上游返回 token usage，就保存。
6. 如果没有返回 token usage，先做简单估算。
7. 支持用户额度扣减。

第一版 token 估算：

```text
中文：约 1 到 2 个字符一个 token
英文：约 4 个字符一个 token
```

完成标准：

- 每次请求都有日志。
- 能查看某个用户消耗。
- 能查看某个上游账号消耗。
- 额度不足时拒绝请求。

### 10.7 阶段 6：限流、并发控制和 fallback

目标：

```text
让中转站稳定，不把某个账号打爆。
```

要做的限制：

```text
用户级 RPM
用户级 TPM
API Key 级额度
上游账号级 RPM
上游账号级并发数
模型级并发数
```

完成标准：

- 超过限流返回 429。
- 某个账号失败后可以 fallback。
- 账号进入冷却状态后短时间不再使用。

### 10.8 阶段 7：Web 对话 Playground

目标：

```text
保留并升级网页版聊天，但它不是主线核心，而是网关的内置客户端。
```

功能：

- 选择公开模型。
- 输入下游 API Key。
- 普通聊天。
- 流式输出。
- 查看会话历史。
- 查看请求 trace_id。

完成标准：

- 网页聊天走 `/v1/chat/completions`。
- 和外部客户端使用同一套鉴权、路由、统计逻辑。

### 10.9 阶段 8：生图能力

目标：

```text
增加图像生成中转接口。
```

接口：

```text
POST /v1/images/generations
```

可接上游：

- OpenAI Images。
- Gemini Image。
- Stability API。
- ComfyUI 本地服务。
- Stable Diffusion WebUI API。

完成标准：

- 能通过 `/v1/images/generations` 请求生图。
- 请求经过下游 Key 鉴权。
- 请求记录用量。
- Web 页面有简单生图 Playground。

### 10.10 阶段 9：管理后台

目标：

```text
做一个类似截图里的后台。
```

页面模块：

```text
仪表盘
API 密钥
上游账号池
模型映射
渠道状态
使用日志
用户额度
系统设置
Web 对话
生图页面
```

后端管理 API：

```text
GET    /admin/upstreams
POST   /admin/upstreams
PUT    /admin/upstreams/{id}
DELETE /admin/upstreams/{id}
GET    /admin/models
POST   /admin/models
GET    /admin/logs
GET    /admin/stats
POST   /admin/api-keys
```

完成标准：

- 能在页面添加上游账号。
- 能查看渠道状态。
- 能生成下游 API Key。
- 能查看请求日志和可用率。

### 10.11 阶段 10：RAG、工具调用和 Agent

这些是后续扩展，不要抢在网关主线前面。

RAG：

```text
文档上传
    -> 切块
    -> embedding
    -> 检索
    -> 拼接上下文
    -> 调用 /v1/chat/completions
```

工具调用：

```text
ToolRegistry
ToolExecutor
ToolCallParser
```

Agent：

```text
用户任务
    -> 模型思考
    -> 工具调用
    -> 观察结果
    -> 再次推理
    -> 最终回答
```

推荐顺序：

```text
RAG 最小闭环
    -> 单工具调用
    -> 单轮 Agent
    -> 多轮 Agent
```

---

## 11. 开发实现总顺序

最稳的实现顺序：

```text
第 1 步：MockProvider
第 2 步：/v1/models
第 3 步：/v1/chat/completions stream=false
第 4 步：/v1/chat/completions stream=true
第 5 步：下游 API Key 鉴权
第 6 步：request_logs
第 7 步：UpstreamAccount 数据结构
第 8 步：UpstreamPool 轮询
第 9 步：ModelMapping
第 10 步：ModelRouter
第 11 步：fallback
第 12 步：RateLimiter
第 13 步：Web 对话页面改走 /v1 API
第 14 步：管理端最小页面
第 15 步：渠道状态和健康检查
第 16 步：/v1/images/generations
第 17 步：生图 Playground
第 18 步：RAG
第 19 步：Tools
第 20 步：Agent
```

原则：

```text
先打通网关闭环
再加安全鉴权
再加号池路由
再加统计限流
再做前端体验
最后扩展高级能力
```

每完成一个阶段，都要能回答：

```text
这个功能有没有 API 可以调用？
这个功能有没有数据表或内存状态？
这个功能有没有日志？
这个功能失败时返回什么错误？
这个功能是否能用 MockProvider 测试？
这个功能是否影响旧聊天接口？
```

---

## 12. 测试路线

### 12.1 测试优先级

```text
MockProvider
    -> OpenAI-compatible API 测试
    -> API Key 鉴权测试
    -> UpstreamPool 测试
    -> ModelRouter 测试
    -> UsageRecorder 测试
    -> RateLimiter 测试
    -> SSE 流式测试
    -> Image API 测试
```

### 12.2 推荐测试目录

```text
test/
├── unit/
│   ├── test_auth_manager.cpp
│   ├── test_upstream_pool.cpp
│   ├── test_model_router.cpp
│   ├── test_usage_recorder.cpp
│   └── test_rate_limiter.cpp
├── integration/
│   ├── test_openai_chat_api.cpp
│   ├── test_gateway_auth.cpp
│   └── test_gateway_fallback.cpp
├── e2e/
│   ├── test_chat_playground.cpp
│   └── test_image_generation.cpp
└── performance/
    └── test_concurrent_gateway.cpp
```

### 12.3 必测场景

- 没有 API Key 返回 401。
- 错误 API Key 返回 401。
- 额度不足返回 402 或自定义错误。
- 限流返回 429。
- 上游失败触发 fallback。
- 流式响应能正常结束。
- 上游账号并发数不会超过限制。
- 请求日志准确记录。
- 生图请求也走鉴权和统计。

---

## 13. 本地开发流程

### 13.1 编译 SDK

```bash
cd /home/sly/chatSDK/chatsdk
mkdir -p build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

### 13.2 编译服务端

```bash
cd /home/sly/chatSDK/chatServer
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### 13.3 启动服务端

```bash
cd /home/sly/chatSDK/chatServer
./build/AIChatServer --port=8080
```

后续建议增加开发参数：

```bash
./build/AIChatServer \
  --port=8080 \
  --enable_mock=true \
  --gateway_api_key=local-dev-key
```

### 13.4 OpenAI-compatible 测试

```bash
curl http://127.0.0.1:8080/v1/models \
  -H "Authorization: Bearer local-dev-key"
```

```bash
curl http://127.0.0.1:8080/v1/chat/completions \
  -H "Authorization: Bearer local-dev-key" \
  -H "Content-Type: application/json" \
  -d '{"model":"mock-model","messages":[{"role":"user","content":"你好"}],"stream":false}'
```

---

## 14. 安全边界

必须注意：

- 不要把上游 API Key 打印到日志。
- 下游 API Key 不要明文入库，只保存 hash。
- 上游 API Key 至少要加密保存，或本地开发阶段先用环境变量。
- 不要一开始做公开商用。
- 不要一开始碰复杂逆向账号池。
- 先做正规 API Key 池，后面再研究 OAuth/Session。
- 管理后台必须有管理员登录。
- 生图接口要注意内容安全和资源消耗。

---

## 15. 最终目标

最终项目可以成为：

```text
C++ AI API 中转网关
├── OpenAI-compatible API
├── 下游 API Key 分发
├── 正规上游 API Key 池
├── 模型映射
├── 路由策略
├── fallback
├── 限流和并发控制
├── token 统计
├── 请求日志
├── 渠道状态监控
├── Web 对话 Playground
├── 生图接口和页面
├── 管理后台
├── RAG 扩展
├── 工具调用扩展
└── Agent 扩展
```

这比单纯聊天网页更有工程价值，也更适合长期学习。

---

## 16. 当前最该做的一步

最推荐从这里开始：

```text
实现 MockProvider + /v1/chat/completions + 下游 API Key 鉴权
```

原因：

- 不依赖外网。
- 不依赖真实 API Key。
- 可以测试网关链路。
- 后续号池、日志、路由、限流都能接在这个闭环上。

一旦这个闭环跑通，项目就正式从“聊天 SDK”进入“AI 中转网关”的阶段。
