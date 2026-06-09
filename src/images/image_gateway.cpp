/**
 * @file image_gateway.cpp
 * @brief 图片网关和图片供应商接口实现预留源文件。
 *
 * 后续实现方向：
 * - 实现 ImageGateway 的完整请求链路：认证上下文、路由、限流、计费、供应商调用和用量记录。
 * - 实现 ImageProvider 子类，对接真实图片模型供应商。
 * - 将 ProviderImageRequest 中的变量映射为供应商 HTTP 请求参数。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免假定具体图片供应商协议。
 */
#include "ai_gateway_core/images/image_gateway.h"

namespace ai_gateway_core {

}
