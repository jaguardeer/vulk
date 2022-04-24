#ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
#define ENGINE_LIBRARY_VULKAN_INSTANCE

#include <vulkan/vulkan.h>
#include <el/util.hpp>


namespace engineLibrary::vulkan{

	class Instance {
		public:

		static Result<Instance, VkResult> Create(const VkInstanceCreateInfo &createInfo);

		Instance() = delete;
		~Instance();
		// move
		Instance& operator= (Instance&& other);
		Instance (Instance &&other);
		// non copyable
		Instance(const Instance&) = delete;
		Instance& operator= (const Instance&) = delete;

		private:

		explicit Instance(const VkInstance id_);
		VkInstance id;
	};

} // namespace engineLibrary::vulkan
#endif // #ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
