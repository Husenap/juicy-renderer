#include "Window.h"

namespace JR {

bool Window::Create(const std::string& title, int width, int height) {
	if (!glfwInit()) {
		return false;
	}

	mTitle  = title;
	mWidth  = width;
	mHeight = height;

	mWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), NULL, NULL);

	if (!mWindow) {
		return false;
	}

	return true;
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(mWindow);
}

void Window::SwapBuffers() const {
	glfwSwapBuffers(mWindow);
}

HWND Window::GetHandle() const {
	return glfwGetWin32Window(mWindow);
}

}  // namespace JR