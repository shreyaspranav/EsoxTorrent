#include "Application.h"
#include "UIManager.h"
#include "Timer.h"

#include "torrent/TorrentHandler.h"

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

		float frameTime = 0.0f;

		while (!m_AppWindow->WindowShouldClose())
		{
			Timer t;

			Application::Update(frameTime);
			Application::UIUpdate(frameTime);

			m_AppWindow->SwapBuffers();
			t.Stop();
			
			frameTime = t.GetElapedTime() / 1000.0f;
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

		TorrentHandler::Init();
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


