#include <iostream>
#include <thread>
// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

using namespace std;
using namespace engineLibrary;
using namespace engineLibrary::vulkan;

/*
	rendering to a window:
		create instance
		- debug callbacks?
		window surface
		vulkan device
		command pool
		swapchain
		render pass
		descriptor layouts
		pipeline layou
		graphics pipeline
		swapchain view
		depth buffer
		framebuffers
		draw buffer
		semaphores
		texture sampler
		uniforms
		descriptor sets
 */

static auto msg (const char* m) { cout << m << endl; }

template<class T> auto check(const T result);

template<>
auto check (const VkResult result) {
	if(result != VK_SUCCESS) {
		std::cout << "vulkan error " << result << ": "
			<< GetVkResultString(result) << std::endl;
	}
	return result;
}

template<class T>
auto check(const Result<T, VkResult> result) {
	check(result.error);
	return result.value;
}

static constexpr auto CreateAppInfo() {
	constexpr VkApplicationInfo appInfo = {
		.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName    = "test app",
		.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName         = "test engine",
		.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion          = VK_API_VERSION_1_0,
	};
	return appInfo;
}

static constexpr auto GetLayersNeeded() {
	// TODO: layers & extensions
	// could check availability before asking
	constexpr std::array layersNeeded = {"VK_LAYER_KHRONOS_validation"};
	return layersNeeded;
}

static auto CreateInstanceInfo() {
	static constexpr auto appInfo = CreateAppInfo();
	static constexpr auto layersNeeded = GetLayersNeeded();
	constexpr VkInstanceCreateInfo instanceInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = layersNeeded.size(),
		.ppEnabledLayerNames = layersNeeded.data(),
	};
	return instanceInfo;
}

static auto CreateVulkanInstance() {
	auto instanceInfo = CreateInstanceInfo();
	VkInstance instance;
	VkResult error = vkCreateInstance(&instanceInfo, nullptr, &instance);
	const Result<VkInstance, VkResult> result {instance, error};
	return result;
}

static auto GetDeviceProperties(const VkPhysicalDevice device) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	cout << properties.deviceName << endl;
}

static auto GetAvailableDevices(const VkInstance instance) {
	uint32_t numDevices;
	auto result = check(vkEnumeratePhysicalDevices(instance, &numDevices, nullptr));
	std::vector<VkPhysicalDevice> devices(numDevices);
	result = check(vkEnumeratePhysicalDevices(instance, &numDevices, devices.data()));
	for(const auto& device : devices) {
		cout << device << endl;
		GetDeviceProperties(device);
	}
}

static auto CreatePhysicalDevice(const VkInstance instance) {
	GetAvailableDevices(instance);
	int a;
	VkResult b;
	return Result{a, b};
}

int main() {
	msg("making main window...");
	Window gameWindow;
	gameWindow.initWindow();

	msg("intializing vulkan...");
	auto instance = check(CreateVulkanInstance());
	auto pdevice = check(CreatePhysicalDevice(instance));

	msg("main loop...");
	while(gameWindow.isOpen()) {
		gameWindow.processMessages();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	msg("cleaning up...");
	vkDestroyInstance(instance, nullptr);
	return 0;
}
