#pragma once

namespace JR {

struct EventResize {
	int width;
	int height;
};

struct EventKey {
	int key;
	int scancode;
	int action;
	int mods;
};

struct EventKeyPress {
	int key;
	int scancode;
	int mods;
};

struct EventKeyRelease {
	int key;
	int scancode;
	int mods;
};

struct EventDroppedFile {
	std::string filepath;
};

class Window : public Module, public MessageEmitter {
public:
	bool Create(const std::string& title, int width, int height);

	void SetWindowIcon();

	bool ShouldClose() const;

	void SwapBuffers() const;

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	glm::vec2 GetSize() const { return {mWidth, mHeight}; }

	HWND GetHandle() const;

private:
	static void WindowFramebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void WindowDropCallback(GLFWwindow* window, int count, const char** paths);

	std::string mTitle;
	int mWidth;
	int mHeight;

	GLFWwindow* mWindow;
};

}  // namespace JR