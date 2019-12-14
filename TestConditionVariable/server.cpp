/*
	Multithreaded Hello World server in C
*/

#include <iostream>

using std::cin;
using std::cout;

int main()
{	
	std::string command;
	int a;
	while (true)
	{
		cin >> a;
		if (a == 9) { break; }
		else { cout << a; }
	}

	return 0;
}