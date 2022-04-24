#ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
#define ENGINE_LIBRARY_VULKAN_INSTANCE

#include <vector>
#include <vulkan/vulkan.h>
#include <el/util.hpp>


namespace engineLibrary::vulkan{

	class Instance {
		public:

		static Result<Instance, VkResult> Create(const VkInstanceCreateInfo &createInfo);

		// convert to VkInstance
		operator VkInstance() const;
		// structors
		Instance() = delete;
		~Instance();
		// move
		Instance& operator= (Instance&& other);
		Instance (Instance &&other);
		// non copyable
		Instance(const Instance&) = delete;
		Instance& operator= (const Instance&) = delete;

		// vulkan funcs
		template<class T> using list = std::vector<T>; // TODO: move this into another header to unify across classes?
		list<VkPhysicalDevice> enumeratePhysicalDevices() const;

		private:

		explicit Instance(const VkInstance id_);
		VkInstance id;
	};

} // namespace engineLibrary::vulkan
#endif // #ifndef ENGINE_LIBRARY_VULKAN_INSTANCE
