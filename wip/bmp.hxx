#ifndef BMP_THING
#define BMP_THING
#include <iostream>
#include <fstream>
#include <cstring>//for memcpy
#include <iomanip>
#include <vulkan/vulkan.h>
using std::cout;
using std::endl;

namespace engine_library {

//todo - move to utility
template <typename T>
class unaligned
{
	uint8_t value[sizeof(T)];

public:
	operator T() const
	{
		T r;
		memcpy(&r,value,sizeof(T));
		return r;
	}
	T& operator=(const T& other) {*reinterpret_cast<T*>(value) = other;}
};

template <typename T>
class Unaligned_Struct
{
public:
	T& readFile(const char* filename)
	{
		std::ifstream file(filename, std::ios::binary);

		if (!file.is_open()) {//todo
			std::cout << "failed to open file! - " << filename << std::endl;
			exit(420);
		}
		//std::cout << "sizeofthis: " << sizeof(T) << std::endl;
		file.read(reinterpret_cast<char*>(this),sizeof(T));
		file.close();
		return *reinterpret_cast<T*>(this);
	}
};

//see https://en.wikipedia.org/wiki/BMP_file_format
struct BITMAPINFOHEADER
{
	unaligned<uint32_t> header_size;
	unaligned<int32_t> bitmap_width;
	unaligned<int32_t> bitmap_height;
	unaligned<uint16_t> num_color_planes;
	unaligned<uint16_t> bits_per_pixel;
	unaligned<uint32_t> compression_method;
	unaligned<uint32_t> image_size;
	unaligned<uint32_t> horizontal_resolution;
	unaligned<uint32_t> vertical_resolution;
	unaligned<uint32_t> num_palette_colors;
	unaligned<uint32_t> num_important_colors;
	unaligned<uint32_t> red_channel_bitmask;
	unaligned<uint32_t> green_channel_bitmask;
	unaligned<uint32_t> blue_channel_bitmask;
	unaligned<uint32_t> alpha_channel_bitmask;
};

struct BMP_Main_Header
{
	unaligned<uint8_t> signature[2];
	unaligned<uint32_t> fileSize;
	unaligned<uint16_t> reserved[2];
	unaligned<uint32_t> pixel_array_offset;
};

struct BMP_Header : Unaligned_Struct<BMP_Header>
{
	BMP_Main_Header main_header;
	//should use BITMAPINFOV5
	BITMAPINFOHEADER dib_header;

	
	/*
	void dump() const
	{
		for(int i = 0; i < sizeof(*this); i++)
		{
			std::cout << (void*)((uint8_t*)this)[i] << " ";
		}
		std::cout << std::endl;
	}
	*/

	void print() const
	{
		//main header
		cout << "signature: " << main_header.signature[0] << main_header.signature[1] << endl;
		cout << "total file size: " << main_header.fileSize << endl;
		cout << "reserved values: " << main_header.reserved[0] <<" "<< main_header.reserved[1] << endl;
		cout << "pixel array offset: " << main_header.pixel_array_offset << endl;
		//dib header
		cout << "dib header_size: " << dib_header.header_size << endl;
		cout << "bitmap width: " << dib_header.bitmap_width << endl;
		cout << "bitmap height: " << dib_header.bitmap_height << endl;
		cout << "num_color_planes: " << dib_header.num_color_planes << endl;
		cout << "bits_per_pixel: " << dib_header.bits_per_pixel << endl;
		cout << "compression_method: " << dib_header.compression_method << endl;
		cout << "image_size: " << dib_header.image_size << endl;
		cout << "horizontal_resolution: " << dib_header.horizontal_resolution << endl;
		cout << "vertical_resolution: " << dib_header.vertical_resolution << endl;
		cout << "num_palette_colors: " << dib_header.num_palette_colors << endl;
		cout << "num_important_colors: " << dib_header.num_important_colors << endl;
		cout << "red_channel_bitmask: " <<std::setfill('0') << std::setw(8)<<std::hex<< dib_header.red_channel_bitmask << endl;
		cout << "green_channel_bitmask: " <<std::setfill('0') << std::setw(8)<< dib_header.green_channel_bitmask << endl;
		cout << "blue_channel_bitmask: " <<std::setfill('0') << std::setw(8)<< dib_header.blue_channel_bitmask << endl;
		cout << "alpha_channel_bitmask: " <<std::setfill('0') << std::setw(8)<< dib_header.alpha_channel_bitmask <<std::dec <<endl;
	}
};

bool rgba_order (uint32_t first, uint32_t second, uint32_t third, uint32_t fourth);
bool rgba_order (uint32_t first, uint32_t second, uint32_t third, uint32_t fourth)
{
	return ( (first < second)&&(second < third)&&(third < fourth) );
}

//todo - refactor
class BMP_File
{
public:
	uint32_t getWidth() const ;
	uint32_t getHeight() const ;
	uint8_t* getPixelData() const;

	uint8_t getPixelRed ( const size_t which_pixel ) const
	{
		size_t red_byte_offset;
		switch(head.dib_header.compression_method)
		{
		case 0:
			red_byte_offset = 2;
			break;

		case 3:
			red_byte_offset = static_cast<size_t>(getBitmaskOffset(head.dib_header.red_channel_bitmask));
			break;
		default:
			red_byte_offset = 0xFFFFFFFF;
		}
		return getColorValue ( red_byte_offset, which_pixel);
	}

	uint8_t getPixelGreen ( const size_t which_pixel ) const
	{
		size_t green_byte_offset;
		switch(head.dib_header.compression_method)
		{
		case 0:
			green_byte_offset = 1;
			break;

		case 3:
			green_byte_offset = static_cast<size_t>(getBitmaskOffset(head.dib_header.green_channel_bitmask));
			break;
		default:
			green_byte_offset = 0xFFFFFFFF;
		}
		return getColorValue ( green_byte_offset, which_pixel);
	}

	uint8_t getPixelBlue ( const size_t which_pixel ) const
	{
		size_t blue_byte_offset;
		switch(head.dib_header.compression_method)
		{
		case 0:
			blue_byte_offset = 0;
			break;

		case 3:
			blue_byte_offset = static_cast<size_t>(getBitmaskOffset(head.dib_header.blue_channel_bitmask));
			break;
		default:
			blue_byte_offset = 0xFFFFFFFF;
		}
		return getColorValue ( blue_byte_offset, which_pixel);
	}

	uint8_t getPixelAlpha ( const size_t which_pixel ) const
	{
		const size_t alpha_byte_offset = static_cast<size_t>(getBitmaskOffset(head.dib_header.alpha_channel_bitmask));
		return getAlphaValue(alpha_byte_offset, which_pixel);
	}

	void print()
	{
		cout << "- - - " << current_file << " - - -" << endl;
		head.print();
		cout << "format: " << VkFormat_to_CString(getFormat()) << endl;
	}

	VkFormat lookupFormat3 ()
	{
		uint32_t r = head.dib_header.red_channel_bitmask;
		uint32_t g = head.dib_header.green_channel_bitmask;
		uint32_t b = head.dib_header.blue_channel_bitmask;
		uint32_t a = head.dib_header.alpha_channel_bitmask;

		if ( rgba_order(r,g,b,a) ) return VK_FORMAT_R8G8B8A8_UNORM;
		else if( rgba_order(b,g,r,a) ) return VK_FORMAT_B8G8R8A8_UNORM;
		else return VK_FORMAT_UNDEFINED;
	}

	VkFormat getFormat()
	{
		switch(head.dib_header.compression_method)
		{
		case 0:
			return VK_FORMAT_R8G8B8_UNORM;
		case 3:
			return lookupFormat3();
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	void readFile(const char* filename)
	{
		current_file = filename;
		readFileHeader();
		readPixelData();
	}

	//this should be responsibility of the engine.
	using Texture_Data = Graphics::Texture_Data;
	Texture_Data convertToTexture()
	{
		//select target format - see if BMP layout matches any VkFormat
		//select translation function(s) based on BMP Format vs target format
		
		//texture
		Texture_Data t;
		t.width = static_cast<uint32_t>(head.dib_header.bitmap_width);
		t.height = static_cast<uint32_t>(head.dib_header.bitmap_height);
		t.data = translateRawPixelData(pixel_data);//maybe do the bit swizzling here
		t.format = getFormat();
		t.format = VK_FORMAT_R8G8B8A8_UNORM;
		return t;
		/*
		for(uint8_t out_pixel[4] in pixel_data)
			out_pixel[0] = pixel_data.r;
		*/
	}

//private:
	//size_t pixel_data_size = 0;
	uint8_t* pixel_data = nullptr;//currently stored as RGBA
	const char* current_file = nullptr;
	BMP_Header head;


	//0+ means nth byte. -1 means no mask. use default value
	int getBitmaskOffset(uint32_t bitmask) const
	{
		if(bitmask == 0) return -1;
		int i = 0;
		while( bitmask > 0xFF )
		{
			bitmask >>= 8;
			i++;
		}
		return i;
	}

	uint8_t getAlphaValue(const size_t alpha_byte_offset, const size_t pixel_index) const
	{
		const size_t num_color_channels = head.dib_header.bits_per_pixel/8;
		if(num_color_channels < 4 || alpha_byte_offset == static_cast<size_t>(-1)) return 255;
		else return pixel_data[num_color_channels * pixel_index + alpha_byte_offset];
	}

	uint8_t getColorValue(const size_t color_byte_offset, const size_t pixel_index) const
	{
		const size_t num_color_channels = head.dib_header.bits_per_pixel/8;
		return pixel_data[num_color_channels * pixel_index + color_byte_offset];
	}

	//this should really go to something on the texture side
	//translates raw pixel data into a vulkan compatible format
	uint8_t* translateRawPixelData (uint8_t* raw_pixel_data)
	{
		//if( isVulkanCompatible(this.format) ) .....
		const uint32_t num_pixels = static_cast<uint32_t>(head.dib_header.bitmap_width * head.dib_header.bitmap_height);
		//allocate buffer for new pixel data
		uint8_t* new_pixel_data = new uint8_t[num_pixels * 4];

		//fill pixel data
		for(size_t i = 0; i < static_cast<size_t>(num_pixels); i++)
		{
			//todo - make this a functor or something to lookup value
			new_pixel_data[4*i+0] = getPixelRed(i);//red
			new_pixel_data[4*i+1] = getPixelGreen(i);//green
			new_pixel_data[4*i+2] = getPixelBlue(i);//blue
			new_pixel_data[4*i+3] = getPixelAlpha(i);//alpha
		}

		return new_pixel_data;
	}

	//this should change based on bmp format
	void readPixelData()
	{
		std::ifstream file(current_file, std::ios::binary);

		if (!file.is_open()) {//todo
			std::cout << "failed to open file!" << std::endl;
			exit(420);
		}

		//get data size and allocate space
		const int32_t num_pixels = head.dib_header.bitmap_width * head.dib_header.bitmap_height;
		const size_t sizeof_raw_pixel = head.dib_header.bits_per_pixel/8;
		const size_t pixel_data_size = static_cast<size_t>(num_pixels) * sizeof_raw_pixel;
		//slurp pixel data
		file.seekg(static_cast<int32_t>(head.main_header.pixel_array_offset));
		delete[] pixel_data;
		pixel_data = new uint8_t[pixel_data_size];
		file.read(reinterpret_cast<char*>(pixel_data), static_cast<std::streamsize>(pixel_data_size));
	}


	void readFileHeader()
	{
		//could go directly to dib_header_size to calculate size of initial read
		head.readFile(current_file);
	}

	//todo - this should go in util
	const char* VkFormat_to_CString (VkFormat format)
	{
		switch(format)
		{
			case VK_FORMAT_B8G8R8A8_UNORM:
			return "VK_FORMAT_B8G8R8A8_UNORM";

			case VK_FORMAT_R8G8B8A8_UNORM:
			return "VK_FORMAT_R8G8B8A8_UNORM";

			case VK_FORMAT_B8G8R8_UNORM:
			return "VK_FORMAT_B8G8R8_UNORM";

			default:
			return "VK_FORMAT_UNDEFINED";
		}
	}
};

}

#endif
