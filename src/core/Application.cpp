#include "Application.h"
#include "UIManager.h"

// UI layers:
#include "ui/MainUILayer.h"

namespace Esox
{
	Application::Application(const ApplicationConfig& config)
		:m_Config(config)
	{
		
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		Application::Start();

		while (!m_AppWindow->WindowShouldClose())
		{
			Application::Update(0.0f);
			Application::UIUpdate(0.0f);

			m_AppWindow->SwapBuffers();
		}

		Application::Finish();
	}

	// Private Methods: -------------------------------------------------------------------------------------------
	void Application::Start()
	{
		m_AppWindow = new ApplicationWindow(m_Config.windowWidth, m_Config.windowHeight, m_Config.windowTitle);
		UIManager::Initialize(m_AppWindow);

		UILayer* mainLayer = new MainUILayer();
		UIManager::PushUILayer(mainLayer);
	}

	void Application::Update(float timestep)
	{
		m_AppWindow->ClearWindow();
	}

	void Application::UIUpdate(float timestep)
	{
		UIManager::RenderUI(timestep);
	}

	void Application::Finish()
	{
		UIManager::Finalize();
	}
}


