#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <unordered_map>
#include <memory>

namespace psh::network
{
    class Session;

    class SessionManager
    {
    public:
        void RegisterSession(std::size_t handle, std::shared_ptr<Session> session);
        void UnregisterSession(std::size_t handle);
        std::shared_ptr<Session> GetSession(std::size_t handle);
        bool HasSession(std::size_t handle) const;
        size_t GetSessionCount() const;

    private:
        std::unordered_map<std::size_t, std::shared_ptr<Session>> sessions_;
    };
}

#endif
