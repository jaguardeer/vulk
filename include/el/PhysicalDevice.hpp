#ifndef ENGINE_LIBRARY_VULKAN_PHYSICAL_DEVICE
#define ENGINE_LIBRARY_VULKAN_PHYSICAL_DEVICE

#include <vulkan/vulkan.h>
#include <el/util.hpp>

namespace engineLibrary::vulkan{
	class PhysicalDevice{
		public:

		VkPhysicalDeviceProperties getProperties() const;
		list<VkQueueFamilyProperties> getQueueFamilyProperties() const;

		explicit operator VkPhysicalDevice();

		PhysicalDevice();
		explicit PhysicalDevice(VkPhysicalDevice id_);

		private:
		VkPhysicalDevice id;
	};
} // namespace engineLibrary::vulkan
#endif // #ifndef ENGINE_LIBRARY_VULKAN_PHYSICAL_DEVICE

