#include <vulkan/vulkan.h>

#include <iostream>
#include <el/VkResultString.hpp>

using namespace std;
using namespace EngineLibrary;

int main() {
	std::cout << VK_SUCCESS << std::endl;
	std::cout << GetVkResultString(VK_SUCCESS) << std::endl;
	return 0;
}
