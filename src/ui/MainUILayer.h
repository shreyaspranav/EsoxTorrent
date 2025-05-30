#pragma once

#include "core/UILayer.h"

namespace Esox
{
    class MainUILayer : public UILayer
    {
    public:
        MainUILayer();
        ~MainUILayer();

        virtual void OnPush() override;
        virtual void OnPop() override;

        virtual void UIUpdate(float timestep) override;
    };
}