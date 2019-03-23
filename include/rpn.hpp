// rpn.hpp - define a namespace holding rpn conversion functions

#pragma once

#include <memory>
#include <string>

namespace RPN {
	// Get RPN statement from infix expression
	std::string FromInfix(std::string infix);

	// Evaluate an RPN statement
	double Evaluate(std::string rpn);

	// Convert RPN statement to infix expression
	std::string ToInfix(std::string infix);
}
