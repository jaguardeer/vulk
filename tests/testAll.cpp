#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/util.hpp>

using std::cout;
using std::endl;

using namespace engineLibrary;

auto PrintVulkanResult(VkResult result, const char* extra_message = nullptr) {
	if(extra_message != nullptr) cout << extra_message << endl;
	cout << "(" << result << "): " << GetVkResultString(result) << endl;
}

class Instance {
	public:

	static Result<Instance, VkResult> Create(VkInstanceCreateInfo createInfo) {
		VkInstance id = VK_NULL_HANDLE;
		auto error = vkCreateInstance(&createInfo, nullptr, &id);
		return {Instance(id), error};
	}

	// move assign
	Instance& operator= (Instance&& other) { this->id = other.id; other.id = VK_NULL_HANDLE; return *this; }
	// move construct
	Instance (Instance &&other) : id(other.id) { other.id = VK_NULL_HANDLE; }

	~Instance() {
		cout << "destructing instance" << endl;
		if(id != VK_NULL_HANDLE) vkDestroyInstance(id, nullptr);
	}

	explicit Instance(const Instance&) { cout << "copied an Instance :(" << endl; }// = default;//delete;
	Instance& operator= (const Instance&) { cout << "copied an Instance :(" << endl; return *this; }// = //default;//delete;

	private:
	// non copyable
	// 
	explicit Instance(const VkInstance id) : id{id} {}

	VkInstance id;
};

Result<Instance, VkResult> CreateVulkanInstance() {
	static constexpr VkApplicationInfo app_info = {
		.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName    = "test app",
		.applicationVersion  = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName         = "test engine",
		.engineVersion       = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion          = VK_API_VERSION_1_0,
	};

	// TODO: layers & extensions
	static constexpr VkInstanceCreateInfo instance_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
	};

	const auto result = Instance::Create(instance_info);
	return result;
}


int main() {
	auto msg = [](const char* m) { cout << m << endl; };
	{
		msg("running app...");

		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto [instance, error] = CreateVulkanInstance();
		if(error != VK_SUCCESS) {
			PrintVulkanResult(error, "failed to create vulkan instance");
		}

		msg("main loop...");
		while(gameWindow.isOpen()) {
			gameWindow.processMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		msg("cleaning up...");
	}
	msg("done!");
	return 0;
}
