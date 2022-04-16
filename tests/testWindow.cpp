#include <el/Window.hpp>
#include <iostream>
using namespace std;
using namespace EngineLibrary;

int main() {
	Window w;
	w.initWindow();

	while(w.isOpen()) {
		w.processMessages();
	}
	cout << "success" << endl;
	return 0;
}
