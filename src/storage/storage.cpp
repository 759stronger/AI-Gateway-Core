/**
 * @file storage.cpp
 * @brief 存储接口实现预留源文件。
 *
 * 后续实现方向：
 * - 实现 SQLite、文件或其他数据库版本的 Storage。
 * - 保存和查询用户、API Key、上游账号、模型映射和用量记录。
 * - 统一处理 StorageError、NotFound 和数据初始化逻辑。
 *
 * 当前文件只建立编译单元和模块边界，不提供默认实现，避免绑定具体数据库。
 */
#include "ai_gateway_core/storage/storage.h"

namespace ai_gateway_core {
    //初始化
    Status InMemoryStorage::initialize()
    {
        std::lock_guard<std::mutex> lock(mutex_); 
        initialized_ = true;
        return successStatus();
    }
    //检查是否初始化成功
    Status InMemoryStorage::ensureInitialized() const
    {
        if(!initialized_)
        {
            return failureStatus({ErrorCode::NotInitialized, "storage 初始化失败"});
        }
        return successStatus();
    }

    Status InMemoryStorage::saveUser(const UserAccount& user)
    {
        auto status = ensureInitialized();
        if(!status.ok)
        {
            return status;
        }
        if(user.user_id.empty())
        {
            return failureStatus({ErrorCode::InvalidArgument, "用户id为空"});
        }
        std::lock_guard<std::mutex> lock(mutex_); 
        users_[user.user_id] = user;
        return successStatus();
    }
    
    Result<UserAccount> InMemoryStorage::getUser(const std::string& user_id)
    {
        auto status = ensureInitialized();
        if(!status.ok)
        {
            return failureResult<UserAccount>(status.error);
        }
        if(user_id.empty())
        {
            return failureResult<UserAccount>({ErrorCode::InvalidArgument,"用户id为空"});
        }
        std::lock_guard<std::mutex> lock(mutex_); 
        auto it = users_.find(user_id);
        if(it == users_.end())
        {
            return failureResult<UserAccount>({ErrorCode::NotFound,"用户不存在"});
        }
        return successResult(it->second);
    }

    Status InMemoryStorage::saveApiKey(const ApiKeyIdentity& identity, const std::string& key_hash)
    {
        auto status = ensureInitialized();
        if(!status.ok)
        {
            return status;
        }
        if(identity.api_key_id.empty() || identity.user_id.empty())
        {
            return failureStatus({ErrorCode::InvalidArgument , "key或者用户不存在"});
        }
        if(key_hash.empty())
        {
            return failureStatus({ErrorCode::InvalidArgument , "key_hash不存在"});
        }
        std::lock_guard<std::mutex> lock(mutex_); 
        api_keys_by_hash_[key_hash] = identity;
        api_key_hash_by_id_[identity.api_key_id] = key_hash;
        return successStatus();

    }
    Result<ApiKeyIdentity> InMemoryStorage::getApiKeyByHash(const std::string& key_hash)
    {
        auto status = ensureInitialized();
        if(!status.ok)
        {
            return failureResult<ApiKeyIdentity>(status.error);
        }
        if(key_hash.empty())
        {
            return failureResult<ApiKeyIdentity>({ErrorCode::InvalidArgument , "key_hash 为空"});
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = api_keys_by_hash_.find(key_hash);
        if(it == api_keys_by_hash_.end())
        {
             return failureResult<ApiKeyIdentity>({ErrorCode::InvalidArgument , "api key 不存在"});
        }

        return successResult(it->second);
    }

    Result<ApiKeyIdentity> InMemoryStorage::getApiKeyById(const std::string& api_key_id)
    {
        
    }



}
