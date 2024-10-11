#include <integer.hpp>

int main() {
	water::integer::unsigned_integer x{ 1 };

	for (int i = 0; i < 32; i++) {
		x = x + x;
	}
	x = x + x;

	std::cout << x << std::endl;
	return 0;
}