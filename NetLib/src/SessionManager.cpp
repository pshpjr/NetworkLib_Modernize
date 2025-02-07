#include "Network/SessionManager.h"
#include "Network/Session.h"

namespace psh::network
{
    void SessionManager::RegisterSession(std::size_t handle, std::shared_ptr<Session> session)
    {
        sessions_.insert({handle, std::move(session)});
    }

    void SessionManager::UnregisterSession(std::size_t handle)
    {
        sessions_.erase(handle);
    }

    std::shared_ptr<Session> SessionManager::GetSession(std::size_t handle)
    {
        auto it = sessions_.find(handle);
        return it != sessions_.end() ? it->second : nullptr;
    }

    bool SessionManager::HasSession(std::size_t handle) const
    {
        return sessions_.contains(handle);
    }

    size_t SessionManager::GetSessionCount() const
    {
        return sessions_.size();
    }
}
