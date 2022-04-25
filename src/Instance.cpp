#include <el/Instance.hpp>

#include <cstddef>
#include <type_traits>

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

Instance::Result<list<PhysicalDevice>> Instance::enumeratePhysicalDevices() {
	uint32_t numDevices;
	{	// return early if querying # devices fails
		const auto error = vkEnumeratePhysicalDevices(id, &numDevices, nullptr);
		if(error != VK_SUCCESS) return {{}, error};
	}
	list<PhysicalDevice> devices{numDevices};
	// reinterpret_cast checks
	static_assert(std::is_standard_layout<PhysicalDevice>());
	static_assert(sizeof(PhysicalDevice) == sizeof(VkPhysicalDevice));
	const auto error = vkEnumeratePhysicalDevices(id, &numDevices, reinterpret_cast<VkPhysicalDevice*>(devices.data()));
	return {devices, error};
}


// special member functions
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
}
