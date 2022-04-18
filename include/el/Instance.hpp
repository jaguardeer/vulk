#ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
#define ENGINE_LIBRARY_VULKAN_INSTANCE

#include <vulkan/vulkan.h>
#include <el/util.hpp>

namespace engineLibrary::vulkan{

	class Instance {
		public:

		static Result<Instance, VkResult> Create(VkInstanceCreateInfo createInfo);

		// move assign
		Instance& operator= (Instance&& other) { this->id = other.id; other.id = VK_NULL_HANDLE; return *this; }
		// move construct
		Instance (Instance &&other);

		~Instance();

		// non copyable
		explicit Instance(const Instance&) = default;//delete;
		Instance& operator= (const Instance&) = default;//delete;

		private:
		explicit Instance(const VkInstance id_) : id{id_} {}

		VkInstance id;
	};

} // namespace engineLibrary::vulkan
#endif // #ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
