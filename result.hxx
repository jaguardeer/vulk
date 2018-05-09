#ifndef ENGINE_LIBRARY_RESULT
#define ENGINE_LIBRARY_RESULT

namespace engine_library{

template <typename T, typename E>
struct Result
{
	T value;
	E error;	// should an E default-construct to FALSE?

	//TODO: allow Result(T _value)?
	//Result() = default;
	//Result(T _value			 ) : value(_value)				  {}
	Result(			 E _error) : 				error(_error) {}
	Result(T _value, E _error) : value(_value), error(_error) {}
};


}//namespace engine_library
#endif

/*
//TODO - put this in engine_library or something?
template <typename T1, typename T2>
struct pair
{
	T1 first;
	T2 second;
};
Result<VkBuffer, VkMemory>
*/
