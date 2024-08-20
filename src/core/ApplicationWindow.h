#pragma once
#include <cstdint>
#include <string>

struct GLFWwindow;

namespace Esox
{
	class ApplicationWindow
	{
	public:
		ApplicationWindow(uint32_t width, uint32_t height, const std::string& title);
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

		std::string m_Title;
	};
}
