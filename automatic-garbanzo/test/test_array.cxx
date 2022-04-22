#include <iostream>
#include <typeinfo>
#include "array.hxx"

using engine_library::array;
using engine_library::carray;

using namespace std;

void test_array()
{
	array<int> foo(5);

	for(auto i = 0u; i < foo.length(); ++i)
		foo[i] = i;

	for(auto&& i : foo)
		cout << i << endl;
}

void test_carray_const_char()
{
	carray<3,const char*> foo = {
		"gucci",
		"mane",
		"baby"
	};

	for(auto&& s : foo)
	{
		cout << s << endl;
	}

	carray<foo.length(),int> bar = {1,2,3};
	carray<3,int> baz = bar;
	for(auto&& i : bar)
	{
		cout << i << endl;
	}

	baz = {420,4,2};
	baz[2] = bar[0];
	for(auto&& i : baz)
	{
		cout << i << endl;
	}

	auto fu = {"wtf","is","this"};
	cout << typeid(fu).name() << endl;
	cout << typeid(baz).name() << endl;
	//static_assert(decltype(fu) == decltype(const char*[3]));

}

void test_carray()
{
	carray<5,int> foo;
	for(auto i = 0u; i < foo.length(); ++i)
		foo[i] = i;

	for(auto&& i : foo)
		cout << i << endl;

	//int bar[5] = {5,4,3,2,1};
	carray<5,int> bar = {5,4,3,2,1};

	for(auto&& i : bar)
		cout << i << endl;
}

void test_move()
{

}

int main()
{
	//test_array();
	test_carray();
	test_carray_const_char();
}
