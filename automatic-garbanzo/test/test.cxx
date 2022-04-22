#include <iostream>
#include <windows.h>
using namespace std;

struct foob
{
	int a;
	static constexpr int pi = 420;//takes up 0 size
};

int main()
{
	foob foo;
	cout << sizeof(int) << endl;
	cout << sizeof(foob) << endl;

	return 0;
}
