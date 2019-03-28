// rpn.cpp - implement the rpn functions

#include "rpn.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace RPN {
	namespace {
		// Private helper functions

		// Array of all possible operators
		// Ordered in priority
		std::vector<std::string> operators {
			"^",
			"*",
			"/",
			"+",
			"-"
		};

		// A struct that represents a node in a binary tree
		struct RPNTree {
			// Pointers to the left and right nodes
			std::shared_ptr<RPNTree> left;
			std::shared_ptr<RPNTree> right;

			// Weak pointer to this node's parent
			// So child doesn't own parent and cause memory leak			
			std::weak_ptr<RPNTree> parent;

			// String representing the number or operation this node represents
			std::string data;

			// Constructor for the RPN tree
			RPNTree(std::string data) : left(nullptr), right(nullptr),
				parent(), data(data) {}
		};

		// Get parts
		// Split a given string by spaces, returning a vector
		std::vector<std::string> Split(std::string expression) {
			std::string buffer = "";
			std::vector<std::string> results;

			// Iterate through each character in the string
			for(auto ch : expression) {
				// If space, push buffer to vector
				if(ch == ' ' && buffer != "") {
					results.push_back(buffer);
					buffer = "";
				}
				// Else, push char to buffer
				else if (ch != ' ') {
					buffer += ch;
				}
			}

			// Ensure buffer is emptyed
			if(buffer != "") {
				results.push_back(buffer);
			}

			return results;
		}

		// Perform an inorder traversal on a given RPNTree
		void Inorder(std::shared_ptr<RPNTree> tree,
			std::shared_ptr<std::string> results) {
			// Traverse left tree if not null
			if(tree->left != nullptr) {
				// Add brackets if child is an operator
				auto opcode = std::find(operators.begin(), operators.end(), tree->left->data) != operators.end();

				if(opcode) {
					*results += "(";
				}

				Inorder(tree->left, results);

				if(opcode) {
					*results += ")";
				}
			}

			*results += tree->data;

			if(tree->right != nullptr) {
				auto opcode = std::find(operators.begin(), operators.end(), tree->right->data) != operators.end();

				if(opcode) {
					*results += "(";
				}

				Inorder(tree->right, results);
				
				if(opcode) {
					*results += ")";
				}
			}
		}
	}

	std::vector<std::string> Operators() {
		return operators;
	}

	// Get RPN statement from infix expression
	std::string FromInfix(std::string infix) {
		// Create a vector to hold the results
		std::vector<std::string> result;

		// Create stack to hold the operators
		std::stack<std::string> stack;

		// Create buffer to hold current number being processed
		std::string buffer = "";

		// Iterate over all chars in the passed string
		for(auto ch : infix) {
			// Get position of character in the operators list
			auto index = std::find(operators.begin(), operators.end(),
				std::string(1, ch));

			// If ch is an operator
			if(index != operators.end()) {
				if(buffer != "") {
					result.push_back(buffer);
					buffer = "";
				}

				// If stack is empty or contains left parenthasis
				if(stack.size() == 0 || stack.top() == "(") {
					// Push to stack
					stack.push(*index);
				}

				// Else, pop the stack until an operator is found with a 
				// prescedence lower or greater (dependant on associative-ness)
				// than the current operator, the push the current operator to
				// the stack
				else if (index - operators.begin() == 0) {
					// '^' is first item in operators list and right-associative
					// NOTE: as ^ is the first item in the operators list, it
					// always has the highest precedence, so can just be pushed
					// to the stack, this will have to be rethought if more
					// operators are added
					stack.push(*index);
				}
				else {
					// All other operators in the list are left-associative
					// Therefore, pop all operators off the stack to the output
					// until the operator atop the stack has a prescedence less
					// than or equal to that of the character

					// Get index of the top of the stack
					auto top = std::find(operators.begin(), operators.end(),
						stack.top());
					
					// While item on stack has greater prescedence
					// ie. appears first in list, so has smaller iterator
					while(top < index) {
						// Push to output
						result.push_back(stack.top());
						stack.pop();

						// Check stack contains item
						if(stack.size() == 0) {
							break;
						}
						else {
							top = std::find(operators.begin(), operators.end(),
								stack.top());
						}				
					}

					// Then, push operator to stack
					stack.push(*index);
				}
			}
			// If ch is a left parenthesis
			else if(ch == '(') {
				// Empty string buffer
				if(buffer != "") {
					result.push_back(buffer);
					buffer = "";
				}

				// Push '(' to the stack
				stack.push("(");
			}
			// Right parenthesis
			else if(ch == ')') {
				// Empty string buffer
				if(buffer != "") {
					result.push_back(buffer);
					buffer = "";
				}

				// Pop and push the contents of the stack until a ( is found
				while(stack.top() != "(") {
					result.push_back(stack.top());
					stack.pop();
				}

				// Pop the "("
				stack.pop();
			}
			// If ch is a space
			else if(ch == ' ') {
				// Empty string buffer
				if(buffer != "") {
					result.push_back(buffer);
					buffer = "";
				}
			}
			// Else, ch is an operand
			else {
				// Add to buffer
				buffer += ch;
			}
		}

		// Push string buffer		
		if(buffer != "") {
			result.push_back(buffer);
			buffer = "";
		}

		// Push all operators on stack to result string
		while(stack.size() != 0) {
			result.push_back(stack.top());
			stack.pop();
		}

		// Push every entry in result to a string, seperated by a space
		std::string str;
		for(auto i : result) {
			str += i;
			str += " ";
		}

		return str;
	}

	// Evaluate an RPN statement
	double Evaluate(std::string string) {
		// Check string is not null
		if(string == "") {
			throw std::invalid_argument("string passed is null");
		}

		// Split the string into it's parts
		auto rpn = Split(string);

		// Check that values were found
		if(rpn.size() == 0) {
			throw std::invalid_argument("string passed contained no symbols");
		}

		// Create a stack to hold the values being processed
		std::stack<double> stack;

		// Iterate through the seperate parts of the expression
		for(auto exp : rpn) {
			// Get location of exp in op list
			auto pos = std::find(operators.begin(), operators.end(), exp);

			// Check if expression is an operator
			if(pos != operators.end()) {
				// Check there are at least two numbers on stack
				if(stack.size() < 2) {
					// Throw exception
					throw std::invalid_argument("operation attempted without at least 2 operands at operator: " + exp);
				}

				// Pop the two operands off the stack - note the order!!
				auto b = stack.top();
				stack.pop();
				auto a = stack.top();
				stack.pop();

				double result;

				// Switch based on chosen operator
				switch(pos - operators.begin()) {
					case 0: result = std::pow(a, b); break;
					case 1: result = a * b; break;
					case 2: result = a / b; break;
					case 3: result = a + b; break;
					case 4: result = a - b; break;
					default: throw std::runtime_error("operator " + exp + " not implemented");
				}

				stack.push(result);
			}
			// Else, attempt to convert to double and push to stack
			else {
				try {
					stack.push(std::stod(exp));
				}
				catch(const std::invalid_argument& e) {
					throw std::invalid_argument("value in rpn statement that is neither a number nor an operator at: " + exp);
				}
				catch(const std::out_of_range& e) {
					throw std::invalid_argument("value in statement too large");
				}
			}
		}

		// Return the number on top of the stack
		return stack.top();
	}

	// Convert RPN statement to infix expression
	std::string ToInfix(std::string string) {
		// Seperate out the parts of the rpn expression
		auto rpn = Split(string);

		// Check that expression contained something
		if(rpn.size() == 0) {
			throw std::invalid_argument("string passed contained no symbols");
		}

		// Reverse the expression
		std::reverse(rpn.begin(), rpn.end());

		// Hold a pointer to the top of the tree so that it doesn't get deleted
		// Create another pointer tree, which indicates at what point in the
		// tree the program currently is
		auto root = std::make_shared<RPNTree>(rpn[0]);
		auto tree = root;

		// Remove the first element of the expression (already created)
		rpn.erase(rpn.begin());

		// Produce a tree representing the rpn tree, prioritising right subtrees
		// over left subtrees
		for(auto exp : rpn) {
			// Ensure there is space to place a node
			// If not, move up the tree
			while(tree != nullptr && tree->right != nullptr && tree->left != nullptr) {
				tree = tree->parent.lock();
			}

			// If there were no free branches in the rpn tree, this rpn
			// expression is invalid
			if(tree == nullptr) {
				throw std::invalid_argument("invalid rpn expression at: " + exp);
			}

			// If character is an operator
			if(std::find(operators.begin(), operators.end(), exp) != operators.end()) {
				// Insert in availible place and then move to new subtree
				if(tree->right == nullptr) {
					tree->right = std::make_shared<RPNTree>(exp);
					tree->right->parent = tree;
					tree = tree->right;
				}
				else if(tree->left == nullptr) {
					tree->left = std::make_shared<RPNTree>(exp);
					tree->left->parent = tree;
					tree = tree->left;
				}
			}
			// Else, presume exp is a number and place in free position beneath
			// the tree node
			else {
				if(tree->right == nullptr) {
					tree->right = std::make_shared<RPNTree>(exp);
					tree->right->parent = tree;
				}
				else if(tree->left == nullptr) {
					tree->left = std::make_shared<RPNTree>(exp);
					tree->left->parent = tree;
				
				}
			}
		}

		// Perform inorder tree traversal
		auto results = std::make_shared<std::string>("");
		Inorder(root, results);

		return *results;
	}
}
