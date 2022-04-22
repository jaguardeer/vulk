#ifndef ENGINE_LIBRARY_ARRAY_HXX
#define ENGINE_LIBRARY_ARRAY_HXX

#include <cstdlib>
#include <cstddef>

namespace engine_library {

//a span refers to a region of Ts in memory. it does not own that memory
template <typename T>
class span
{
public:
	//TODO: can maybe restrict template using constexpr template + static_assert?
	template<typename I>//I should be an integral type. concepts would be cool here
	const T& operator[] (I i) {return _start[i];}
private:
	T* const _start;
	T* const _end;
};

//TODO - allow data to be passed in constructor (placement)
//dynamic array
template <typename T>
class darray
{
public:
	inline 	  T*  		data 	() 	const { return _data; 	}
	inline 	  size_t 	length 	() 	const { return _length; }
	constexpr size_t 	size 	()	const { return _length * sizeof(T); }
	template<typename I>//I should be unsigned integral type
	constexpr T& operator[] (I i) 	const { return _data[i]; }
	constexpr T* begin 		()		const {	return _data;		  }
	constexpr T* end 		()		const {	return _data+_length; }

	//TODO: this should swap elements into the new darray
	//TODO: realloc or malloc and move
	void resize(size_t const num_elements)
	{
		_length = num_elements;
		delete[] _data;
		_data = new T[num_elements];
	}

	//Constructor, Destructor
	//TODO: malloc + construct in place?
	darray()
	:	_length	(0),
		_data 	(nullptr)
	{}
	darray(size_t const num_elements)
	:	_length	(num_elements),
		_data	(new T[num_elements])
	{}
	~darray()
	{
		delete[] _data;
	}
	//Move Constructor
	darray(darray&& other)
	:	_length	(other._length),
		_data	(other._data)
	{
		//other._length = 0;
		other._data = nullptr;
	}
	//Move Assignment
	darray& operator= (darray&& other)
	{
		if(this == &other) return *this;
		_length = other._length;
		_data   = other._data;
		other._data = nullptr;
		return *this;
	}
	//No copy (yet)
	darray			  (darray const &) = delete;
	darray& operator= (darray const &) = delete;
private:
	size_t	_length ;
	T*		_data	;

//below this line is TODO
//public:
	//Guaranteed copy elision doesn't work? Cygwin GCC 7.3.0
	//array(array const &) = delete;
	//array& operator= (array const &) = delete;
	//array(array&&) 		 = delete;
	//array& operator= (array &&) = delete;

	/*
	array(array const& other)
	:	_length	(other._length),
		_data	(other._data)
	{
		exit(1);
	}
	*/
};

//constant-sized array
template <size_t N, typename T>
class carray
{
public:
	constexpr size_t length() const   {return N;}
	constexpr 		T* data()		  {return &_data[0];}
	constexpr const T* data() const	  {return &_data[0];}
	constexpr 		T& operator[] (size_t i) 	   {return _data[i];}
	constexpr const T& operator[] (size_t i) const {return _data[i];}

	constexpr 		T* begin() 		 {return &_data[0];}
	constexpr const T* begin() const {return &_data[0];}
	constexpr 		T* end  () 		 {return &_data[N];}
	constexpr const T* end  () const {return &_data[N];}

	T _data[N];
};

//template <size_t N, typename T>





}//namespace engine_library
#endif
