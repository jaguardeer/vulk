#include <iostream>
#include "vulkan_device.hxx"
#include "VkResult_to_CString.hxx"
using namespace std;
using namespace Vulkan_Device;

Result<VkInstance> Create_Instance()
{
	return {VK_NULL_HANDLE, VK_SUCCESS};
}

Result<VkInstance> Create_Error()
{
	return {VK_NULL_HANDLE, VK_NOT_READY};
}

void print_error(VkResult error)
{
	cout << "This is ";
	if(!error) cout << "not ";
	cout << "an error: ";
	cout << VkResult_to_CString(error) << endl;
}

int main()
{
	const auto [instance,error] = Create_Instance();
	print_error(error);
	auto [i,e] = Create_Error();
	print_error(e);

	Device d;
	vkCreateDescriptorPool(
		d,
		nullptr,
		nullptr,
		nullptr
	);
}
