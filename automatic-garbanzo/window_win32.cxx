#include "window_win32.hxx"
#include "input_win32.hxx"//TODO - why is this here
using namespace Graphics;

const char window_class_name[] = "la_ventana";

ATOM Register_Window_Class ()
{
	WNDCLASSEX window_class = {
		sizeof(WNDCLASSEX),			//cbsize
		0x0,						//style
		&Input_Win32::WindowProc,	//lpfnWndProc
		0,							//cbClsExtra
		0,							//cbWndExtra
		nullptr,					//hInstance
		nullptr,					//hIcon
		LoadCursor(nullptr, IDC_ARROW),//hCursor
		nullptr,					//hbrBackground
		nullptr,					//lpszMenuName
		window_class_name,			//lpszClassName
		nullptr						//hIconSm
	};

	return RegisterClassEx(&window_class);
}

window_win32::Window_Result window_win32::Create
(
	const Window_Create_Info &window_info
){
	if(Register_Window_Class() == 0) return GetLastError();
	int width  = static_cast<int>(window_info.width);
	int height = static_cast<int>(window_info.height);

	HWND hwnd = CreateWindowEx ( 
		0,							//dwExStyle
		window_class_name,			//lpClassName
		window_info.title,			//lpWindowName
		WS_SYSMENU/*WS_POPUP*/,		//dwStyle
		CW_USEDEFAULT,				//x
		CW_USEDEFAULT,				//y
		width,						//nWidth
		height,						//nHeight
		nullptr,					//hWndParent
		nullptr,					//hMenu
		nullptr,					//hInstance
		nullptr						//lParam
	);

	if(hwnd == NULL) return GetLastError();
	else return {hwnd,ERROR_SUCCESS};
}
