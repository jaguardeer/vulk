#include "input_win32.hxx"
#include <iostream>

//todo
extern void OnMouseMoveRaw (LONG x_delta, LONG y_delta);
extern void OnMouseWheelRaw	(USHORT mouse_wheel_movement);
extern void OnKeyDown (WPARAM wparam);
extern void OnKeyUp (WPARAM wparam);
extern void OnMouseMove (LPARAM lparam);

bool Input_Win32::Key_Is_Down (int key)
{
	//If the high-order bit is 1, the key is down; otherwise, it is up.
	bool key_state = GetKeyState(key) & 0x800000;
	return key_state;
}

void Input_Win32::Process_Message_Queue ()
{
	MSG msg;
	while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

namespace{
void Handle_RawInput 		(WPARAM wparam, LPARAM lparam);
void Handle_RawMouse 		(RAWMOUSE &mouse_data);
void Handle_RawMouseButtons (RAWMOUSE &mouse_data);
}

LRESULT CALLBACK Input_Win32::WindowProc
(
	HWND hwnd,
	UINT msg_id,
	WPARAM wparam,
	LPARAM lparam
){
	switch ( msg_id )
	{
		case WM_CLOSE:
		exit(0);//todo
		return 0;

		case WM_KEYDOWN:
		OnKeyDown(wparam);
		return 0;

		case WM_KEYUP:
		OnKeyUp(wparam);
		return 0;

		case WM_MOUSEMOVE:
		OnMouseMove(lparam);
		return 0;

		case WM_INPUT:
		Handle_RawInput(wparam,lparam);
		return DefWindowProc ( hwnd, msg_id, wparam, lparam );//"for cleanup"

		default:
		return DefWindowProc ( hwnd, msg_id, wparam, lparam );//default wndproc
	}
}

void Input_Win32::Register_RawInput ()
{
	const RAWINPUTDEVICE devices[] = {
		0x01,//usUsagePage	for UsagePage and Usage see:
		0x02,//usUsage		docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
		0,//dwFlags
		nullptr,//hwndTarget

		/*keybord
	devices[1].usUsagePage = 0x01; 
	devices[1].usUsage = 0x06; 
	devices[1].dwFlags = RIDEV_NOLEGACY;  // adds HID keyboard and also ignores legacy keyboard messages
	devices[1].hwndTarget = 0;
		*/
	};

	const size_t size_per = sizeof(RAWINPUTDEVICE);
	if( ! RegisterRawInputDevices(devices,sizeof(devices)/size_per,size_per) )
		std::cout << "failure\n";
}

namespace{
void Handle_RawInput(WPARAM wparam, LPARAM lparam)
{
	UINT data_size = sizeof(RAWINPUT);
	//get data - should check data_size with GetRawInputData()
	RAWINPUT raw_input;
	GetRawInputData(
		reinterpret_cast<HRAWINPUT>(lparam),
		RID_INPUT,
		&raw_input,
		&data_size,
		sizeof(RAWINPUTHEADER)
	);

	switch(raw_input.header.dwType)
	{
		case RIM_TYPEKEYBOARD:
		break;

		case RIM_TYPEMOUSE:
		Handle_RawMouse(raw_input.data.mouse);
		break;

		default:
		break;
	}
}

void Handle_RawMouse(RAWMOUSE &mouse_data)
{
	if(mouse_data.usFlags == MOUSE_MOVE_RELATIVE)
	{
		if(mouse_data.usButtonFlags)
		{
			Handle_RawMouseButtons(mouse_data);
		}else{
			OnMouseMoveRaw(mouse_data.lLastX, mouse_data.lLastY);
		}
	}
	else std::cout << "Unexpected mouse data." << std::endl;
}

void Handle_RawMouseButtons(RAWMOUSE &mouse_data)
{
	if(mouse_data.usButtonFlags | RI_MOUSE_WHEEL)
	{
		OnMouseWheelRaw(mouse_data.usButtonData);
	}
}
}
