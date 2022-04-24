#ifndef ENGINE_LIBRARY_WINDOW
#define ENGINE_LIBRARY_WINDOW

#ifdef _WIN32
#include "WindowImpl_win32.hpp"
#endif // #ifdef _WIN32
#ifdef __linux__
#include "WindowImpl_linux.hpp"
#endif // #ifdef linux

namespace engineLibrary{
	class Window {
		// TODO: return result instead of void
		public:
			void initWindow();
			bool isOpen() const;
			void processMessages(); // TODO: move this elsewhere?
			VulkanSurfaceInfo getSurfaceInfo(); // returns info needed to build the appropriate Vk**SurfaceCreateInfo struct
		private:
			engineLibrary::WindowImpl _impl;
	};
}      // namespace engineLibrary
#endif // ENGINE_LIBRARY_WINDOW
