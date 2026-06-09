/**
 * @file ai_gateway_core.cpp
 * @brief AI Gateway Core 库的源文件入口。
 *
 * 当前项目的大部分能力以头文件中的抽象接口和数据结构形式暴露，本源文件用于形成
 * 一个可编译的库目标，并统一包含公共聚合头 ai_gateway_core.h。
 *
 * 与其他文件的联系：
 * - ai_gateway_core/ai_gateway_core.h 聚合所有对外接口和 DTO。
 * - 后续如果添加默认实现、工厂函数或库级初始化逻辑，可以放在本文件中。
 */
#include "ai_gateway_core/ai_gateway_core.h"
