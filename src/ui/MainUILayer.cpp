#include "MainUILayer.h"
#include "Base.h"
#include "core/ApplicationWindow.h"
#include "core/UIManager.h"
#include "imgui_internal.h"
#include "ui/FileDialog.h"
#include <cfloat>
#include <iostream>

#include <imgui.h>
#include <implot.h>

namespace Esox
{
    // Current Window:
    ApplicationWindow* window = nullptr;
    
    struct TestTorrent 
    {
        String name;
        float progress;
        String upload, download, eta;
    };

    MainUILayer::MainUILayer()
    {
        layerName = "MainUILayer";
    }
    MainUILayer::~MainUILayer()
    {

    }
    void MainUILayer::OnPush()
    {
        window = UIManager::GetCurrentWindow();
    }
    void MainUILayer::OnPop()
    {

    }
   void MainUILayer::UIUpdate(float timestep)
    {
        // Test Data:
        TestTorrent torrents[] = {
            {"GTAIV.Razor1911.Crack", 24.53f, "1.2kB/s", "3.2MB/s", "1m04s"},
            {"Oppenheimer - 2160p.IMAX", 54.53f, "6.2kB/s", "4.2MB/s", "1m04s"},
            {"Bob the Builder", 100.0f, "5kB/s", "1.6MB/s", "1m04s"},
            {"GTAV.Razor1911.Crack", 45.6f, "2kB/s", "4.1MB/s", "1m04s"},
        };
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        // Torrents panel: (Shows all the torrents and its download progress) ----------------------------------
        ImGui::Begin("Torrents", NULL, windowFlags);
        
        float data[1000];
        float x[1000];
        int last = 720;
        for(int i = 0; i < 1000; i++)
        {
            x[i] = i;

            double angle = i * 3.1416 / 180.0;
            data[i] = sin(angle) * 2.0f;
        }

        ImGui::SetWindowSize(
            "Torrents",
            {(float)window->GetFrambufferWidth(), (float)window->GetFrambufferHeight() / 2},
            ImGuiCond_Always
        );
        ImGui::Text("EsoxTorrent");
        ImGui::SameLine(((float)window->GetFrambufferWidth() / 2 - ImGui::CalcTextSize("EsoxTorrent").x));

        ImGui::PlotLines("#Test", data, 1000, 0, NULL, FLT_MAX, FLT_MAX, {300, 70});

        ImGui::Separator();

        ImGuiTableColumnFlags f = ImGuiTableColumnFlags_WidthStretch;
        ImGui::BeginTable("##Torrents", 6);
        ImGui::TableSetupColumn("Name", f, 0.3f);
        ImGui::TableSetupColumn("Progress", f, 0.4f);
        ImGui::TableSetupColumn("Download", f, 0.1f);
        ImGui::TableSetupColumn("Upload", f, 0.1f);
        ImGui::TableSetupColumn("ETA", f, 0.05f);
        ImGui::TableSetupColumn("", f, 0.05f);
        ImGui::TableHeadersRow();

        for(auto&& row : torrents)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(row.name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::ProgressBar(row.progress / 100.0f);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text(row.download.c_str());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text(row.upload.c_str());

            ImGui::TableSetColumnIndex(4);
            ImGui::Text(row.eta.c_str());

            ImGui::TableSetColumnIndex(5);
            ImGui::Button("Stop");

        }
        ImGui::EndTable();

        ImGui::End();

        // Details panel: (Shows details of a selected torrent(download/upload speeds, files, peers etc)) ------
        ImGui::Begin("Details", NULL, windowFlags);
        ImGui::SetWindowPos("Details", {0, (float)window->GetFrambufferHeight() / 2}, ImGuiCond_Always);
        ImGui::SetWindowSize(
            "Details",
            {(float)window->GetFrambufferWidth(), (float)window->GetFrambufferHeight() / 2},
            ImGuiCond_Always
        );

        ImGui::Text("fdhjsahfuoesruofheshuo");
        ImGui::Text("Frametime: %f", timestep);
        if (ImGui::Button("Test Button"))
        {
            String s = FileDialog::Open();
            ESOX_LOG_INFO("%s", s.c_str());
        }

        ImGui::End();

        ImGui::ShowDemoWindow();
    }
}
