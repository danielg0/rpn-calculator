// main.cpp

#include "rpn.hpp"

#include <iostream>
#include <string>

int main() {
	std::string input = "4 + 3";
	std::cout << input << "\n";

	do {
		auto calc = RPN::FromInfix(input);
		
		try {
			std::cout << calc << "= " << RPN::Evaluate(calc) << "\n";
		}
		catch (const std::invalid_argument& e) {
			std::cout << "invalid rpn statement\n";
		}

		std::getline(std::cin, input);
	} while(input != "");

	return 0;
}