#include <el/PhysicalDevice.hpp>

using namespace engineLibrary::vulkan;

VkPhysicalDeviceProperties PhysicalDevice::getProperties() {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(id, &properties);
	return properties;
}
