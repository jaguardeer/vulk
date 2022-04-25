#include <el/PhysicalDevice.hpp>

using namespace engineLibrary::vulkan;

VkPhysicalDeviceProperties PhysicalDevice::getPhysicalDeviceProperties() {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(id, &properties);
	return properties;
}
