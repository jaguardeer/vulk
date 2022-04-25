// system headers
#include <vulkan/vulkan.h>
#include <iostream>
#include <thread>
#include <vector>
#include <array>

// engineLibrary headers
#include <el/Window.hpp>
#include <el/VkResultString.hpp>
#include <el/Instance.hpp>
#include <el/util.hpp>

// test headers
#include "test-common.hpp"

// add test that counts destructions?

using std::cout;
using std::endl;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;

static auto isDeviceSuitable() {

}

int main() {
	{
		msg("running app...");

		msg("making main window...");
		Window gameWindow;
		gameWindow.initWindow();

		msg("creating vulkan instance...");
		const auto instance = CreateInstance();



		return 0;
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
