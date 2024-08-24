#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "ApplicationWindow.h"

namespace Esox
{
	// Struct that holds the command-line arguments of the application, used a raw string array
	// because the the arguments lifetime is guarenteed.
	struct ApplicationArgs
	{
		uint32_t applicationArgCount;
		const char** applicationArgs;
	};

	// Struct that holds the configuration of the application, 
	// To be created and passed when Application instance is being created.
	struct ApplicationConfig
	{
		ApplicationArgs* args;

		uint32_t windowWidth, windowHeight;
		std::string windowTitle;
	};

	class Application
	{
	public:
		Application(const ApplicationConfig& config);
		~Application();

		void Run();

	private:
		void Start();

		void Update(float timestep);
		void UIUpdate(float timestep);

		void Finish();

	private:
		ApplicationConfig m_Config;
		ApplicationWindow* m_AppWindow;
	};
}