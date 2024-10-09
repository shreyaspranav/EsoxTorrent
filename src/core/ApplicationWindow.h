#pragma once
#include "Base.h"

struct GLFWwindow;

namespace Esox
{
	class ApplicationWindow
	{
	public:
		ApplicationWindow(uint32_t width, uint32_t height, const String& title);
		~ApplicationWindow();

		void ClearWindow();
		void SwapBuffers();

		void SetWindowCloseCallback();

		// Temp:
		bool WindowShouldClose();

		inline GLFWwindow* GetNativeWindow() { return m_Window; }
	private:
		GLFWwindow* m_Window;
		uint32_t m_Width, m_Height;

		String m_Title;
	};
}
