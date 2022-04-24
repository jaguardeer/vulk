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

int main() {
	msg("making main window...");
	Window gameWindow;
	gameWindow.initWindow();

	msg("main loop...");
	while(gameWindow.isOpen()) {
		gameWindow.processMessages();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	msg("cleaning up...");

	return 0;
}
