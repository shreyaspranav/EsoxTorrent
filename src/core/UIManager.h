#pragma once
#include "Base.h"

#include "UILayer.h"
#include "ApplicationWindow.h"

namespace Esox
{
    class UIManager
    {
    public:
        static void Initialize(ApplicationWindow* window);

        // Pushes the layer to the end of the stack.
        static void PushUILayer(UILayer* layer);

        // Here if index is non negative(0, 1, 2, ....) then the layers are indexed from the bottom
        // if index is negative, the layers are indexed from the top.
        static void PopUILayer(int32_t index = -1);
        
        // Renders all the UI in the layer stack
        static void RenderUI(float timestep);

        static ApplicationWindow* GetCurrentWindow();

        static void Finalize();
    private:
    };
}
