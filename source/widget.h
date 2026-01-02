#pragma once

namespace SBMap
{
    class IWidget
    {
    public:
        virtual ~IWidget() = default;
        
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUIRender() = 0;
    };
}
