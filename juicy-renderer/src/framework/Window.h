#pragma once

namespace JR {

struct EventResize {
	int width;
	int height;
};

class Window : public Module, public MessageEmitter {
public:
	bool Create(const std::string& title, int width, int height);

	bool ShouldClose() const;

	void SwapBuffers() const;

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	HWND GetHandle() const;

private:
	std::string mTitle;
	int mWidth;
	int mHeight;

	GLFWwindow* mWindow;
};

}  // namespace JR