#include <el/Instance.hpp>

#include <cstddef>
#include <type_traits>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

Instance::Result<list<PhysicalDevice>> Instance::enumeratePhysicalDevices() {
	uint32_t numDevices;
	const auto error = vkEnumeratePhysicalDevices(id, &numDevices, nullptr);
	list<PhysicalDevice> devices{numDevices};
	// reinterpret_cast checks
	static_assert(std::is_standard_layout<PhysicalDevice>());
	static_assert(sizeof(PhysicalDevice) == sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(id, &numDevices, reinterpret_cast<VkPhysicalDevice*>(devices.data()));
	return {devices, error};
}


// constructors
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
	return {Instance{id}, error};
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
