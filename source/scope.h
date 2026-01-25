#pragma once

#include <type_traits>

namespace SBMap
{
    template<typename TResource, typename TDeleter>
    requires(std::is_invocable_v<TDeleter, TResource*>)
    class Scope
    {
    public:
        Scope(TResource* resource, TDeleter deleter)
            : m_Resource(resource)
            , m_Deleter(deleter)
        {}
        
        ~Scope()
        {
            if (m_Resource)
                m_Deleter(m_Resource);
        }
        
        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
        
        Scope(Scope&&) = delete;
        Scope& operator=(Scope&&) = delete;
        
        TResource* Get() { return m_Resource; }
        const TResource* Get() const { return m_Resource; }
        
        TResource& operator*() { return *m_Resource; }
        TResource* operator->() { return m_Resource; }
        const TResource& operator*() const { return *m_Resource; }
        const TResource* operator->() const { return m_Resource; }
        
        operator bool() const { return m_Resource != nullptr; }
        
    private:
        TResource* m_Resource = nullptr;
        TDeleter m_Deleter;
    };
    
    template<typename TResource, typename TDeleter>
    Scope<TResource, TDeleter> MakeScope(TResource* resource, TDeleter deleter)
    {
        return Scope<TResource, TDeleter>(resource, deleter);
    }
}
