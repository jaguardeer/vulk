module;

module EngineLibrary.Window;

// auto Window::GetHandle() {return _hwnd;}
// auto Window::isOpen();
// auto Window::ProcessMessages(); // todo: move this out of Window

auto Window::CreateWindow() {
	const auto xcb_connection = xcb_connect(NULL, NULL);

}
