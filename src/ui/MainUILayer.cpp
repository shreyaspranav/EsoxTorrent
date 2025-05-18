#include "MainUILayer.h"
#include "Base.h"
#include "ui/FileDialog.h"
#include <iostream>

#include <imgui.h>

namespace Esox
{
	MainUILayer::MainUILayer()
	{
		layerName = "MainUILayer";
	}
	MainUILayer::~MainUILayer()
	{

	}
	void MainUILayer::OnPush()
	{

	}
	void MainUILayer::OnPop()
	{

	}
	void MainUILayer::UIUpdate(float timestep)
	{
		ImGui::Begin("Test Window");

		ImGui::Text("This is going to be a torrenting application.");
		ImGui::Text("Frametime: %f", timestep);
		if (ImGui::Button("Test Button"))
        {
            String s = FileDialog::Open();
            ESOX_LOG_INFO("%s", s.c_str());
        }

		ImGui::End();
	}
}
