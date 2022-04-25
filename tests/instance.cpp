#include <cstdlib>
#include <vulkan/vulkan.h>
#include <iostream>

#include "test-common.hpp"
#include <el/Instance.hpp>

using std::cout;
using std::endl;

using namespace engineLibrary;
using namespace engineLibrary::vulkan;


int main() {
	cout << "creating vulkan instance..." << endl;
	auto instance = CreateInstance();

	instance.enumeratePhysicalDevices();
	//instance.enumeratePhysicalDeviceGroups();
	//instance.getInstanceProcAddr();
}
