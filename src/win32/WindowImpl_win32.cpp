#include <el/WindowImpl_win32.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace EngineLibrary;

// todo: move to util?
static auto PrintWindowsError(const char* extra_message) {
	const auto error_code = GetLastError();
	LPTSTR error_msg;
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,// [in]           DWORD   dwFlags,
			nullptr,                                                    // [in, optional] LPCVOID lpSource,
			error_code,                                                 // [in]           DWORD   dwMessageId,
			0,                                                          // [in]           DWORD   dwLanguageId,
			reinterpret_cast<LPTSTR>(&error_msg),                       // [out]          LPTSTR  lpBuffer,
			0,                                                          // [in]           DWORD   nSize,
			nullptr                                                     // [in, optional] va_list *Arguments
			);
	cout << extra_message << endl;
	cout << "(" << error_code << "): " << error_msg << endl;
	LocalFree(error_msg);
}

LRESULT CALLBACK WindowImpl::wndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WindowImpl::registerWindowClass() {
	 // todo: research GetModuleHandle()
	const auto hInstance = GetModuleHandle(nullptr);
	{
		WNDCLASSEX class_info;
		if(GetClassInfoEx(hInstance, this->WINDOW_CLASS_NAME, &class_info) == true) {
			cout << "window class is already registered" << endl;
			return;
		}
	}
	const WNDCLASSEX window_class = {            //        the window proc"
		.cbSize = sizeof(window_class),
		.style = 0,
		.lpfnWndProc = wndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = nullptr, 
		.hCursor = nullptr,// todo - load default icon
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = this->WINDOW_CLASS_NAME,
		.hIconSm = nullptr
	};
	auto class_atom = RegisterClassEx(&window_class);
	if(class_atom == 0) PrintWindowsError("failed to register window class: ");
}

bool WindowImpl::isOpen() const {
	return IsWindowVisible(this->surfaceInfo.hwnd);
}

void WindowImpl::processMessages() {
	MSG msg;
	while(PeekMessage(&msg, this->surfaceInfo.hwnd, 0, 0, PM_REMOVE)) DispatchMessage(&msg);
}

void WindowImpl::initWindow() {
	if(this->surfaceInfo.hwnd != nullptr) {
		cout << "already created game window";
		return;
	}
	this->registerWindowClass();
	const auto hInstance = GetModuleHandle(nullptr); // todo - research this
	auto hwnd = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,           // [in]           DWORD     dwExStyle,
			this->WINDOW_CLASS_NAME,                            // [in, optional] LPCSTR    lpClassName,
			"Test Window",                                      // [in, optional] LPCSTR    lpWindowName,
			WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_VISIBLE,      // [in]           DWORD     dwStyle,
            CW_USEDEFAULT,                          			// [in]           int       X,
            CW_USEDEFAULT,                          			// [in]           int       Y,
            1024,                                   			// [in]           int       nWidth,
            768,                                    			// [in]           int       nHeight,
            nullptr,                                   			// [in, optional] HWND      hWndParent,
            nullptr,                                   			// [in, optional] HMENU     hMenu,
            hInstance,                              			// [in, optional] HINSTANCE hInstance,
            nullptr                                    			// [in, optional] LPVOID    lpParam
			);
	if(hwnd == nullptr) PrintWindowsError("failed to create game window: ");
	this->surfaceInfo.hwnd = hwnd;
}

VulkanSurfaceInfo WindowImpl::getSurfaceInfo() {
	return surfaceInfo;
}
