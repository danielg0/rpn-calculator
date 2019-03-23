// main.cpp

#include "rpn.hpp"

#include <iostream>
#include <string>

int main() {
	std::string input = "4 3 +";
	do {
		try {
			std::cout << RPN::ToInfix(input) << " = " << RPN::Evaluate(input) << "\n";
		}
		catch (const std::invalid_argument& e) {
			std::cout << "invalid rpn statement\n";
		}

		std::getline(std::cin, input);
	} while(input != "");

	return 0;
}