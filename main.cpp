#include <iostream>
#include <vector>
#include <algorithm>
#include "circular_buffer.h"
#include "dynamic_circular_buffer.h"

int main()
{
	circular_buffer <int, 3> a = { 1,2,1 };
	//a.push_back(4);
	a.push_back(std::move(4));
	std::vector<int> b = { 4,4,1 };

	return 0;
}
