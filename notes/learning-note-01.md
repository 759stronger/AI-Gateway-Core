# 学习笔记 1：Result 体系与 InMemoryStorage 基础实现

> 来源说明  
> 这篇笔记根据当前这次结对学习会话整理。  
> 我不能读取你在别处的全部历史会话，所以“所有会话”这里先按本次会话沉淀。  
> 后续有新知识，继续往后写新的编号笔记，或在本系列下追加。

## 1. 本次学习主题

- `result.h` 里的返回值体系：`ErrorCode` / `Error` / `Status` / `Result<T>`
- `InMemoryStorage` 的生命周期设计：`initialize()` 与 `ensureInitialized()`
- 内存存储里最基础的用户与 API Key 保存/查询
- `std::unordered_map` 的 `[]` 和 `find()` 的区别
- 成员函数实现和普通函数实现的区别

## 2. 问题索引

### Q1. `initialize()` 和 `ensureInitialized()` 有什么区别？

**短答**

- `initialize()` 负责真的把对象切换到“可用状态”
- `ensureInitialized()` 负责每次操作前检查“现在是不是已经可用”

**记忆法**

- `initialize()` = 开门
- `ensureInitialized()` = 进门前检查门开没开

**工程意义**

- 生命周期函数和前置条件检查函数分离
- 避免每个业务函数都自己重复写初始化状态逻辑

### Q2. 为什么 `result.h` 要搞这么多层？

**短答**

- `ErrorCode`：错误类别
- `Error`：错误类别 + 错误消息
- `Status`：只有成败，没有业务值
- `Result<T>`：成败 + 成功时的业务值

**核心判断规则**

- 动作函数：`Status`
- 查询函数：`Result<T>`

### Q3. 为什么 `ErrorCode` 用 `enum class`？

**短答**

- 它是强类型枚举
- 用来表达“这个值只能从一组固定业务状态里选”

**优点**

- 不污染命名空间
- 不容易和别的枚举混用
- 不会随便隐式转成整数

### Q4. `saveUser()` 到底是在做什么？

**短答**

- 接收外面传进来的 `UserAccount`
- 用 `user.user_id` 作为 key
- 把整个 `user` 存进 `users_`

也就是：

```cpp
users_[user.user_id] = user;
```

### Q5. 为什么 `getUser()` 不能用 `users_[user_id]`？

**短答**

- 因为 `map[key]` 在 key 不存在时会偷偷插入默认值
- 查询函数不应该在“查不到”时产生副作用

**正确做法**

```cpp
auto it = users_.find(user_id);
if (it == users_.end()) { ... }
```

### Q6. 为什么 `saveApiKey()` 要维护两张表？

**短答**

- `api_keys_by_hash_`：`key_hash -> identity`
- `api_key_hash_by_id_`：`api_key_id -> key_hash`

**作用**

- 鉴权时通常按 `key_hash` 查
- 撤销/管理时通常按 `api_key_id` 查

这属于一个对象维护多个索引。

## 3. 本次真正写过的代码模式

### 模式 1：动作函数

适用：

- `initialize()`
- `saveUser()`
- `saveApiKey()`

模板：

```cpp
Status Xxx(...)
{
    auto status = ensureInitialized();
    if (!status.ok)
    {
        return status;
    }

    // 参数校验
    // 加锁
    // 写入

    return successStatus();
}
```

### 模式 2：查询函数

适用：

- `getUser()`
- `getApiKeyByHash()`

模板：

```cpp
Result<T> Xxx(...)
{
    auto status = ensureInitialized();
    if (!status.ok)
    {
        return failureResult<T>(status.error);
    }

    // 参数校验
    // 查找
    // 找不到返回 failureResult<T>(...)

    return successResult(value);
}
```

## 4. 本次容易犯的错误

### 错误 1：把成员函数写成普通函数

错误写法：

```cpp
Status saveUser(const UserAccount& user)
```

正确写法：

```cpp
Status InMemoryStorage::saveUser(const UserAccount& user)
```

**原因**

- 只有成员函数实现才能访问：
  - `ensureInitialized()`
  - `mutex_`
  - `users_`

### 错误 2：把 `Status` 当成 `bool`

错误思路：

```cpp
if (!ensureInitialized())
```

正确思路：

```cpp
auto status = ensureInitialized();
if (!status.ok)
{
    return status;
}
```

### 错误 3：查询时误用 `unordered_map[key]`

错误后果：

- 查不到时会插入默认对象

正确做法：

- 用 `find()`

### 错误 4：返回类型不匹配

比如：

- `getUser()` 返回 `Result<UserAccount>`
- 不能直接 `return status;`
- 也不能直接 `return user;`

必须写成：

```cpp
return failureResult<UserAccount>(...);
return successResult(user);
```

## 5. 知识点拆解

### 5.1 `Status` 和 `Result<T>` 的联系

- 二者都表示“成功还是失败”
- 区别在于有没有业务值

**类比**

- `Status` 像“操作回执”
- `Result<T>` 像“操作回执 + 快递内容”

### 5.2 为什么要先检查初始化？

因为对象可能还没准备好。

第一版内存存储里看起来只是一个 `bool initialized_`，但以后换成 SQLite 时，`initialize()` 可能会真的做这些事：

- 打开数据库
- 建表
- 检查连接

所以现在先把这个习惯建立起来，是为了让以后替换底层实现时，上层代码不用推倒重来。

### 5.3 多索引设计的优缺点

**优点**

- 查找快
- 面向不同业务入口灵活
- 管理动作和认证动作都能高效支持

**缺点**

- 写入时必须同时维护多张表
- 一旦更新逻辑不一致，容易出现状态不同步

**工程要求**

- 同一批更新应尽量在同一临界区里完成

## 6. 本次学习得到的“举一反三”规则

### 规则 1

看到函数签名时先判断：

- 它是动作函数还是查询函数？

如果是动作函数，优先想 `Status`。  
如果是查询函数，优先想 `Result<T>`。

### 规则 2

只要是“只查不改”的 map 访问，优先想：

```cpp
find()
```

而不是：

```cpp
operator[]
```

### 规则 3

类成员函数实现时，先确认你是不是漏了：

```cpp
类名::
```

### 规则 4

工程代码里先建立统一模式，再复制到相似函数。

这次已经形成了两个可复用模板：

- 写入模板
- 查询模板

后面写 `getApiKeyById()`、`saveUpstreamAccount()`、`getModelMapping()` 时都可以套这个思路。

## 7. 当前已经掌握的函数

- `InMemoryStorage::initialize()`
- `InMemoryStorage::ensureInitialized()`
- `InMemoryStorage::saveUser(...)`
- `InMemoryStorage::getUser(...)`
- `InMemoryStorage::saveApiKey(...)`

## 8. 下一步该学什么

优先顺序建议：

1. `InMemoryStorage::getApiKeyByHash(...)`
2. `InMemoryStorage::getApiKeyById(...)`
3. `InMemoryStorage::updateApiKey(...)`

这三步会把“多索引查询与更新”练熟。

## 9. 本页总结

这次真正学到的不是几个零散语法点，而是一套很基础的工程写法：

- 用统一返回值协议表达成功/失败
- 用生命周期函数和守卫函数管理对象状态
- 用 `find()` 做纯查询
- 用多索引支持不同业务入口
- 用成员函数作用域访问对象内部状态

如果后面继续学新的知识，按同样结构继续写“学习笔记 2、3、4”会很合适。

## 10. 参考思路

这篇笔记的组织方式借鉴了两类常见做法：

- Cornell Notes 的“问题线索 + 主体笔记 + 总结”思路
- 工程 Markdown 笔记常见的“模式 / 错误 / 模板 / 下一步”结构

参考：

- Cornell Notes（Wikipedia）：https://en.wikipedia.org/wiki/Cornell_Notes
- Note-taking / Outline methods（Wikipedia）：https://en.wikipedia.org/wiki/Note-taking
- Markdown（Wikipedia）：https://en.wikipedia.org/wiki/Markdown
