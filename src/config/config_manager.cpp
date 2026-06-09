/**
 * @file config_manager.cpp
 * @brief 配置加载接口实现预留源文件。
 *
 * 后续实现方向：
 * - 从配置文件、环境变量或启动参数加载 GatewayConfig。
 * - 校验 host、port、database_path、log_level 和功能开关是否合法。
 * - 将 GatewayConfig 转换为 GatewayServerOptions 供服务初始化使用。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定具体配置格式。
 */
#include "ai_gateway_core/config/config_manager.h"

namespace ai_gateway_core {

}
