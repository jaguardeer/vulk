//data_structs.hxx
//defines types used to pass info between engine components
//most of these structs are filled by Resource_Loader
#ifndef ENGINE_DATA_STRUCTS
#define ENGINE_DATA_STRUCTS
#include "renderer.hxx"
#include "array.hxx"

namespace Graphics{

struct Mesh_Data
{
	template <typename T>
	using darray = engine_library::darray<T>;
	darray<uint16_t> indexes;
	darray<Renderer::vertex> vertexes;
};

struct Texture_Data
{
	uint32_t width;
	uint32_t height;
	VkFormat format;
	void*	 data;
};

}//namespace Graphics
#endif
