#include <App.h>

int main() {
	JR::App app;

	if (!app.Start()) {
		return 1;
	}

	return 0;
}