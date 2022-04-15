#include <el/Window.hpp>

using namespace EngineLibrary;

int main() {
	Window w;
	w.initWindow();

	while(w.isOpen()) {
		w.processMessages();
	}

	return 0;
}
