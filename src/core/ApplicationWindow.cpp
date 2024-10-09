#include "ApplicationWindow.h"
#include "Base.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace Esox
{
	// Common Configuration, tweak them if you want to --------------------------------------------------------------------
	
	static bool vSync = true;
	
	// --------------------------------------------------------------------------------------------------------------------

	ApplicationWindow::ApplicationWindow(uint32_t width, uint32_t height, const String& title)
		:m_Width(width), m_Height(height), m_Title(title)
	{

		bool initialized = glfwInit();
		ENOX_ASSERT(initialized);

		m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		// Center the window: 
		const GLFWvidmode* primaryMonitorVidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		uint32_t xWindowPos = (primaryMonitorVidMode->width - width) / 2;
		uint32_t yWindowPos = (primaryMonitorVidMode->height - height) / 2;

		glfwSetWindowPos(m_Window, xWindowPos, yWindowPos);

		// Create the OpenGL context
		glfwMakeContextCurrent(m_Window);
		
		// Set vsync
		glfwSwapInterval(vSync);

		// Show the window
		glfwShowWindow(m_Window);
	}
	ApplicationWindow::~ApplicationWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void ApplicationWindow::ClearWindow()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	}

	void ApplicationWindow::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void ApplicationWindow::SetWindowCloseCallback()
	{

	}
	bool ApplicationWindow::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}
}