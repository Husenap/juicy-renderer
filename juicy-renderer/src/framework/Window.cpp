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
	glfwSetKeyCallback(mWindow, WindowKeyCallback);
	glfwSetDropCallback(mWindow, WindowDropCallback);
	glfwSetWindowContentScaleCallback(mWindow, WindowContentScaleCallback);

	SetWindowIcon();

	currentWndProc = (WNDPROC)GetWindowLongPtr(GetHandle(), -4);
	SetWindowLongPtr(GetHandle(), -4, (LONG_PTR)MyWndProc);

	return true;
}

void Window::SetWindowIcon() {
	GLFWimage icon;
	int channels;
	icon.pixels = stbi_load("assets/textures/icon.png", &icon.width, &icon.height, &channels, 4);

	if (!icon.pixels) {
		LOG_ERROR("Failed to find icon file for window!");
		return;
	}

	glfwSetWindowIcon(mWindow, 1, &icon);
	stbi_image_free(icon.pixels);
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

	userWindow->mWidth  = width;
	userWindow->mHeight = height;

	userWindow->Emit(EventResize{width, height});
}

void Window::WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!userWindow) {
		return;
	}

	userWindow->Emit(EventKey{key, scancode, action, mods});

	if (action == GLFW_PRESS) {
		userWindow->Emit(EventKeyPress{key, scancode, mods});
	} else if (action == GLFW_RELEASE) {
		userWindow->Emit(EventKeyRelease{key, scancode, mods});
	}
}

void Window::WindowDropCallback(GLFWwindow* window, int count, const char** paths) {
	auto userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!userWindow) {
		return;
	}

	for (int i = 0; i < count; ++i) {
		userWindow->Emit(EventDroppedFile{.filepath = paths[i]});
	}
}

void Window::WindowContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY) {
	auto userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!userWindow) {
		return;
	}

	userWindow->Emit(EventContentScale{glm::vec2(scaleX, scaleY)});
}

}  // namespace JR