import EngineLibrary.Window;


int main() {
	Window w;
	w.CreateWindow();

	while(w.isOpen()) {
		w.ProcessMessages();
	}

	return 0;
}
