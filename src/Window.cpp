#include "Window.hpp"

using namespace EngineLibrary;

void Window::InitWindow() {
	_impl.InitWindow();
}

windowHandle Window::GetHandle() {
	return _impl.hwnd;
}

bool Window::isOpen() {
	return _impl.isOpen();
}

void Window::ProcessMessages() {
	return _impl.ProcessMessages();
}
