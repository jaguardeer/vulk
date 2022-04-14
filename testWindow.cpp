#include "src/Window.hpp"

using namespace EngineLibrary;

int main() {
	Window w;
	w.InitWindow();

	while(w.isOpen()) {
		w.ProcessMessages();
	}

	return 0;
}
