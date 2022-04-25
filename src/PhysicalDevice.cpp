#include <el/PhysicalDevice.hpp>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

VkPhysicalDeviceProperties PhysicalDevice::getProperties() const {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(id, &properties);
	return properties;
}

list<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilyProperties() const {
	uint32_t numFamilies;
	vkGetPhysicalDeviceQueueFamilyProperties(id, &numFamilies, nullptr);
	list<VkQueueFamilyProperties> queueFamilies(numFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(id, &numFamilies, queueFamilies.data());
	return queueFamilies;
}
