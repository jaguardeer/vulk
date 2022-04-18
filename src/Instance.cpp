#include <el/Instance.hpp>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

Instance::Instance(Instance&& other) : id{other.id} {
	other.id = VK_NULL_HANDLE;
}

Instance::~Instance() {
	if(id != VK_NULL_HANDLE) vkDestroyInstance(id, nullptr);
}

Result<Instance, VkResult> Instance::Create(VkInstanceCreateInfo createInfo) {
	VkInstance id = VK_NULL_HANDLE;
	auto error = vkCreateInstance(&createInfo, nullptr, &id);
	return {Instance(id), error};
}
