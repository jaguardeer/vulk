#ifndef WINDOW_WIN32_HXX
#define WINDOW_WIN32_HXX
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

//TODO: rename this to Window_WIN or so
extern "C" {
#include <Windows.h>
}
#include <cstdint>
#include "result.hxx"

namespace Graphics{
//This would go into window.hxx along with the window pimpl class
struct Window_Create_Info
{
	char const	*title;
	uint32_t	width;
	uint32_t	height;
};


//TODO: should this be a window_handle?
//		move show()+hide() into a pimpl class. and implement proper move/copy
class window_win32
{
public:
	void show() {ShowWindow(hwnd, SW_SHOW);}
	void hide() {ShowWindow(hwnd, SW_HIDE);}

	using Window_Result = engine_library::Result<window_win32, DWORD>;

	static Window_Result Create(const Window_Create_Info &window_info);

	HWND handle() {return hwnd;}

	window_win32() = default;
	window_win32(const window_win32&) = default;
private:
	window_win32(const HWND _hwnd) : hwnd(_hwnd) {}
	window_win32(window_win32&&) = default;
	HWND hwnd;
};

}//namespace Graphics
#endif
