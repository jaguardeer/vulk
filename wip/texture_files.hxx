#ifndef ENGINE_ASSET_LOADER
#define ENGINE_ASSET_LOADER
#include "data_structs.hxx"

namespace Texture_Files{

enum class Error
{
	no_error = 0,
	file_not_found,
	invalid_format
};

template <typename T>
class Result{
	T 	value;
	int error;
};

enum class Filetype
{
	invalid = 0,//	not a valid texture filetype
	bmp,		//	*.bmp
	texture,	//	*.texture
};

Filetype Determine_Filetype(const char* const filename)
{

}

Result<Texture_Data> Load_Texture(const char* const filename)
{
	const auto&& filetype = Determine_Filetype(filename);
	if(filetype == Filetype::invalid) return Error::invalid_format;
}

Texture_Data Load_Texture
(
	const char* filename,
	const Filetype filetype
){

}

template <Filetype>
Texture_Data& Load_Texture (const char* const filename) = delete;

template<>
Texture_Data& Load_Texture<Filetype::texture>(const char* const filename)
{
	Texture_Data texture;
}


//does asset loading even need a class?
class Asset_Loader
{

};

}//namespace Texture_Files
#endif
