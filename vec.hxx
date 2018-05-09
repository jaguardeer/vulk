#ifndef VEC_HPP
#define VEC_HPP
#include <cmath>
//TODO: namespace this
//TODO - same members by different names? do it anyway and suppress warnings?
static constexpr auto pi = acos(-1);

struct vec2
{
	/*
	union
	{
		struct {
			float x,y;
		};
		struct {
			float u,v;
		};
	};
	*/
	float x, y;

	constexpr vec2 operator*(const float f) const
	{
		return {x*f, y*f};
	}

	vec2& operator+=(const vec2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2& operator-=(const vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	bool operator==(const vec2& other) const
	{
		return x==other.x && y==other.y;
	}


	float length() const
	{
		return sqrtf( x*x + y*y );
	}
};

constexpr vec2 operator* (const float f, const vec2 vec)
{
	return vec * f;
}

struct vec3
{
	/*
	union
	{
		struct {
			float r,g,b;
		};
		struct {
			float x,y,z;
		};
	};
	*/
	float x, y, z;

	constexpr vec3 operator-() { return {-x, -y, -z}; }

	constexpr vec3& operator-= (const vec3 other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	constexpr vec3& operator+= (const vec3 other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	float length() const
	{
		return sqrtf( x*x + y*y + z*z );
	}

	vec3 normal() const
	{
		const float l = length();
		if(l == 0) return {0,0,0};
		return { x/l, y/l, z/l};
	}

	static constexpr vec3 zero()
	{
		return {0,0,0};
	}

	bool operator==(const vec3& other) const
	{
		return x==other.x && y==other.y && z==other.z;
	}

	vec3() = default;
	constexpr vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	constexpr vec3(const vec2 v, const float _z) : vec3(v.x, v.y, _z) {}
	constexpr vec3(vec2 v) : vec3(v.x, v.y, 0) {}
};

struct mat4
{
	//	column-major layout - consecutive elements are same column
	float data[4][4];//access using [col][row]
	//todo - use data[16]?;

	mat4 operator* (mat4 right) const
	{
		const mat4 left = *this;
		mat4 product = {};
		for(int r = 0; r < 4; r++)
		for(int c = 0; c < 4; c++)
		for(int i = 0; i < 4; i++)
			product.data[c][r] += left.data[i][r] * right.data[c][i];
		return product;
	}

	mat4& operator*= (const mat4& left)
	{
		const mat4&& result = left * (*this);
		*this = result;
		return *this;
	}

	static constexpr mat4 identity()
	{
		return {{
			{1,0,0,0},// x column
			{0,1,0,0},// y column
			{0,0,1,0},// z column
			{0,0,0,1} // w column
		}};
	}

	static inline mat4 translation(const vec3 vec)
	{
		return translation(vec.x, vec.y, vec.z);
	}
	//todo - rename back to rotate, translate, etc
	static inline mat4 translation(float x, float y, float z = 0)
	{
		return {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{x,y,z,1}
		}};
	}

	static inline mat4 scale(float x, float y, float z)
	{
		return {{
			{x,0,0,0},
			{0,y,0,0},
			{0,0,z,0},
			{0,0,0,1}
		}};
	}
	static inline mat4 scale(float x, float y) { return scale(x,y,1); }
	static inline mat4 scale(float xyz) { return scale(xyz,xyz,xyz); }

	static inline mat4 perspective(const float degFovX, const float aspectRatio, const float nearPlane, const float farPlane)
	{
		const float S = 1 / tan ( degFovX / 2 * pi/180 );
		const float f = farPlane;
		const float n = nearPlane;

		return {{
			{S,0			,0,			0},
			{0,S*aspectRatio,0,			0},
			{0,0			,f/(f-n),	1},
			{0,0			,f*(1-f/(f-n)),	0}
		}};
	}

	static inline mat4 perspective_infinite(const float degFovX, const float aspectRatio)
	{
		const float S = 1 / tan ( degFovX / 2 * pi/180 );
		constexpr float nearplane = 0.001;

		return {{
			{S,0			,0,	0},
			{0,S*aspectRatio,0,	0},
			{0,0			,1,	1},
			{0,0			,-nearplane,	0}
		}};
	}

	//todo
	static inline mat4 lookAt(const vec3 origin, const vec3 destination, const vec3 upVector)
	{
		return {{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
		}};
	}
	//todo
	static inline mat4 rotation(const float degrees, const vec3 rotAxis)
	{
		const vec3 axis = rotAxis.normal();
		const float _cos = cos( degrees * pi/180 );
		const float _sin = sin( degrees * pi/180 );
		float x = axis.x;
		const float y = axis.y;
		const float z = axis.z;
		if( axis == vec3::zero() ) x = 1;
		const float xx = x*x;
		const float xy = x*y;
		const float xz = x*z;
		const float yy = y*y;
		const float yz = y*z;
		const float zz = z*z;
		const mat4 m = {{
			{ _cos+xx*(1-_cos)  ,xy*(1-_cos)+z*_sin , xz*(1-_cos)-y*_sin ,0},
			{ xy*(1-_cos)-z*_sin,  _cos+yy*(1-_cos) , yz*(1-_cos)+x*_sin ,0},
			{ xz*(1-_cos)+y*_sin,yz*(1-_cos)-x*_sin , _cos+zz*(1-_cos)   ,0},
			{ 0					,0					, 0					 ,1}
		}};
		return m;
	}
};


#endif
