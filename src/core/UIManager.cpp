#include "UIManager.h"
#include "core/ApplicationWindow.h"

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl2.h>

namespace Esox
{
    struct UIManagerData
    {
        Vector<UILayer*> uiLayers;
        uint8_t defaultFontSize = 17;

        ApplicationWindow* window = nullptr;
    }uiManagerState;

    void UIManager::Initialize(ApplicationWindow* window)
    {
        ImGui::CreateContext();
        ImPlot::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.Fonts->AddFontFromFileTTF("resources/fonts/DroidSans.ttf", uiManagerState.defaultFontSize);

        ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
        ImGui_ImplOpenGL2_Init();

        uiManagerState.window = window;
    }
    void UIManager::PushUILayer(UILayer* layer)
    {
        uiManagerState.uiLayers.push_back(layer);
        layer->OnPush();
    }
    void UIManager::PopUILayer(int32_t index)
    {
        // TODO: Implement this!
    }
    void UIManager::RenderUI(float timestep)
    {
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render all your layers here: 
        for (auto uiLayer : uiManagerState.uiLayers)
            uiLayer->UIUpdate(timestep);

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }
    ApplicationWindow* UIManager::GetCurrentWindow()
    {
        return uiManagerState.window;
    }
    void UIManager::Finalize()
    {
        // Delete all the layers by yourself.
        for (auto layer : uiManagerState.uiLayers)
            delete layer;
    }
}
