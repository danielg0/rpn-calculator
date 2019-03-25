// main.cpp

#include "rpn.hpp"

#include <iostream>
#include <string>

int main() {
	std::string input = "4 3 +";
	do {
		std::cout << RPN::FromInfix(input) << "\n";

		std::getline(std::cin, input);
	} while(input != "");

	return 0;
}