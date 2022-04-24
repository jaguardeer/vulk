#include <el/Instance.hpp>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

Instance::operator VkInstance() const {
	return id;
}

Instance::Instance(const VkInstance id_) : id{id_} {
}

Instance::~Instance() {
	vkDestroyInstance(id, nullptr);
}

Instance& Instance::operator= (Instance &&other) {
	this->id = other.id;
	other.id = VK_NULL_HANDLE;
	return *this;
}

Instance::Instance(Instance&& other) {
	this->id = other.id;
	other.id = VK_NULL_HANDLE;
}

Result<Instance, VkResult> Instance::Create(const VkInstanceCreateInfo &createInfo) {
	VkInstance id = VK_NULL_HANDLE;
	auto error = vkCreateInstance(&createInfo, nullptr, &id);
	return Result {Instance{id}, error};
	// TODO: can it work without std::move? probably not, std::tuple can't do it
	//Instance instance{VK_NULL_HANDLE};
	//auto error = vkCreateInstance(&createInfo, nullptr, &instance.id);
	//return {std::move(instance), error};
}

/*
std::tuple<Instance, VkResult> Instance::CreateT(const VkInstanceCreateInfo &createInfo) {
	Instance instance{VK_NULL_HANDLE};
	auto error = vkCreateInstance(&createInfo, nullptr, &instance.id);
	return {std::forward<Instance> (instance), error}; // this line works with either move or forward, but not neither
}
*/
