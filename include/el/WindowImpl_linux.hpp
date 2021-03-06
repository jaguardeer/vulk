#ifndef ENGINE_LIBRARY_WINDOW_IMPL
#define ENGINE_LIBRARY_WINDOW_IMPL

#include <xcb/xcb.h>

namespace engineLibrary {

	struct VulkanSurfaceInfo {
		xcb_connection_t* connection = nullptr;
		xcb_window_t window = 0;
	};

	class WindowImpl {
		public: // todo: friend class vs public?
			//friend class engineLibrary::Window;
			// todo: return result instead of void
			void initWindow();
			bool isOpen() const;
			void processMessages();
			VulkanSurfaceInfo getSurfaceInfo();
		private:
			VulkanSurfaceInfo _surfaceInfo;
	};

}      // namespace engineLibrary
#endif // ENGINE_LIBRARY_WINDOW_IMPL
