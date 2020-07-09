#include "Window.h"

namespace JR {

static WNDPROC currentWndProc;

static LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	return CallWindowProc(currentWndProc, hWnd, msg, wParam, lParam);
}

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

	glfwSetWindowUserPointer(mWindow, this);

	glfwSetFramebufferSizeCallback(mWindow, WindowFramebufferSizeCallback);

	currentWndProc = (WNDPROC)GetWindowLongPtr(GetHandle(), -4);
	SetWindowLongPtr(GetHandle(), -4, (LONG_PTR)MyWndProc);

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

void Window::WindowFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	auto userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!userWindow) {
		return;
	}

	userWindow->mWidth = width;
	userWindow->mHeight = height;

	userWindow->Emit(EventResize{width, height});
}

}  // namespace JR