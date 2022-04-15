#include <el/Window.hpp>

using namespace EngineLibrary;

void Window::initWindow() {
	_impl.initWindow();
}

bool Window::isOpen() const {
	return _impl.isOpen();
}

void Window::processMessages() {
	return _impl.processMessages();
}

VulkanSurfaceInfo Window::getSurfaceInfo() {
	return _impl.getSurfaceInfo();
}
