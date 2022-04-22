#ifndef MINMAX_HXX
#define MINMAX_HXX

namespace engine_library{

template <typename T>
T max (T&& left, T&& right)
{
	if( left > right ) return left;
	else return right;
}

template <typename T>
T min (T&& left, T&& right)
{
	if( left < right ) return left;
	else return right;
}

//TODO: forwarding reference?
template <typename T, typename L, typename U>
T clamp ( T&& value, L&& lower, U&& upper)
{
	if( value < lower ) return lower;
	else if( value > upper ) return upper;
	else return value;
}


}
#endif
