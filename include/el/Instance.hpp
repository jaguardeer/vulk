#ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
#define ENGINE_LIBRARY_VULKAN_INSTANCE

#include <vector>
#include <vulkan/vulkan.h>

#include <el/util.hpp>
#include <el/PhysicalDevice.hpp>

namespace engineLibrary::vulkan{

	// TODO: make this global to engineLibrary?
	// list is currently std::vector, might change later
	template<class T> using list = std::vector<T>;

	class Instance {
		public:

		// currently all results returned from Instance are <T, VkResult>
		template<class T> using Result = Result<T, VkResult>;

		// public creation method (constructor is private)
		static Result<Instance> Create(const VkInstanceCreateInfo &createInfo);
		// wrapped instance functions from Vulkan API
		Result<list<PhysicalDevice>> enumeratePhysicalDevices();
		//enumeratePhysicalDeviceGroups();
		//getInstanceProcAddr();

		// special member functions
		~Instance();
		// move
		Instance& operator= (Instance&& other);
		Instance (Instance &&other);
		// non copyable
		Instance(const Instance&) = delete;
		Instance& operator= (const Instance&) = delete;

		private:
		// TODO: unprivate this? user could provide their own instance handle
		explicit Instance(const VkInstance id_);
		VkInstance id;
	};

} // namespace engineLibrary::vulkan
#endif // #ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
