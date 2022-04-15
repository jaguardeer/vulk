#ifndef ENGINE_LIBRARY_WINDOW_IMPL
#define ENGINE_LIBRARY_WINDOW_IMPL

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace EngineLibrary {

	struct VulkanSurfaceInfo {
		HWND hwnd = nullptr;
	};

	class WindowImpl {
		public: // todo: friend class vs public?
		//friend class EngineLibrary::Window;
		// todo: return result instead of void
		void initWindow();
		bool isOpen() const;
		void processMessages();
		VulkanSurfaceInfo getSurfaceInfo();
		private:
		VulkanSurfaceInfo surfaceInfo;
		void registerWindowClass();
		static constexpr auto WINDOW_CLASS_NAME = "elWindowClass";
		static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

}      // namespace EngineLibrary
#endif // ENGINE_LIBRARY_WINDOW_IMPL
