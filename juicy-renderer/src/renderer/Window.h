#pragma once

namespace JR {

class Window : public Module{
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