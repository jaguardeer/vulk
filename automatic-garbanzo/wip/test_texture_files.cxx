#include "texture_files.hxx"
#include <iostream>
using namespace std;
using namespace Texture_Files;

int main()
{
	const auto tex = Load_Texture("test.bmp");
	cout << "hey" << endl;
	return 0;
}
