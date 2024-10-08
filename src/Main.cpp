#include <iostream>

#include "core/Application.h"

// This is the main entry point of the application.
// Defined at the end of the file.
int EntryPoint(int argc, char** argv);

// Using WinMain instead of main to disable that command window.
#if defined(_WIN32) && defined(NDEBUG)
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	return EntryPoint(nCmdShow, &lpCmdLine);
}

#else

int main(int argc, char** argv)
{
	return EntryPoint(argc, argv);
}

#endif

int EntryPoint(int argc, char** argv)
{
	Esox::ApplicationArgs args = { argc, (const char**)argv };

	Esox::ApplicationConfig config;
	config.args = &args;
	config.windowWidth = 1600;
	config.windowHeight = 900;
	config.windowTitle = "EsoxTorrent: A Torrenting Application";

	Esox::Application* app = new Esox::Application(config);
	app->Run();
	delete app;

	return 0;
}
